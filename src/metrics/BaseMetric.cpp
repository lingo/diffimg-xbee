
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

#include <opencv2/imgproc/imgproc.hpp>

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
static bool equalize(QImage &img)
{
    if (img.isNull()) {
        return(false);
    }


    if(img.depth() < 32){
        img = img.convertToFormat(img.hasAlphaChannel() ?
                                  QImage::Format_ARGB32 :
                                  QImage::Format_RGB32);
    }

    const int pixelCount = img.width()*img.height();

    // form histogram
    IntegerPixel histogram [256];
    QRgb *dest = (QRgb *)img.bits();

    if(img.format() == QImage::Format_ARGB32_Premultiplied){
        for(int i=0; i < pixelCount; ++i, ++dest){
            const QRgb pixel = qUnpremultiply(*dest);
            histogram[qRed(pixel)].red++;
            histogram[qGreen(pixel)].green++;
            histogram[qBlue(pixel)].blue++;
            histogram[qAlpha(pixel)].alpha++;
        }
    } else {
        for(int i=0; i < pixelCount; ++i){
            const QRgb pixel = *dest++;
            histogram[qRed(pixel)].red++;
            histogram[qGreen(pixel)].green++;
            histogram[qBlue(pixel)].blue++;
            histogram[qAlpha(pixel)].alpha++;
        }
    }

    // integrate the histogram to get the equalization map
    IntegerPixel map[256];
    IntegerPixel intensity;
    for (int i=0; i < 256; ++i){
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

    // Yes, a normal pixel can be used instead but this is easier to read
    // and no shifts to get components.
    struct CharPixel {
        quint8 red{}, green{}, blue{}, alpha{};
    };

    CharPixel equalize_map[256];

    for (int i=0; i < 256; ++i) {
        if(deltaRed) {
            equalize_map[i].red = uint8_t((255*(map[i].red - low.red)) / deltaRed);
        }

        if(deltaGreen) {
            equalize_map[i].green = uint8_t((255*(map[i].green - low.green)) / deltaGreen);
        }

        if(deltaBlue) {
            equalize_map[i].blue = uint8_t((255*(map[i].blue - low.blue)) / deltaBlue);
        }
    }

    // stretch the histogram and write
    dest = (QRgb *)img.bits();
    uint8_t r, g, b;
    if(img.format() == QImage::Format_ARGB32_Premultiplied){
        for(int i=0; i < pixelCount; ++i, ++dest) {
            const QRgb pixel = qUnpremultiply(*dest);
            r = (deltaRed) ? equalize_map[qRed(pixel)].red : qRed(pixel);
            g = (deltaGreen) ? equalize_map[qGreen(pixel)].green : qGreen(pixel);
            b = (deltaBlue) ?  equalize_map[qBlue(pixel)].blue : qBlue(pixel);
            *dest = qPremultiply(qRgba(r, g, b, qAlpha(pixel)));
        }
    } else {
        for(int i=0; i < pixelCount; ++i){
            const QRgb pixel = *dest;
            r = (deltaRed) ? equalize_map[qRed(pixel)].red : qRed(pixel);
            g = (deltaGreen) ? equalize_map[qGreen(pixel)].green : qGreen(pixel);
            b = (deltaBlue) ?  equalize_map[qBlue(pixel)].blue : qBlue(pixel);
            *dest++ = qRgba(r, g, b, qAlpha(pixel));
        }
    }

    return(true);
}


bool MetricParam::isValid()
{
    if ( value.toFloat() > threshold.toFloat() )
        return false;

    return true;
}

void MetricParam::reset(bool all)
{
    value = defaultValue;
    if (all)
        threshold = defaultValue;
}

void MetricParam::setThreshold(const QString &valText)
{
    switch ( uint(threshold.type()) )
    {
        case QVariant::Int:
            threshold = valText.toInt();
            break;
        case QVariant::Double:
        case QMetaType::Float:
        {
            threshold = valText.toFloat();
            break;
        }
        default:
            break;
    }
}

static void applyGainOffset(QImage &img, double gain, double offset)
{
    if(img.depth() < 32){
        img = img.convertToFormat(img.hasAlphaChannel() ?
                                  QImage::Format_ARGB32 :
                                  QImage::Format_RGB32);
    }

    QRgb *dest = (QRgb *)img.bits();
    const int pixelCount = img.width()*img.height();

    if(img.format() == QImage::Format_ARGB32_Premultiplied){
        for(int i=0; i < pixelCount; ++i, ++dest) {
            const QRgb pixel = qUnpremultiply(*dest);
            const uint8_t r = qBound(0, int(qRed(pixel)   * gain + offset), 255);
            const uint8_t g = qBound(0, int(qGreen(pixel) * gain + offset), 255);
            const uint8_t b = qBound(0, int(qBlue(pixel)  * gain + offset), 255);
            *dest = qPremultiply(qRgba(r, g, b, qAlpha(pixel)));
        }
    } else {
        for(int i=0; i < pixelCount; ++i){
            const QRgb pixel = *dest;
            const uint8_t r = qBound(0, int(qRed(pixel)   * gain + offset), 255);
            const uint8_t g = qBound(0, int(qGreen(pixel) * gain + offset), 255);
            const uint8_t b = qBound(0, int(qBlue(pixel)  * gain + offset), 255);
            *dest++ = qRgba(r, g, b, qAlpha(pixel));
        }
    }
}
static void findMinMax(const QImage &inp, double *min, double *max)
{
    QImage img(inp);

    if(img.depth() < 32){
        img = img.convertToFormat(img.hasAlphaChannel() ?
                                  QImage::Format_ARGB32 :
                                  QImage::Format_RGB32);
    }

    *min = 255;
    *max = 0;

    QRgb *dest = (QRgb *)img.bits();
    const int pixelCount = img.width()*img.height();
    if(img.format() == QImage::Format_ARGB32_Premultiplied){
        for(int i=0; i < pixelCount; ++i, ++dest) {
            const QRgb pixel = qUnpremultiply(*dest);
            *min = std::min(double(qRed(pixel)), *min);
            *min = std::min(double(qGreen(pixel)), *min);
            *min = std::min(double(qBlue(pixel)), *min);

            *max = std::max(double(qRed(pixel)), *max);
            *max = std::max(double(qGreen(pixel)), *max);
            *max = std::max(double(qBlue(pixel)), *max);
        }
    } else {
        for(int i=0; i < pixelCount; ++i){
            const QRgb pixel = *dest;
            *min = std::min(int(qRed(pixel)),   int(*min));
            *min = std::min(int(qGreen(pixel)), int(*min));
            *min = std::min(int(qBlue(pixel)),  int(*min));

            *max = std::max(int(qRed(pixel)),   int(*max));
            *max = std::max(int(qGreen(pixel)), int(*max));
            *max = std::max(int(qBlue(pixel)),  int(*max));
        }
    }
}


//---------------------------------------------------------------------------

BaseMetric::BaseMetric(QObject *parent) :
    QObject(parent),
    m_type("Unknown"),
    m_name( tr("Unknown") ),
    m_desc( tr("No description") ),
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
    if (m_init)
        return;

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
    foreach (MetricParam * param, m_outputParams)
    {
        param->reset(false);
    }
}

void BaseMetric::createInputParams()
{
}

void BaseMetric::createOutputParams()
{
    // mean error
    addOutputParam( new MetricParam("MeanError",tr("Mean error"),tr("Mean absolute error"),BaseMetric::defaultThresholdMeanError) );

    // min error
    addOutputParam( new MetricParam("MinError",tr("Min error"),tr("Minimum error"),0) );

    // max error
    addOutputParam( new MetricParam("MaxError",tr("Max error"),tr("Maximum error"),BaseMetric::defaultThresholdMaxError) );

    addOutputParam( new MetricParam("StandardDeviation",tr("Standard deviation"),tr("Standard deviation"),BaseMetric::defaultThresholdStandardDeviation) );

    // Root Mean Square
    addOutputParam( new MetricParam("RMS",tr("RMS error deviation"),tr("Root Mean Square error"),BaseMetric::defaultThresholdRMSError) );

    // nb error
    addOutputParam( new MetricParam("ErrorNum",tr("Error num (pixels)"),tr("Number of different pixels"),BaseMetric::defaultThresholdNumPixel) );

    // % error
    addOutputParam( new MetricParam("ErrorPercent",tr("Error (% pixels)"),tr("Number of different pixels in %"),0.0f) );
}

MetricParam * BaseMetric::getInputParam(const QString &name)
{
    foreach (MetricParam * param, m_inputParams)
    {
        if (param->type == name)
            return param;
    }
    return NULL;
}

MetricParam * BaseMetric::getOutputParam(const QString &name)
{
    foreach (MetricParam * param, m_outputParams)
    {
        if (param->type == name)
            return param;
    }
    return NULL;
}

QPixmap BaseMetric::getLogo() const
{
    return QPixmap("::/providers/providers/unknown.png");
}

const QString & BaseMetric::getType() const
{
    return m_type;
}

const QString & BaseMetric::getName() const
{
    return m_name;
}

const QString & BaseMetric::getDesc() const
{
    return m_desc;
}

const QList<ImageProperty> & BaseMetric::getProperties() const
{
    return m_properties;
}

const QList<MetricParam *> & BaseMetric::getInputParams()
{
    init();
    return m_inputParams;
}

const QList<MetricParam *> & BaseMetric::getOutputParams()
{
    init();
    return m_outputParams;
}

void BaseMetric::computeStandardProperties()
{
    // Dimension (pixels)
    m_properties << ImageProperty( tr("Dimension (pixels)"), tr("Dimension in pixel of images"),QString("%1x%2").arg(m_image1.width()).arg(m_image1.height()) );

    // size (pixels)
    m_properties << ImageProperty( tr("Size (pixels)"), tr("Size in pixel of images"),MiscFunctions::pixelsToString(m_image1.width() * m_image1.height()) );

    // size (bytes)
    int sz1 = MiscFunctions::getFileSize(m_file1);
    m_properties << ImageProperty( tr("Size file1"), tr("Size in bytes of image"),tr("%1/%2").arg(sz1).arg( MiscFunctions::bytesToString(sz1) ) );
    int sz2 = MiscFunctions::getFileSize(m_file2);
    m_properties << ImageProperty( tr("Size file2"), tr("Size in bytes of image"),tr("%1/%2").arg(sz2).arg( MiscFunctions::bytesToString(sz2) ) );

    // image format
    QMap<QString, QString> longFormats = MiscFunctions::getLongImageFormats();
    QString ext1 = QFileInfo(m_file1).suffix();
    QString fm1 = tr("Unknown");
    if ( longFormats.contains(ext1) )
        fm1 = QString("%1 (%2)").arg(longFormats[ext1]).arg(ext1);
    m_properties << ImageProperty( tr("Format file1"), tr("Format of the image"),QString("%1").arg(fm1) );

    QString ext2 = QFileInfo(m_file1).suffix();
    QString fm2 = tr("Unknown");
    if ( longFormats.contains(ext2) )
        fm2 = QString("%1 (%2)").arg(longFormats[ext2]).arg(ext2);
    m_properties << ImageProperty( tr("Format file2"), tr("Format of the image"),QString("%1").arg(fm2) );

    // bands
    m_properties << ImageProperty( tr("Band"), tr("Number of band in the image (3 for RGB image)"),QString::number( m_opencvInput1.channels() ) );

    // depth
    m_properties << ImageProperty( tr("Band depth"), tr("Number of bits per band (U:unsigned, S:signed, F:float)"),MiscFunctions::matDepthToText( m_opencvInput1.depth() ) );

    findMinMax(m_image1, &m_minImage1, &m_maxImage1);
    qDebug() << m_minImage1 << m_maxImage1;
    findMinMax(m_image2, &m_minImage2, &m_maxImage2);
    cv::minMaxLoc(m_opencvInput1, &m_minImage1, &m_maxImage1); //Locate max and min values
    qDebug() << m_minImage1 << m_maxImage1;
    cv::minMaxLoc(m_opencvInput2, &m_minImage2, &m_maxImage2); //Locate max and min values
}

void BaseMetric::loadSettings()
{
    AppSettings settings;

    settings.beginGroup("Metrics");

    settings.beginGroup(m_type);

    settings.beginGroup("Input");

    foreach (MetricParam * param, m_inputParams)
    {
        QVariant t = settings.value(param->type,param->defaultValue);
        t.convert( param->defaultValue.type() );
        param->threshold = t;
    }

    settings.endGroup();

    settings.beginGroup("Output");

    foreach (MetricParam * param, m_outputParams)
    {
        QVariant t = settings.value(param->type,param->defaultValue);
        t.convert( param->defaultValue.type() );
        param->threshold = t;
    }

    QString typeUsed = settings.value("paramUsed","").toString();
    if ( !typeUsed.isEmpty() )
        setDiscriminatingParam( getOutputParam(typeUsed) );

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

    foreach (MetricParam * param, m_inputParams)
    {
        settings.setValue(param->type,param->threshold);
        qDebug() << m_type << " " << param->type << " = " << param->threshold;
    }

    settings.endGroup();

    settings.beginGroup("Output");

    foreach (MetricParam * param, m_outputParams)
    {
        settings.setValue(param->type,param->threshold);
        if (param->used)
            settings.setValue("paramUsed",param->type);
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

const QImage & BaseMetric::getImage1() const
{
    return m_image1;
}

const QImage & BaseMetric::getImage2() const
{
    return m_image2;
}

const QImage & BaseMetric::getImageDifference() const
{
    return m_imageDiff;
}

bool BaseMetric::saveDifference(const QString &file) const
{
    return m_imageDiff.save(file);
}

const QImage & BaseMetric::getImageMask() const
{
    return m_imageMask;
}

int BaseMetric::getDifferenceChannels() const
{
    return m_opencvDiff.channels();
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
    return m_opencvInput1.channels();
}

int BaseMetric::getImage2Channels() const
{
    return m_opencvInput2.channels();
}

void BaseMetric::setDiscriminatingParam(MetricParam *p)
{
    foreach (MetricParam * param, m_outputParams)
    {
        if (param == p)
            param->used = true;
        else
            param->used = false;
    }
}

QString BaseMetric::getImage1Data(int x,int y) const
{
    return MiscFunctions::dataToText(m_opencvInput1,x,y);
}

QString BaseMetric::getImage2Data(int x,int y) const
{
    return MiscFunctions::dataToText(m_opencvInput2,x,y);
}

QString BaseMetric::getErrorData(int x,int y) const
{
    return MiscFunctions::dataToText(m_opencvDiff,x,y);
}

QString BaseMetric::getImage1Data(const QPoint &pt) const
{
    return getImage1Data( pt.x(),pt.y() );
}

QString BaseMetric::getImage2Data(const QPoint &pt) const
{
    return getImage2Data( pt.x(),pt.y() );
}

QString BaseMetric::getErrorData(const QPoint &pt) const
{
    return getErrorData( pt.x(),pt.y() );
}

MetricParam *BaseMetric::getDiscriminatingParam() const
{
    foreach (MetricParam * param, m_outputParams)
    if (param->used == true)
        return param;

    return NULL;
}

bool BaseMetric::checkImages()
{
    m_valid = false;

    // check loading of files
    if ( m_opencvInput1.empty() )
    {
        LogHandler::getInstance()->reportError( QString("Can't read %1").arg( m_file1 ) );
        return m_valid;
    }
    if ( m_opencvInput2.empty() )
    {
        LogHandler::getInstance()->reportError( QString("Can't read %1").arg( m_file2 ) );
        return false;
    }

    // check size
    if (m_opencvInput1.rows != m_opencvInput2.rows || m_opencvInput1.cols != m_opencvInput2.cols)
    {
        LogHandler::getInstance()->reportError( QString("Size mismatch (%1x%2)/(%3/%4)").arg(m_opencvInput1.rows ).arg(m_opencvInput1.cols).arg(m_opencvInput2.rows).arg(m_opencvInput2.cols) );
        return false;
    }

    // check type
    if ( m_opencvInput1.type() != m_opencvInput2.type() )
    {
        LogHandler::getInstance()->reportError( QString("Type mismatch (%1)/(%2)").arg( MiscFunctions::matTypeToText( m_opencvInput1.type() ) ).arg( MiscFunctions::matTypeToText( m_opencvInput2.type() ) ) );
        return m_valid;
    }

    m_valid = true;
    return m_valid;
}

bool BaseMetric::selectedStatsIsValid() const
{
    foreach (MetricParam * param, m_outputParams)
    {
        if ( param->used && !param->isValid() )
            return false;
    }
    return true;
}

const QList<QPolygonF> &BaseMetric::getHistogramImage1()
{
    if ( m_histoImage1.isEmpty() )
        computeHisto(m_image1,m_histoImage1);
    return m_histoImage1;
}

const QList<QPolygonF> &BaseMetric::getHistogramImage2()
{
    if ( m_histoImage2.isEmpty() )
        computeHisto(m_image2,m_histoImage2);
    return m_histoImage2;
}

const QList<QPolygonF> &BaseMetric::getHistogramImageDiff(bool showZero)
{
    if ( m_histoImageDiff.isEmpty() || m_prevShowHistoZero != showZero )
    {
        m_histoImageDiff.clear();
        computeHisto(m_imageDiff,m_histoImageDiff,!showZero);
    }
    m_prevShowHistoZero = showZero;
    return m_histoImageDiff;
}

void BaseMetric::computeHisto(const QImage &input,QList<QPolygonF> &polys, bool skipZeroLevel)
{
    QImage img(input); // CoW, so don't care

    if (img.format() != QImage::Format_ARGB32) {
        img = img.convertToFormat(QImage::Format_ARGB32);
    }

    polys.clear();

    if ( img.isNull() ) {
        return;
    }

    typedef struct
    {
        quint32 red, green, blue, alpha;
    } HistogramListItem;

    // form histogram
    IntegerPixel histogram [256];
    QRgb *dest = (QRgb *)img.constBits();

    const int pixelCount = img.width()*img.height();

    int max = 0, min = 0;;
    for(int i=0; i < pixelCount; ++i){
        const QRgb pixel = *dest++;
        histogram[qRed(pixel)].red++;
        histogram[qGreen(pixel)].green++;
        histogram[qBlue(pixel)].blue++;
        histogram[qAlpha(pixel)].alpha++;

        //max = std::max(qRed(pixel), max);.
        //max = std::max(qGreen(pixel), max);
        //max = std::max(qBlue(pixel), max);
        //max = std::max(qAlpha(pixel), max);

        //min = std::min(qRed(pixel), min);.
        //min = std::min(qGreen(pixel), min);
        //min = std::min(qBlue(pixel), min);
        //min = std::min(qAlpha(pixel), min);
    }

    QPolygonF points[3];
    for (int i= 0; i<256; i++) {
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

void BaseMetric::checkDifferences(const QString &file1,const QString &file2)
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

    m_opencvInput1.release();

    //m_opencvTransf1.release();
    m_opencvInput2.release();

    //m_opencvTransf2.release();
    m_opencvDiff.release();
    m_opencvMaskDiff.release();

    m_properties.clear();
    init();
    resetOutputParams();

    QImage image1(m_file1);
    if (image1.size().isEmpty()) {
        LogHandler::getInstance()->reportDebug( QString("Can't load %1").arg( m_file1 ) );
        return;
    }

    QImage image2(m_file2);
    if (image2.size().isEmpty()) {
        LogHandler::getInstance()->reportDebug( QString("Can't load %1").arg( m_file2 ) );
        return;
    }

    if (image1.size() != image2.size()) {
        int maxWidth = qMax(image1.width(), image2.width());
        int maxHeight = qMax(image1.height(), image2.height());
        if (image1.width() < image2.width() && image1.height() < image2.height()) {
            image1 = image1.scaled(maxWidth, maxHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        } else if (image2.width() < image1.width() && image2.height() < image1.height()) {
            image2 = image2.scaled(maxWidth, maxHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        } else {
            image1 = image1.convertToFormat(QImage::Format_ARGB32).copy(0, 0, maxWidth, maxHeight);
            image2 = image2.convertToFormat(QImage::Format_ARGB32).copy(0, 0, maxWidth, maxHeight);
        }
    }

    m_opencvInput1 = MiscFunctions::qImageToOpencvMat(image1);
    if ( m_opencvInput1.empty() )
    {
        LogHandler::getInstance()->reportDebug( QString("OpenCV can't read file (%1)").arg( m_file1 ) );
        return;
    }

    m_opencvInput2 = MiscFunctions::qImageToOpencvMat(image2);
    // load file2
    //m_opencvInput2 = cv::imread(m_file2.toStdString(),CV_LOAD_IMAGE_UNCHANGED );
    if ( m_opencvInput2.empty() )
    {
        LogHandler::getInstance()->reportDebug( QString("OpenCV can't read file (%1)").arg( m_file2 ) );
        return;
    }

    // check compatibility
    if( !checkImages() ) {
        return;
    }
    m_image1 = MiscFunctions::opencvMatToQImage(m_opencvInput1,false);
    m_image2 = MiscFunctions::opencvMatToQImage(m_opencvInput2,false);

    // compute "standard" properties
    computeStandardProperties();

    // perform the difference algorithm
    performDifference();

    // compute some statistics on difference image
    computeStatistics();

    /*
       cvtColor(m_opencvInput1, m_opencvTransf1,CV_BGR2RGB);
       m_image1 = QImage((uchar*)m_opencvTransf1.data, m_opencvTransf1.cols, m_opencvTransf1.rows,m_opencvTransf1.step,QImage::Format_RGB888);
       cvtColor(m_opencvInput2, m_opencvTransf2,CV_BGR2RGB);
       m_image2 = QImage((uchar*)m_opencvTransf2.data, m_opencvTransf2.cols, m_opencvTransf2.rows,m_opencvTransf2.step,QImage::Format_RGB888);
     */

    m_image1 = MiscFunctions::opencvMatToQImage(m_opencvInput1,false);
    m_image2 = MiscFunctions::opencvMatToQImage(m_opencvInput2,false);
    m_imageDiff = MiscFunctions::opencvMatToQImage(m_opencvDiff,false).convertToFormat(QImage::Format_ARGB32_Premultiplied);
    equalize(m_imageDiff);

    computeDifferenceMask();

    m_imageMask = MiscFunctions::opencvMatToQImage(m_opencvMaskDiff,false);
}

void BaseMetric::computeStatistics()
{
    MetricParam *param;

    //double maxval, minval;
    minMaxLoc(m_opencvDiff, &m_minError, &m_maxError);

    cv::Scalar templMean, templSdv;

    cv::Mat reshaped = m_opencvDiff.reshape(1);
    meanStdDev( reshaped, templMean, templSdv );

    //////////////////////////////////

/*
    unsigned char * pixelPtr = (unsigned char*)m_opencvDiff.data;
    int cn = m_opencvDiff.channels();
    float err = 0, err2 = 0;
    int nb = 0;

    for(int i=0;i<m_opencvDiff.rows;i++)
    {
        for(int j=0;j<m_opencvDiff.cols;j++)
        {
            for(int k=0;k<cn;k++)
            {
                err += pixelPtr[i*(m_opencvDiff.step)+j*cn+k];
                err2 += (pixelPtr[i*(m_opencvDiff.step)+j*cn+k]*pixelPtr[i*(m_opencvDiff.step)+j*cn+k]);
            }
            nb++;
        }
    }
    float errm = err/(3*nb);
    err2 /=(3*nb);
    err2 = err2 - (errm*errm);
    err2 =  std::sqrt(err2);
 */

    //////////////////////////////////

    double stdDev = 0;
    for (int i = 0; i<reshaped.channels(); i++)
    {
        m_meanError += templMean[i];
        stdDev += templSdv[i];
    }
    m_meanError *= m_opencvDiff.channels(); // mean of 3 components if rgb
    stdDev /= reshaped.channels();

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
    m_nbPixelError = computeNbErrors(m_opencvDiff);
    param = getOutputParam("ErrorNum");
    param->value = m_nbPixelError;

    // % error
    param = getOutputParam("ErrorPercent");
    param->value = m_nbPixelError * 100.0 / (m_opencvDiff.cols * m_opencvDiff.rows);
}

int BaseMetric::computeNbErrors(const cv::Mat &mat)
{
    cv::Mat thresh;
    if ( mat.empty() )
        return 0;

    // caution must do threshold before rgbtogray !!
    if (mat.channels()>1)
        threshold(mat,thresh,0,255,cv::THRESH_BINARY);
    else
        thresh = mat;

    // convert multichannel to a single plane
    // CAUTION !! 2 channels isn't take into account (waiting some data to test this case !!)
    if (thresh.channels()>=3)
        cvtColor(thresh,thresh,CV_RGB2GRAY);
    return countNonZero(thresh);
}

void BaseMetric::computeDifferenceMask()
{
// methode bestiale pour l'instant

    m_opencvMaskDiff = cv::Mat(m_opencvDiff.size(), CV_8UC4);

    // unsigned char * pixelPtr = (unsigned char*)m_opencvDiff.data;
    int cn = m_opencvDiff.channels();

//     qDebug() << "type = " << MiscFunctions::matTypeToText( m_opencvDiff.type() );
//     qDebug() << "depth = " << MiscFunctions::matDepthToText( m_opencvDiff.depth() );

    m_opencvMaskDiff.setTo( cv::Scalar(0,0,0,255) );

    for(int i = 0; i<m_opencvDiff.rows; i++)
        for(int j = 0; j<m_opencvDiff.cols; j++)
        {
            //for(int k=0;k<cn;k++)
            {
                float val = 0;
                if (cn == 1)
                {
                    if (m_opencvDiff.depth() == CV_8U)
                        val = m_opencvDiff.at<uchar>(i,j);
                    else if (m_opencvDiff.depth() == CV_16S)
                        val = m_opencvDiff.at<short>(i,j);
                    else if (m_opencvDiff.depth() == CV_16U)
                        val = m_opencvDiff.at<ushort>(i,j);
                    else if (m_opencvDiff.depth() == CV_32F)
                        val = m_opencvDiff.at<float>(i,j);
                }
                else if (cn == 3)
                {
//                     for(int k = 0; k<cn; k++)
//                         val += m_opencvDiff.at<cv::Vec3b>(i,j)[k];
                    for(int k = 0; k<cn; k++)
                    {
                        if (m_opencvDiff.depth() == CV_8U)
                            val += m_opencvDiff.at<cv::Vec3b>(i,j)[k];
                        else if (m_opencvDiff.depth() == CV_16S)
                            val += m_opencvDiff.at<cv::Vec3s>(i,j)[k];
                        else if (m_opencvDiff.depth() == CV_16U)
                            val += m_opencvDiff.at<cv::Vec3w>(i,j)[k];
                        else if (m_opencvDiff.depth() == CV_32F)
                            val += m_opencvDiff.at<cv::Vec3f>(i,j)[k];
                    }

                    val /= cn;
                }
                else // RGBA => not taking account of Alpha channel !!
                {
//                     for(int k = 0; k<cn - 1; k++)
//                         val += m_opencvDiff.at<cv::Vec4b>(i,j)[k];
                    for(int k = 0; k<cn; k++)
                    {
                        if (m_opencvDiff.depth() == CV_8U)
                            val += m_opencvDiff.at<cv::Vec4b>(i,j)[k];
                        else if (m_opencvDiff.depth() == CV_16U)
                            val += m_opencvDiff.at<cv::Vec4w>(i,j)[k];
                        else if (m_opencvDiff.depth() == CV_16S)
                            val += m_opencvDiff.at<cv::Vec4s>(i,j)[k];
                        else if (m_opencvDiff.depth() == CV_32F)
                            val += m_opencvDiff.at<cv::Vec4f>(i,j)[k];
                    }

                    val /= cn - 1;
                }

                if (val > 0.0)
                {
                    if (val > m_meanError)
                    {
                        m_opencvMaskDiff.at<cv::Vec4b>(i,j)[0] = 255;   // first channel  (B)
                        m_opencvMaskDiff.at<cv::Vec4b>(i,j)[1] = 255; // second channel (G)
                        m_opencvMaskDiff.at<cv::Vec4b>(i,j)[2] = 0;  // 3 channel (R)
                        m_opencvMaskDiff.at<cv::Vec4b>(i,j)[3] = 255;  // 4 channel (A)
                    }
                    else
                    {
                        m_opencvMaskDiff.at<cv::Vec4b>(i,j)[0] = 255;   // first channel  (B)
                        m_opencvMaskDiff.at<cv::Vec4b>(i,j)[1] = 0; // second channel (G)
                        m_opencvMaskDiff.at<cv::Vec4b>(i,j)[2] = 0;  // 3 channel (R)
                        m_opencvMaskDiff.at<cv::Vec4b>(i,j)[3] = 255;  // 4 channel (A)
                    }
                }
                else
                {
                    m_opencvMaskDiff.at<cv::Vec4b>(i,j)[0] = 0;   // first channel  (B)
                    m_opencvMaskDiff.at<cv::Vec4b>(i,j)[1] = 0; // second channel (G)
                    m_opencvMaskDiff.at<cv::Vec4b>(i,j)[2] = 0;  // second channel (R)
                    m_opencvMaskDiff.at<cv::Vec4b>(i,j)[3] = 0;  // second channel (A)
                }
            }
        }

/*
    return;
    cv::Mat upperMean,lowerMean;

    cv::Scalar mins,maxs;

    for (int i = 0; i < m_opencvDiff.channels(); i++)
    {
        mins[i] = m_meanError+1;
        maxs[i] = m_maxError;
    }
    inRange(m_opencvDiff, mins,maxs, upperMean);
    //threshold(upperMean,upperMean,0,255,cv::THRESH_BINARY);
    cv::imwrite("d:/mask.png",upperMean);

    for (int i = 0; i < m_opencvDiff.channels(); i++)
    {
        mins[i] = 1;
        maxs[i] = m_meanError+1;
    }
    inRange(m_opencvDiff, mins,maxs, lowerMean);
    cv::imwrite("d:/mask2.png",lowerMean);
 */
}
