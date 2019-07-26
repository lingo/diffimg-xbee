
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

#ifndef _BASEMETRIC_H_
#define _BASEMETRIC_H_

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QPixmap>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>
#include <opencv2/imgproc/types_c.h>

struct ImageProperty
{
    ImageProperty(const QString & n, const QString &d, const QString &v) :
        name(n),
        desc(d),
        value(v)
    {
    }

    QString name;
    QString desc;
    QString value;
};

class MetricParam
{
public:

    MetricParam(const QString &t, const QString &n, const QString &d, const QVariant &v) :
        type(t),
        name(n),
        desc(d),
        threshold(v),
        value(v),
        defaultValue(v),
        used(false)
    {
    }

    virtual ~MetricParam(){}

    bool isValid();
    void reset(bool all = true);
    void setThreshold(const QString &valText);

    QString type;
    QString name;
    QString desc;
    QVariant threshold;
    QVariant value;
    QVariant defaultValue;
    bool used;
};

class BaseMetric : public QObject
{
    Q_OBJECT

public:

    explicit BaseMetric(QObject *parent = 0);
    virtual ~BaseMetric();

    virtual QPixmap getLogo() const;

    const QString & getType() const;
    const QString & getName() const;
    const QString & getDesc() const;

    const QList<ImageProperty> & getProperties() const;
    const QList<MetricParam *> & getOutputParams();
    const QList<MetricParam *> & getInputParams();

    virtual void checkDifferences(const QString &file1,const QString &file2);

    static int defaultThresholdNumPixel;
    static float defaultThresholdMeanError;
    static float defaultThresholdRMSError;
    static int defaultThresholdMinError;
    static int defaultThresholdMaxError;
    static float defaultThresholdStandardDeviation;

    QImage getImage1WithGain(double gain, double offset);
    QImage getImage2WithGain(double gain, double offset);
    QImage getImageDifferenceWithGain(double gain, double offset);
    const QImage & getImage1() const;
    const QImage & getImage2() const;
    const QImage & getImageDifference() const;
    const QImage & getImageMask() const;

    const QList<QPolygonF> &getHistogramImage1();
    const QList<QPolygonF> &getHistogramImage2();
    const QList<QPolygonF> &getHistogramImageDiff(bool showZero);

    void setDiscriminatingParam(MetricParam *);
    MetricParam *getDiscriminatingParam() const;

    bool isValid() const;

    int getPixelError() const;
    float getMaxError() const;
    float getMinError() const;
    int getDifferenceChannels() const;
    int getImage2Channels() const;
    int getImage1Channels() const;
    bool saveDifference(const QString &) const;

    double getMaxImage1() const;
    double getMaxImage2() const;
    double getMinImage1() const;
    double getMinImage2() const;

    QString getImage1Data(int x,int y) const;
    QString getImage2Data(int x,int y) const;
    QString getErrorData(int x,int y) const;
    QString getImage1Data(const QPoint &) const;
    QString getImage2Data(const QPoint &) const;
    QString getErrorData(const QPoint &) const;

    virtual bool selectedStatsIsValid() const;

    MetricParam * getOutputParam(const QString &name);
    MetricParam * getInputParam(const QString &name);

public slots:

protected slots:

protected:

    void init();
    MetricParam *addOutputParam(MetricParam *);
    MetricParam *addInputParam(MetricParam *);

    void computeStandardProperties();
    void clearOutputParams();
    void resetOutputParams();
    virtual void createOutputParams();
    virtual void createInputParams();
    void saveSettings();
    void loadSettings();
    bool checkImages();

    virtual void computeStatistics();
    virtual void performDifference() = 0;

    int computeNbErrors(const cv::Mat &mat);
    void computeHisto(const QImage &input,QList<QPolygonF> &polys, bool skipZeroLevel = false);
    void computeDifferenceMask();

    QString m_type;
    QString m_name;
    QString m_desc;

    // properties = size, type, ...
    QList<ImageProperty> m_properties;

    // input param if needed
    QList<MetricParam *> m_inputParams;

    // params RMS, ...
    QList<MetricParam *> m_outputParams;

    QString m_file1;
    QString m_file2;

    cv::Mat m_opencvInput1;

    //cv::Mat m_opencvTransf1;
    cv::Mat m_opencvInput2;

    //cv::Mat m_opencvTransf2;
    cv::Mat m_opencvDiff;
    cv::Mat m_opencvMaskDiff;
    QImage m_image1;
    QImage m_image2;
    QImage m_imageDiff;
    QImage m_imageMask;

    bool m_valid;
    int m_nbPixelError;
    double m_maxError;
    double m_minError;
    double m_meanError;

    double m_minImage1;
    double m_minImage2;
    double m_maxImage1;
    double m_maxImage2;

    QList<QPolygonF> m_histoImage1;
    QList<QPolygonF> m_histoImage2;
    QList<QPolygonF> m_histoImageDiff;

    bool m_init;
    bool m_prevShowHistoZero;
};

#endif // _BASEMETRIC_H_
