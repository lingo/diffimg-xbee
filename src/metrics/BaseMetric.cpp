
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
    switch ( threshold.type() )
    {
        case QMetaType::Int:
        {
            threshold = valText.toInt();
            break;
        }
        case QMetaType::Double:
        case QMetaType::Float:
        {
            threshold = valText.toFloat();
            break;
        }
        default:
            break;
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
    m_properties << ImageProperty( tr("Dimension (pixels)"), tr("Dimension in pixel of images"),QString("%1x%2").arg(m_opencvInput1.cols).arg(m_opencvInput1.rows) );

    // size (pixels)
    m_properties << ImageProperty( tr("Size (pixels)"), tr("Size in pixel of images"),MiscFunctions::pixelsToString(m_opencvInput1.cols * m_opencvInput1.rows) );

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

    cv::minMaxLoc(m_opencvInput1, &m_minImage1, &m_maxImage1); //Locate max and min values
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
    cv::Mat converted;
    m_opencvInput1.convertTo(converted, -1, gain, offset);
    return MiscFunctions::opencvMatToQImage(converted);
}

QImage BaseMetric::getImage2WithGain(double gain, double offset)
{
    cv::Mat converted;
    m_opencvInput2.convertTo(converted, -1, gain, offset);
    return MiscFunctions::opencvMatToQImage(converted);
}

QImage BaseMetric::getImageDifferenceWithGain(double gain, double offset)
{
    cv::Mat converted;
    m_opencvDiff.convertTo(converted, -1, gain, offset);
    return MiscFunctions::opencvMatToQImage(converted);
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
    return cv::imwrite( file.toStdString(), m_opencvDiff);
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
        return m_valid;
    }

    // check size
    if (m_opencvInput1.rows != m_opencvInput2.rows || m_opencvInput1.cols != m_opencvInput2.cols)
    {
        LogHandler::getInstance()->reportError( QString("Size mismatch (%1x%2)/(%3/%4)").arg(m_opencvInput1.rows ).arg(m_opencvInput1.cols).arg(m_opencvInput2.rows).arg(m_opencvInput2.cols) );
        return m_valid;
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
        computeHisto(m_opencvInput1,m_histoImage1);
    return m_histoImage1;
}

const QList<QPolygonF> &BaseMetric::getHistogramImage2()
{
    if ( m_histoImage2.isEmpty() )
        computeHisto(m_opencvInput2,m_histoImage2);
    return m_histoImage2;
}

const QList<QPolygonF> &BaseMetric::getHistogramImageDiff(bool showZero)
{
    if ( m_histoImageDiff.isEmpty() || m_prevShowHistoZero != showZero )
    {
        m_histoImageDiff.clear();
        computeHisto(m_opencvDiff,m_histoImageDiff,!showZero);
    }
    m_prevShowHistoZero = showZero;
    return m_histoImageDiff;
}

void BaseMetric::computeHisto(const cv::Mat img,QList<QPolygonF> &polys, bool skipZeroLevel)
{
    polys.clear();

    if ( img.empty() )
        return;

    QList<cv::Mat> listHisto;

    for (int i = 0; i < img.channels(); i++)
        listHisto << cv::Mat();

    std::vector<cv::Mat> listChannels;

    cv::split(img, listChannels);

    // compute global min/max
    double min = 100000;
    double max = -100000;
    for (size_t i = 0; i < listChannels.size(); i++)
    {
        double minVal, maxVal;
        cv::minMaxLoc(listChannels[i], &minVal, &maxVal); //Locate max and min values
        min = qMin(min,minVal);
        max = qMax(max,maxVal);
    }

    // null image => exit
    if (min == 0.0 && max == 0.0)
        return;

    for (size_t i = 0; i < listChannels.size(); i++)
    {
//         double minVal, maxVal;
//         cv::minMaxLoc(listChannels[i], &minVal, &maxVal); //Locate max and min values
//
//         qDebug() << "min " << minVal << " max = " << maxVal;

        // Initialize histogram settings
        int histSize[] = { (int)(max - min) };
        float Range[] = { (float)min, (float)max }; //{0, 256} = 0 to 255
        const float *Ranges[] = { Range };
        int channels[] = { 0 };

        /*
           int histSize[] = { 256 };
           float Range[] = { 0, 256 }; //{0, 256} = 0 to 255
           const float *Ranges[] = { Range };
           int channels[] = { 0 };
         */

        cv::Mat hist;

        calcHist(&(listChannels[i]), 1, channels, cv::Mat(), // do not use mask
                 hist,
                 1,
                 histSize,
                 Ranges,
                 true, // the histogram is uniform
                 false);

        QPolygonF points;
        for( int h = 0; h < hist.rows; h++)
        {
            float bin_value = 0;
            if (!skipZeroLevel || h)
                bin_value = hist.at<float>(h);
            points << QPointF( (float)h, bin_value );
        }
        polys << points;
    }

    /*
       return;

       cv::Mat histB;
       cv::Mat histG;
       cv::Mat histR;

       MiscFunctions::ComputeHistogramBGR(img, histB, histG, histR);

       //computeHistogram1D

       // Insert the points that should be plotted on the graph in a vector of QPoints or a QPolgonF
       QPolygonF points;
       for( int h = 0; h < histB.rows; h++)
       {
        float bin_value = histB.at<float>(h);
        points << QPointF((float)h, bin_value);
       }
       polys << points;

       points.clear();
       for( int h = 0; h < histG.rows; h++)
       {
        float bin_value = histG.at<float>(h);
        points << QPointF((float)h, bin_value);
       }
       polys << points;

       points.clear();
       for( int h = 0; h < histR.rows; h++)
       {
        float bin_value = histR.at<float>(h);
        points << QPointF((float)h, bin_value);
       }
       polys << points;

     */
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

    // load file1
    m_opencvInput1 = cv::imread(m_file1.toStdString(),CV_LOAD_IMAGE_UNCHANGED );
    if ( m_opencvInput1.empty() )
    {
        LogHandler::getInstance()->reportDebug( QString("OpenCV can't read file, trying with Qt ... (%1)").arg( m_file1 ) );

        // try to load with Qt
        m_opencvInput1 = MiscFunctions::searchDecoder( m_file1 );
    }

    // load file2
    m_opencvInput2 = cv::imread(m_file2.toStdString(),CV_LOAD_IMAGE_UNCHANGED );
    if ( m_opencvInput2.empty() )
    {
        LogHandler::getInstance()->reportDebug( QString("OpenCV can't read file, trying with Qt ... (%1)").arg( m_file2 ) );

        // try to load with Qt
        m_opencvInput2 = MiscFunctions::searchDecoder( m_file2 );
    }

    // check compatibility
    if( !checkImages() )
        return;

    // compute "standard" properties
    computeStandardProperties();

    // perform the difference algorithm
    performDifference();

    // compute some statistics on difference image
    computeStatistics();

//      bool ok1 = cv::imwrite("d:/toto1.png",m_opencvInput1);
//      bool ok2 = cv::imwrite("d:/toto2.png",m_opencvInput2);

    /*
       cvtColor(m_opencvInput1, m_opencvTransf1,CV_BGR2RGB);
       m_image1 = QImage((uchar*)m_opencvTransf1.data, m_opencvTransf1.cols, m_opencvTransf1.rows,m_opencvTransf1.step,QImage::Format_RGB888);
       cvtColor(m_opencvInput2, m_opencvTransf2,CV_BGR2RGB);
       m_image2 = QImage((uchar*)m_opencvTransf2.data, m_opencvTransf2.cols, m_opencvTransf2.rows,m_opencvTransf2.step,QImage::Format_RGB888);
     */

    m_image1 = MiscFunctions::opencvMatToQImage(m_opencvInput1,false);
    m_image2 = MiscFunctions::opencvMatToQImage(m_opencvInput2,false);
    m_imageDiff = MiscFunctions::opencvMatToQImage(m_opencvDiff,false).convertToFormat(QImage::Format_ARGB32_Premultiplied);
    //m_imageDiff.fill(Qt::red);
    qDebug() << m_imageDiff;

    // !!!!!!!!!!! debug !!!!!!!!

    /*
       if (m_opencvDiff.type() == CV_8UC3)
       {
        m_opencvDiff.at<cv::Vec3b>(200,300)[0] = 255;
        m_opencvDiff.at<cv::Vec3b>(200,300)[1] = 128;
        m_opencvDiff.at<cv::Vec3b>(200,300)[2] = 50;
       }
     */

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
