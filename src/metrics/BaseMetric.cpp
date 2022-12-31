
/******************************************************************************
   DiffImg: image difference viewer
   Copyright(C) 2011-2014  xbee@xbee.net

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *******************************************************************************/

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include <cmath>

#include "BaseMetric.h"
#include "LogHandler.h"
#include "AppSettings.h"
#include "MiscFunctions.h"

int BaseMetric::defaultThresholdNumPixel = 100;
float BaseMetric::defaultThresholdMeanError = 0.3f;
float BaseMetric::defaultThresholdRMSError = 0.3f;
int BaseMetric::defaultThresholdMinError = 50;
int BaseMetric::defaultThresholdMaxError = 50;
float BaseMetric::defaultThresholdStandardDeviation = 0.3f;

// equalize is partially from qimageblitz
//   Copyright (c) 2003-2007 Clarence Dang <dang@kde.org>
//   Copyright (c) 2006 Mike Gashler <gashlerm@yahoo.com>
//   All rights reserved.

//   Redistribution and use in source and binary forms, with or without
//   modification, are permitted provided that the following conditions
//   are met:

//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.

//   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
//   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//   OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//   IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
//   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
//   THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

struct IntegerPixel {
    quint32 red{}, green{}, blue{}, alpha{};
};

inline bool equalize(QImage &img)
{
    if (img.isNull()) {
        return (false);
    }


    if (img.depth() < 32) {
        img = img.convertToFormat(img.hasAlphaChannel() ?
                                  QImage::Format_ARGB32 :
                                  QImage::Format_RGB32);
    }

    // form histogram
    IntegerPixel histogram [256];

    if (img.format() == QImage::Format_ARGB32_Premultiplied) {
        qWarning() << "Has alpha!";
        for (int line = 0; line < img.height(); line++) {
            QRgb *pixels = (QRgb *)img.scanLine(line);

            for (int i = 0; i < img.width(); ++i, ++pixels) {
                const QRgb pixel = qUnpremultiply(*pixels);
                histogram[qRed(pixel)].red++;
                histogram[qGreen(pixel)].green++;
                histogram[qBlue(pixel)].blue++;
                histogram[qAlpha(pixel)].alpha++;
            }
        }
    } else {
        for (int line = 0; line < img.height(); line++) {
            const uchar *pixels = img.scanLine(line);

            for (int i = 0; i < img.width(); ++i, pixels += 4) {
                histogram[pixels[0]].red++;
                histogram[pixels[1]].green++;
                histogram[pixels[2]].blue++;
                histogram[pixels[3]].alpha++;
            }
        }
    }

    // integrate the histogram to get the equalization map
    IntegerPixel map[256];
    IntegerPixel intensity;

    for (int i = 0; i < 256; ++i) {
        intensity.red += histogram[i].red;
        intensity.green += histogram[i].green;
        intensity.blue += histogram[i].blue;
        map[i] = intensity;
    }

    const IntegerPixel low = map[0];
    const IntegerPixel high = map[255];
    const uint32_t deltaRed = high.red - low.red;
    const uint32_t deltaGreen = high.green - low.green;
    const uint32_t deltaBlue = high.blue - low.blue;

    quint8 equalize_map_red[256]{0};
    quint8 equalize_map_green[256]{0};
    quint8 equalize_map_blue[256]{0};

    for (int i = 0; i < 256; ++i) {
        if (deltaRed) {
            equalize_map_red[i] = uint8_t((255 * (map[i].red - low.red)) / deltaRed);
        }

        if (deltaGreen) {
            equalize_map_green[i] = uint8_t((255 * (map[i].green - low.green)) / deltaGreen);
        }

        if (deltaBlue) {
            equalize_map_blue[i] = uint8_t((255 * (map[i].blue - low.blue)) / deltaBlue);
        }
    }

    // stretch the histogram and write
    uint8_t r, g, b;

    if (img.format() == QImage::Format_ARGB32_Premultiplied) {
        qWarning() << "Has alpha!";
        for (int line = 0; line < img.height(); line++) {
            QRgb *pixels = (QRgb *)img.scanLine(line);

            for (int i = 0; i < img.width(); ++i, ++pixels) {
                const QRgb pixel = qUnpremultiply(*pixels);
                r = (deltaRed) ? equalize_map_red[qRed(pixel)] : qRed(pixel);
                g = (deltaGreen) ? equalize_map_green[qGreen(pixel)] : qGreen(pixel);
                b = (deltaBlue) ?  equalize_map_blue[qBlue(pixel)] : qBlue(pixel);
                *pixels = qPremultiply(qRgba(r, g, b, qAlpha(pixel)));
            }
        }
    } else {
        for (int line = 0; line < img.height(); line++) {
            uchar *pixels = img.scanLine(line);
            if (deltaRed) {
                for (int i = 0; i < img.width(); ++i, pixels += 4) {
                    pixels[0] = equalize_map_red[pixels[0]];
                }
            }

            pixels = img.scanLine(line);
            if (deltaGreen) {
                for (int i = 0; i < img.width(); ++i, pixels += 4) {
                    pixels[1] = equalize_map_green[pixels[1]];
                }
            }

            pixels = img.scanLine(line);
            if (deltaBlue) {
                for (int i = 0; i < img.width(); ++i, pixels += 4) {
                    pixels[2] = equalize_map_blue[pixels[2]];
                }
            }
        }
    }

    return (true);
}


bool MetricParam::isValid()
{
    if (value.toFloat() > threshold.toFloat()) {
        return false;
    }

    return true;
}

void MetricParam::reset(bool all)
{
    value = defaultValue;

    if (all) {
        threshold = defaultValue;
    }
}

void MetricParam::setThreshold(const QString &valText)
{
    switch (uint(threshold.type())) {
    case QVariant::Int:
        threshold = valText.toInt();
        break;

    case QVariant::Double:
    case QMetaType::Float: {
        threshold = valText.toFloat();
        break;
    }

    default:
        break;
    }
}

// Branchless, pretty massive speed improvement especially for gain/offset
#define MIN_INT(x, y) (y ^ ((x ^ y) & -(x < y)))
#define MAX_INT(x, y) (x ^ ((x ^ y) & -(x < y)))
//#define MIN(x, y) (x < y ? x : y)
//#define MAX(x, y) (x > y ? x : y)

#define BOUND_INT(lower, x, upper) (MAX_INT(lower, MIN_INT(upper, int(x))))

static void applyGainOffset(QImage &img, double gain, double offset)
{
    if (img.depth() < 32) {
        img = img.convertToFormat(img.hasAlphaChannel() ?
                                  QImage::Format_ARGB32 :
                                  QImage::Format_RGB32);
    }

    if (img.format() == QImage::Format_ARGB32_Premultiplied) {
        qWarning() << "Have alpha channel!";
        for (int line = 0; line < img.height(); line++) {
            QRgb *pixels = (QRgb *)img.scanLine(line);

            for (int i = 0; i < img.width(); ++i, ++pixels) {
                const QRgb pixel = qUnpremultiply(*pixels);
                const uint8_t r = BOUND_INT(0, qRed(pixel)   * gain + offset, 255);
                const uint8_t g = BOUND_INT(0, qGreen(pixel) * gain + offset, 255);
                const uint8_t b = BOUND_INT(0, qBlue(pixel)  * gain + offset, 255);
                *pixels = qPremultiply(qRgba(r, g, b, qAlpha(pixel)));
            }
        }
    } else {
        for (int line = 0; line < img.height(); line++) {
            uchar *pixels = img.scanLine(line);

            for (int i = 0; i < img.width(); ++i, pixels += 4) {
                pixels[0] = BOUND_INT(0, pixels[0] * gain + offset, 255);
                pixels[1] = BOUND_INT(0, pixels[1] * gain + offset, 255);
                pixels[2] = BOUND_INT(0, pixels[2] * gain + offset, 255);
            }
        }
    }
}

static double findMinMax(const QImage &img, double *min, double *max)
{
    uint64_t mean = 0;

    if (img.depth() != 32) {
        qWarning() << "Invalid depth, converting" << img.depth();
        return findMinMax(
                img.convertToFormat(img.hasAlphaChannel() ?
                                  QImage::Format_ARGB32 :
                                  QImage::Format_RGB32),
                min, max
        );
    }

    int min_ = std::numeric_limits<int>::max();
    int max_ = std::numeric_limits<int>::min();

    if (img.format() == QImage::Format_ARGB32_Premultiplied) {
        for (int line = 0; line < img.height(); line++) {
            const QRgb *pixels = (const QRgb *)img.scanLine(line);

            for (int i = 0; i < img.width(); ++i, ++pixels) {
                const QRgb pixel = qUnpremultiply(*pixels);
                const int r = qRed(pixel);
                const int g = qGreen(pixel);
                const int b = qBlue(pixel);

                min_ = MIN_INT(r, min_);
                min_ = MIN_INT(g, min_);
                min_ = MIN_INT(b, min_);

                max_ = MAX_INT(r, max_);
                max_ = MAX_INT(g, max_);
                max_ = MAX_INT(b, max_);
                mean += r + g + b;
            }
        }
    } else {
        for (int line = 0; line < img.height(); line++) {
            const uchar *pixels = img.scanLine(line);

            for (int i = 0; i < img.width(); ++i, pixels += 4) {
                const int r = pixels[0];
                const int g = pixels[1];
                const int b = pixels[2];

                min_ = MIN_INT(r, min_);
                min_ = MIN_INT(g, min_);
                min_ = MIN_INT(b, min_);

                max_ = MAX_INT(r, max_);
                max_ = MAX_INT(g, max_);
                max_ = MAX_INT(b, max_);
                mean += r + g + b;
            }
        }
    }
    *max = max_;
    *min = min_;

    return double(mean) / (img.width() * img.height() * 3);
}


//---------------------------------------------------------------------------

BaseMetric::BaseMetric(QObject *parent) :
    QObject(parent),
    m_type("Unknown"),
    m_name(tr("Unknown")),
    m_desc(tr("No description")),
    m_valid(false),
    m_nbPixelError(0),
    m_maxError(0.0),
    m_minError(0.0),
    m_meanError(0.0),
    m_minImage1(0.0),
    m_minImage2(0.0),
    m_maxImage1(0.0),
    m_maxImage2(0.0),
    m_init(false),
    m_prevShowHistoZero(false)
{
}

BaseMetric::~BaseMetric()
{
    saveSettings();
    clearOutputParams();
}

void BaseMetric::init()
{
    if (m_init) {
        return;
    }

    createInputParams();
    createOutputParams();
    loadSettings();
    m_init = true;
}

MetricParam *BaseMetric::addInputParam(MetricParam *p)
{
    m_inputParams << p;
    return p;
}

MetricParam *BaseMetric::addOutputParam(MetricParam *p)
{
    m_outputParams << p;
    return p;
}

void BaseMetric::clearOutputParams()
{
    qDeleteAll(m_outputParams);
    m_outputParams.clear();
}

void BaseMetric::resetOutputParams()
{
    foreach (MetricParam *param, m_outputParams) {
        param->reset(false);
    }
}

void BaseMetric::createInputParams()
{
}

void BaseMetric::createOutputParams()
{
    // mean error
    addOutputParam(new MetricParam("MeanError", tr("Mean error"), tr("Mean absolute error"), BaseMetric::defaultThresholdMeanError));

    // min error
    addOutputParam(new MetricParam("MinError", tr("Min error"), tr("Minimum error"), 0));

    // max error
    addOutputParam(new MetricParam("MaxError", tr("Max error"), tr("Maximum error"), BaseMetric::defaultThresholdMaxError));

    addOutputParam(new MetricParam("StandardDeviation", tr("Standard deviation"), tr("Standard deviation"), BaseMetric::defaultThresholdStandardDeviation));

    // Root Mean Square
    addOutputParam(new MetricParam("RMS", tr("RMS error deviation"), tr("Root Mean Square error"), BaseMetric::defaultThresholdRMSError));

    // nb error
    addOutputParam(new MetricParam("ErrorNum", tr("Error num (pixels)"), tr("Number of different pixels"), BaseMetric::defaultThresholdNumPixel));

    // % error
    addOutputParam(new MetricParam("ErrorPercent", tr("Error (% pixels)"), tr("Number of different pixels in %"), 0.0f));
}

MetricParam *BaseMetric::getInputParam(const QString &name)
{
    foreach (MetricParam *param, m_inputParams) {
        if (param->type == name) {
            return param;
        }
    }

    return nullptr;
}

MetricParam *BaseMetric::getOutputParam(const QString &name)
{
    foreach (MetricParam *param, m_outputParams) {
        if (param->type == name) {
            return param;
        }
    }

    return nullptr;
}

QPixmap BaseMetric::getLogo() const
{
    return QPixmap("::/providers/providers/unknown.png");
}

const QString &BaseMetric::getType() const
{
    return m_type;
}

const QString &BaseMetric::getName() const
{
    return m_name;
}

const QString &BaseMetric::getDesc() const
{
    return m_desc;
}

const QList<ImageProperty> &BaseMetric::getProperties() const
{
    return m_properties;
}

const QList<MetricParam *> &BaseMetric::getInputParams()
{
    init();
    return m_inputParams;
}

const QList<MetricParam *> &BaseMetric::getOutputParams()
{
    init();
    return m_outputParams;
}

static int qimageChannelCount(const QImage &img)
{
    switch (img.format()) {
    case QImage::Format_Mono:
    case QImage::Format_MonoLSB:
    case QImage::Format_Alpha8:
    case QImage::Format_Grayscale8:
    case QImage::Format_Grayscale16:
        return 1;

    default:
        break;
    }

    if (img.hasAlphaChannel()) {
        return 4;
    } else {
        return 3;
    }
}


void BaseMetric::computeStandardProperties()
{
    // Dimension (pixels)
    m_properties << ImageProperty(tr("Dimension (pixels)"), tr("Dimension in pixel of images"), QString("%1x%2").arg(m_image1.width()).arg(m_image1.height()));

    // size (pixels)
    m_properties << ImageProperty(tr("Size (pixels)"), tr("Size in pixel of images"), MiscFunctions::pixelsToString(m_image1.width() * m_image1.height()));

    // size (bytes)
    int sz1 = MiscFunctions::getFileSize(m_file1);
    m_properties << ImageProperty(tr("Size file1"), tr("Size in bytes of image"), tr("%1/%2").arg(sz1).arg(MiscFunctions::bytesToString(sz1)));
    int sz2 = MiscFunctions::getFileSize(m_file2);
    m_properties << ImageProperty(tr("Size file2"), tr("Size in bytes of image"), tr("%1/%2").arg(sz2).arg(MiscFunctions::bytesToString(sz2)));

    // image format
    QMap<QString, QString> longFormats = MiscFunctions::getLongImageFormats();
    QString ext1 = QFileInfo(m_file1).suffix();
    QString fm1 = tr("Unknown");

    if (longFormats.contains(ext1)) {
        fm1 = QString("%1 (%2)").arg(longFormats[ext1]).arg(ext1);
    }

    m_properties << ImageProperty(tr("Format file1"), tr("Format of the image"), QString("%1").arg(fm1));

    QString ext2 = QFileInfo(m_file1).suffix();
    QString fm2 = tr("Unknown");

    if (longFormats.contains(ext2)) {
        fm2 = QString("%1 (%2)").arg(longFormats[ext2]).arg(ext2);
    }

    m_properties << ImageProperty(tr("Format file2"), tr("Format of the image"), QString("%1").arg(fm2));

    // bands
    m_properties << ImageProperty(tr("Band"), tr("Number of band in the image (3 for RGB image)"), QString::number(qimageChannelCount(m_image1)));

    // depth
    m_properties << ImageProperty(tr("Band depth"), tr("Bits per pixel"), QString::number(m_image1.depth()));

    m_minImage1 = 255.;
    m_minImage2 = 255.;
    m_maxImage1 = 0.;
    m_maxImage2 = 0.;

    findMinMax(m_image1, &m_minImage1, &m_maxImage1);
    findMinMax(m_image2, &m_minImage2, &m_maxImage2);
}

void BaseMetric::loadSettings()
{
    AppSettings settings;

    settings.beginGroup("Metrics");

    settings.beginGroup(m_type);

    settings.beginGroup("Input");

    foreach (MetricParam *param, m_inputParams) {
        QVariant t = settings.value(param->type, param->defaultValue);
        t.convert(param->defaultValue.type());
        param->threshold = t;
    }

    settings.endGroup();

    settings.beginGroup("Output");

    foreach (MetricParam *param, m_outputParams) {
        QVariant t = settings.value(param->type, param->defaultValue);
        t.convert(param->defaultValue.type());
        param->threshold = t;
    }

    QString typeUsed = settings.value("paramUsed", "").toString();

    if (!typeUsed.isEmpty()) {
        setDiscriminatingParam(getOutputParam(typeUsed));
    }

    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

void BaseMetric::saveSettings()
{
    AppSettings settings;

    settings.beginGroup("Metrics");

    settings.beginGroup(m_type);

    settings.beginGroup("Input");

    foreach (MetricParam *param, m_inputParams) {
        settings.setValue(param->type, param->threshold);
        qDebug() << m_type << " " << param->type << " = " << param->threshold;
    }

    settings.endGroup();

    settings.beginGroup("Output");

    foreach (MetricParam *param, m_outputParams) {
        settings.setValue(param->type, param->threshold);

        if (param->used) {
            settings.setValue("paramUsed", param->type);
        }
    }

    settings.endGroup();
    settings.endGroup();
    settings.endGroup();
}

bool BaseMetric::isValid() const
{
    return m_valid;
}

int BaseMetric::getPixelError() const
{
    return m_nbPixelError;
}

float BaseMetric::getMinError() const
{
    return m_minError;
}

float BaseMetric::getMaxError() const
{
    return m_maxError;
}

QImage BaseMetric::getImage1WithGain(double gain, double offset)
{
    if (qFuzzyCompare(gain, 1.) && qFuzzyCompare(offset, 0.)) {
        return m_image1;
    }

    QImage ret = m_image1;
    applyGainOffset(ret, gain, offset);
    return ret;
}

QImage BaseMetric::getImage2WithGain(double gain, double offset)
{
    if (qFuzzyCompare(gain, 1.) && qFuzzyCompare(offset, 0.)) {
        return m_image2;
    }

    QImage ret = m_image2;
    applyGainOffset(ret, gain, offset);
    return ret;
}

QImage BaseMetric::getImageDifferenceWithGain(double gain, double offset)
{
    if (qFuzzyCompare(gain, 1.) && qFuzzyCompare(offset, 0.)) {
        return m_imageDiff;
    }

    QImage ret = m_imageDiff;
    applyGainOffset(ret, gain, offset);
    return ret;
}

const QImage &BaseMetric::getImage1() const
{
    return m_image1;
}

const QImage &BaseMetric::getImage2() const
{
    return m_image2;
}

const QImage &BaseMetric::getImageDifference() const
{
    return m_imageDiff;
}

bool BaseMetric::saveDifference(const QString &file) const
{
    return m_imageDiff.save(file);
}

const QImage &BaseMetric::getImageMask() const
{
    return m_imageMask;
}

int BaseMetric::getDifferenceChannels() const
{
    return qimageChannelCount(m_imageDiff);
}

double BaseMetric::getMaxImage1() const
{
    return m_maxImage1;
}

double BaseMetric::getMaxImage2() const
{
    return m_maxImage2;
}

double BaseMetric::getMinImage1() const
{
    return m_minImage1;
}

double BaseMetric::getMinImage2() const
{
    return m_minImage2;
}

int BaseMetric::getImage1Channels() const
{
    return qimageChannelCount(m_image1);
}

int BaseMetric::getImage2Channels() const
{
    return qimageChannelCount(m_image2);
}

void BaseMetric::setDiscriminatingParam(MetricParam *p)
{
    foreach (MetricParam *param, m_outputParams) {
        if (param == p) {
            param->used = true;
        } else {
            param->used = false;
        }
    }
}

static QString description(const QImage &img, const int x, const int y)
{
    if (x < 0 || y < 0 || x >= img.width() || y >= img.height()) {
        return QString();
    }

    const QRgb pixel = img.pixel(x, y);
    QString ret;
    ret += QObject::tr("Red: %1,").arg(qRed(pixel));
    ret += QObject::tr("Green: %1,").arg(qGreen(pixel));
    ret += QObject::tr("Blue: %1,").arg(qBlue(pixel));

    if (img.hasAlphaChannel()) {
        ret += QObject::tr("Alpha: %1").arg(qAlpha(pixel));
    }

    return ret;
}

QString BaseMetric::getImage1Data(int x, int y) const
{
    return description(m_image1, x, y);
}

QString BaseMetric::getImage2Data(int x, int y) const
{
    return description(m_image2, x, y);
}

QString BaseMetric::getErrorData(int x, int y) const
{
    return description(m_imageDiff, x, y);
}

QString BaseMetric::getImage1Data(const QPoint &pt) const
{
    return getImage1Data(pt.x(), pt.y());
}

QString BaseMetric::getImage2Data(const QPoint &pt) const
{
    return getImage2Data(pt.x(), pt.y());
}

QString BaseMetric::getErrorData(const QPoint &pt) const
{
    return getErrorData(pt.x(), pt.y());
}

MetricParam *BaseMetric::getDiscriminatingParam() const
{
    foreach (MetricParam *param, m_outputParams)
        if (param->used == true) {
            return param;
        }

    return nullptr;
}

bool BaseMetric::checkImages()
{
    m_valid = false;

    // check loading of files
    if (m_image1.isNull()) {
        qWarning() << "can't read 1";
        LogHandler::getInstance()->reportError(QString("Can't read %1").arg(m_file1));
        return m_valid;
    }

    if (m_image2.isNull()) {
        qWarning() << "can't read 2";
        LogHandler::getInstance()->reportError(QString("Can't read %1").arg(m_file2));
        return false;
    }

    // check size
    if (m_image1.size() != m_image2.size()) {
        qWarning() << QString("Size mismatch (%1x%2)/(%3/%4)").arg(m_image1.width()).arg(m_image1.height()).arg(m_image2.width()).arg(m_image2.height());
        LogHandler::getInstance()->reportError(QString("Size mismatch (%1x%2)/(%3/%4)").arg(m_image1.width()).arg(m_image1.height()).arg(m_image2.width()).arg(m_image2.height()));
        return false;
    }

    m_valid = true;
    return m_valid;
}

bool BaseMetric::selectedStatsIsValid() const
{
    foreach (MetricParam *param, m_outputParams) {
        if (param->used && !param->isValid()) {
            return false;
        }
    }

    return true;
}

const QList<QPolygonF> &BaseMetric::getHistogramImage1()
{
    if (m_histoImage1.isEmpty()) {
        computeHisto(m_image1, m_histoImage1);
    }

    return m_histoImage1;
}

const QList<QPolygonF> &BaseMetric::getHistogramImage2()
{
    if (m_histoImage2.isEmpty()) {
        computeHisto(m_image2, m_histoImage2);
    }

    return m_histoImage2;
}

const QList<QPolygonF> &BaseMetric::getHistogramImageDiff(bool showZero)
{
    if (m_histoImageDiff.isEmpty() || m_prevShowHistoZero != showZero) {
        m_histoImageDiff.clear();
        computeHisto(m_imageDiff, m_histoImageDiff, !showZero);
    }

    m_prevShowHistoZero = showZero;
    return m_histoImageDiff;
}

void BaseMetric::computeHisto(const QImage &input, QList<QPolygonF> &polys, bool skipZeroLevel)
{
    QImage img(input); // CoW, so don't care

    if (img.format() != QImage::Format_ARGB32) {
        img = img.convertToFormat(QImage::Format_ARGB32);
    }

    polys.clear();

    if (img.isNull()) {
        return;
    }

    // form histogram
    IntegerPixel histogram [256];

    for (int line = 0; line < img.height(); line++) {
        QRgb *pixels = (QRgb *)img.scanLine(line);

        for (int i = 0; i < img.width(); ++i, ++pixels) {
            const QRgb pixel = *pixels;
            histogram[qRed(pixel)].red++;
            histogram[qGreen(pixel)].green++;
            histogram[qBlue(pixel)].blue++;
            histogram[qAlpha(pixel)].alpha++;
        }
    }

    QPolygonF points[3];

    for (int i = 0; i < 256; i++) {
        if (!skipZeroLevel || histogram[i].red) {
            points[0] << QPointF(i, histogram[i].red);
        }

        if (!skipZeroLevel || histogram[i].green) {
            points[1] << QPointF(i, histogram[i].green);
        }

        if (!skipZeroLevel || histogram[i].blue) {
            points[2] << QPointF(i, histogram[i].blue);
        }
    }

    polys << points[0] << points[1] << points[2];
    float maxX = 0;

    for (const QPolygonF &pol : polys) {
        for (const QPointF &p : pol) {
            maxX = std::max(maxX, float(p.x()));
        }
    }
}
QImage BaseMetric::createDiffMask(const QImage &img)
{
    QImage ret(img.width(), img.height(), QImage::Format_ARGB32_Premultiplied);
    ret.fill(Qt::transparent);

    for (int line = 0; line < img.height(); line++) {
        const QRgb *pixels = (const QRgb *)img.scanLine(line);
        QRgb *dest = (QRgb *)ret.scanLine(line);

        for (int i = 0; i < img.width(); ++i, ++pixels, ++dest) {
            const int val = qGray(*pixels);

            if (val == 0) {
                continue;
            }

            if (val > m_meanError) {
                *dest = qRgba(0, 255, 255, 255);
            } else {
                *dest = qRgba(0, 0, 255, 255);
            }
        }
    }

    return ret;
}

void BaseMetric::checkDifferences(const QString &file1, const QString &file2)
{
    m_file1 = file1;
    m_file2 = file2;
    m_valid = false;
    m_nbPixelError = 0;
    m_maxError = 0;
    m_minError = 0;
    m_meanError = 0;
    m_minImage1 = 0.0;
    m_minImage2 = 0.0;
    m_maxImage1 = 0.0;
    m_maxImage2 = 0.0;
    m_histoImage1.clear();
    m_histoImage2.clear();
    m_histoImageDiff.clear();

    m_properties.clear();
    init();
    resetOutputParams();

    m_image1 = QImage(m_file1);

    if (m_image1.size().isEmpty()) {
        LogHandler::getInstance()->reportDebug(QString("Can't load %1").arg(m_file1));
        return;
    }

    m_image2 = QImage(m_file2);

    if (m_image2.size().isEmpty()) {
        LogHandler::getInstance()->reportDebug(QString("Can't load %1").arg(m_file2));
        return;
    }

    if (m_image1.size() != m_image2.size()) {
        int maxWidth = qMax(m_image1.width(), m_image2.width());
        int maxHeight = qMax(m_image1.height(), m_image2.height());

        if (m_image1.width() < m_image2.width() && m_image1.height() < m_image2.height()) {
            m_image1 = m_image1.scaled(maxWidth, maxHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        } else if (m_image2.width() < m_image1.width() && m_image2.height() < m_image1.height()) {
            m_image2 = m_image2.scaled(maxWidth, maxHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        } else {
            m_image1 = m_image1.convertToFormat(QImage::Format_ARGB32).copy(0, 0, maxWidth, maxHeight);
            m_image2 = m_image2.convertToFormat(QImage::Format_ARGB32).copy(0, 0, maxWidth, maxHeight);
        }
    }

    // check compatibility
    if (!checkImages()) {
        return;
    }

    // compute "standard" properties
    computeStandardProperties();

    // perform the difference algorithm
    performDifference();

    // compute some statistics on difference image
    computeStatistics();

    //equalize(m_imageDiff);

    m_imageMask = createDiffMask(m_imageDiff);
}

static size_t countNonBlack(const QImage &img)
{
    size_t ret = 0;

    for (int line = 0; line < img.height(); line++) {
        QRgb *pixels = (QRgb *)img.scanLine(line);

        for (int i = 0; i < img.width(); ++i, ++pixels) {
            // I think it should use qGray(), but to preserve compatibility with opencv we do it the bad way
            //if (qGray(*pixels)) {
            if (qRed(*pixels) || qGreen(*pixels) || qBlue(*pixels)) {
                ret++;
            }
        }
    }

    return ret;
}

static double findStdDev(const QImage &input, const double mean)
{
    QImage img;

    if (input.depth() < 32) {
        img = input.convertToFormat(img.hasAlphaChannel() ?
                                    QImage::Format_ARGB32 :
                                    QImage::Format_RGB32);
    } else {
        img = input;
    }

    double stddev = 0;

    if (img.format() == QImage::Format_ARGB32_Premultiplied) {
        for (int line = 0; line < img.height(); line++) {
            QRgb *pixels = (QRgb *)img.scanLine(line);

            for (int i = 0; i < img.width(); ++i, ++pixels) {
                const QRgb pixel = qUnpremultiply(*pixels);
                stddev += (qRed(pixel) - mean) * (qRed(pixel) - mean);
                stddev += (qGreen(pixel) - mean) * (qGreen(pixel) - mean);
                stddev += (qBlue(pixel) - mean) * (qBlue(pixel) - mean);
            }
        }
    } else {
        for (int line = 0; line < img.height(); line++) {
            QRgb *pixels = (QRgb *)img.scanLine(line);

            for (int i = 0; i < img.width(); ++i, ++pixels) {
                const QRgb pixel = *pixels;
                stddev += (qRed(pixel) - mean) * (qRed(pixel) - mean);
                stddev += (qGreen(pixel) - mean) * (qGreen(pixel) - mean);
                stddev += (qBlue(pixel) - mean) * (qBlue(pixel) - mean);
            }
        }
    }

    return std::sqrt(stddev / (img.width() * img.height() * 3));
}

void BaseMetric::computeStatistics()
{
    MetricParam *param;

    m_minError = 255., m_maxError = 0.;
    m_meanError = findMinMax(m_imageDiff, &m_minError, &m_maxError);

    const double stdDev = findStdDev(m_imageDiff, m_meanError);

    // mean error
    param = getOutputParam("MeanError");
    param->value = m_meanError;

    // min error
    param = getOutputParam("MinError");
    param->value = (int)m_minError;

    // max error
    param = getOutputParam("MaxError");
    param->value = (int)m_maxError;

    param = getOutputParam("StandardDeviation");
    param->value = stdDev;

    // Root Mean Square as https://en.wikipedia.org/wiki/Root_mean_square#Relationship_to_the_arithmetic_mean_and_the_standard_deviation
    float rms = sqrtf(m_meanError * m_meanError + stdDev * stdDev);
    param = getOutputParam("RMS");
    param->value = rms;

    // nb error
    m_nbPixelError = countNonBlack(m_imageDiff);
    param = getOutputParam("ErrorNum");
    param->value = m_nbPixelError;

    // % error
    param = getOutputParam("ErrorPercent");
    param->value = m_nbPixelError * 100.0 / (m_imageDiff.width() * m_imageDiff.height());
}
