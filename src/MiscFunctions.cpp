
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

#include <QtCore/QCoreApplication>
#include <QtCore/QLocale>
#include <QtCore/QCoreApplication>
#include <QtCore/QLibraryInfo>
#include <QtCore/QTranslator>
#include <QtCore/QDir>
#include <QtCore/QDebug>

#include <QtGui/QApplication>
#include <QtGui/QPixmap>
#include <QtGui/QImageReader>
#include <QtGui/QImage>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "MiscFunctions.h"
#include "LogHandler.h"
#include "AppSettings.h"
#include "FormatsManager.h"

#include <math.h>
#include <vector>

#ifndef ABS
#define ABS(x) ( ( (x)>=0 ) ? (x) : ( -(x) ) )
#endif

#ifndef MAX
#define MAX(a,b)  ( ( (a) > (b) ) ? (a) : (b) )
#endif

#ifndef MIN
#define MIN(a,b)  ( ( (a) < (b) ) ? (a) : (b) )
#endif

QStringList MiscFunctions::getAvailablesImageFormatsList()
{
    QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();
    QStringList result;
    QListIterator<QByteArray> supportedFormat(supportedFormats);
    QByteArray format;

    while ( supportedFormat.hasNext() )
    {
        format = supportedFormat.next().toLower();

        if ( !result.contains(format) )
            result << format;
    }

    // add formats not support by Qt but by opencv
    result << "exr"; // OpenEXR
    result << "jp2"; // jpeg2000
    result << "yuv"; // yuv

    return result;
}

/*
 * Extract from Wally - Qt4 wallpaper/background changer
 * Copyright (C) 2009  Antonio Di Monaco <tony@becrux.com>
 */

QMap<QString, QString> MiscFunctions::getLongImageFormats()
{
    QMap<QString, QString> formats;

    /*
       BMP files [reading, writing]
       Dr. Halo CUT files [reading] *
       DDS files [reading]
       EXR files [reading, writing]
       Raw Fax G3 files [reading]
       GIF files [reading, writing]
       HDR files [reading, writing]
       ICO files [reading, writing]
       IFF files [reading]
       JBIG [reading, writing] **
       JNG files [reading]
       JPEG/JIF files [reading, writing]
       JPEG-2000 File Format [reading, writing]
       JPEG-2000 codestream [reading, writing]
       KOALA files [reading]
       Kodak PhotoCD files [reading]
       MNG files [reading]
       PCX files [reading]
       PBM/PGM/PPM files [reading, writing]
       PFM files [reading, writing]
       PNG files [reading, writing]
       Macintosh PICT files [reading]
       Photoshop PSD files [reading]
       RAW camera files [reading]
       Sun RAS files [reading]
       SGI files [reading]
       TARGA files [reading, writing]
       TIFF files [reading, writing]
       WBMP files [reading, writing]
       XBM files [reading]
       XPM files [reading, writing]
     */

    formats["bw"] = "Black & White";
    formats["eps"] = "Encapsulated Postscript";
    formats["epsf"] = "Encapsulated PostScript";
    formats["epsi"] = "Encapsulated PostScript Interchange";
    formats["exr"] = "OpenEXR";
    formats["pcx"] = "PC Paintbrush Exchange";
    formats["psd"] = "Photoshop Document";
    formats["rgb"] = "Raw red, green, and blue samples";
    formats["rgba"] = "Raw red, green, blue, and alpha samples";
    formats["sgi"] = "Irix RGB";
    formats["tga"] = "Truevision Targa";
    formats["xcf"] = "eXperimental Computing Facility (GIMP)";
    formats["dds"] = "DirectDraw Surface";
    formats["xv"] = "Khoros Visualization";
    formats["bmp"] = "Windows Bitmap";
    formats["gif"] = "Graphic Interchange Format";
    formats["jpg"] = "Joint Photographic Experts Group";
    formats["jpeg"] = "Joint Photographic Experts Group";
    formats["jp2"] = "Joint Photographic Experts Group 2000";
    formats["mng"] = "Multiple-image Network Graphics";
    formats["png"] = "Portable Network Graphics";
    formats["pbm"] = "Portable Bitmap";
    formats["pgm"] = "Portable Graymap";
    formats["ppm"] = "Portable Pixmap";
    formats["tiff"] = "Tagged Image File Format";
    formats["tif"] = "Tagged Image File Format";
    formats["xbm"] = "X11 Bitmap";
    formats["xpm"] = "X11 Pixmap";
    formats["ico"] = "Icon Image";
    formats["svg"] = "Scalable Vector Graphics";
    formats["yuv"] = "Raw CCIR 601";

    return formats;
}

QString MiscFunctions::getAvailablesImageFormats()
{
    QString imglist;
    QStringList formats = getAvailablesImageFormatsList();
    for (int i = 0; i < formats.size(); ++i)
        imglist += "*." + formats[i] + " ";

    return imglist;
}

QString MiscFunctions::getTranslationsFile(const QString &lang)
{
    return QString("%1_%2.qm").arg(PACKAGE_NAME).arg(lang).toLower();
}

QString MiscFunctions::getTranslationsPath(const QString &refLang)
{
    // search in application path
    QStringList ldir;
    ldir << QCoreApplication::applicationDirPath () + "/lang";
    ldir << QCoreApplication::applicationDirPath () + "/../lang";
    ldir << QCoreApplication::applicationDirPath () + "/../../lang";
    ldir << QCoreApplication::applicationDirPath () + "/../translations"; // MacOSX
    ldir << QString("/usr/share/%1/locale").arg(PACKAGE_NAME).toLower();
    ldir << QString("/usr/local/share/%1/locale").arg(PACKAGE_NAME).toLower();
    ldir << "/usr/local/share/locale";
    ldir << "/usr/share/local/locale";
    ldir << "/usr/share/locale";


    QString ext;
    if ( !refLang.isEmpty() )
        ext = "/" + getTranslationsFile(refLang);

    foreach (const QString &dir, ldir)
    {
        if ( QFileInfo(dir + ext).exists() )
            return dir;
    }

    return "";
}

QMap<QString, QString> MiscFunctions::getAvailableLanguages()
{
    QMap<QString, QString> languageMap;
    QDir dir( MiscFunctions::getTranslationsPath("fr") );
    QRegExp expr( QString("^%1_(\\w+)\\.qm$").arg(PACKAGE_NAME).toLower() );
    QStringList files = dir.entryList(QDir::Files, QDir::Name);

    LogHandler::getInstance()->reportDebug( QString("translations path %1").arg( dir.path() ) );

    foreach ( const QString &file, files )
    {
        if ( !file.contains(expr) )
            continue;

        QString lang = expr.cap(1);
        QString name = QString("%1 (%2)").arg(QLocale::languageToString( QLocale(lang).language() ), lang);

        if (lang.contains("_")) // detect variant
        {
            QString country = QLocale::countryToString(QLocale(lang).country());
            name = QString("%1 (%2) (%3)").arg(QLocale::languageToString( QLocale(lang).language() ), country, lang);
        }

        languageMap[name] = lang;
    }
    return languageMap;
}

void MiscFunctions::setDefaultLanguage()
{
    QString lang;
    AppSettings settings;

    settings.beginGroup("Application");
    lang = settings.value("currentLanguage","auto").toString();
    settings.endGroup();

    if ( lang.isEmpty() )
        lang = QLocale::system().name().left(2);

    if ( !lang.isEmpty() )
        setLanguage(lang);
}

void MiscFunctions::setLanguage(const QString& lang)
{
    QString language(lang);
    LogHandler::getInstance()->reportDebug( QObject::tr("setting language to : %1").arg(language) );

    // special cases
    if (language == "auto") // auto detection
    {
        language = QLocale::system().name().left(2);
    }
    else if (language == "default") // no use of translator
    {
        return;
    }

    // try load the qt translator for selected language
    QTranslator *qt = new QTranslator();
    QStringList excludedFiles;
    QString globalTranslationPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    if ( !QFileInfo(globalTranslationPath).exists() )
        globalTranslationPath = MiscFunctions::getTranslationsPath("fr");

    if ( qt->load( "qt_" + language, globalTranslationPath ) )
    {
        LogHandler::getInstance()->reportDebug( QObject::tr("successfully loaded data from %1").arg( globalTranslationPath + "/qt_" + language ) );
        QCoreApplication::installTranslator(qt);
        excludedFiles << "qt_" + language + ".qm";
    }
    else
    {
        delete qt;
    }

    QString suff = language + ".qm";
    QDir dir( MiscFunctions::getTranslationsPath("fr") );

    foreach ( const QString &s, dir.entryList(QDir::Files | QDir::Readable) )
    {
        if ( !s.endsWith(suff) || excludedFiles.contains(s) )
            continue;
        QTranslator *t = new QTranslator();

        if ( t->load( dir.filePath(s) ) )
        {
            QCoreApplication::installTranslator(t);
            LogHandler::getInstance()->reportDebug( QObject::tr("successfully loaded data from %1").arg( dir.filePath(s) ) );
        }
        else
        {
            delete t;
            LogHandler::getInstance()->reportDebug( QObject::tr("failed to load data from %1").arg( dir.filePath(s) ) );
        }
    }
}

void MiscFunctions::updateApplicationIdentity()
{
    QCoreApplication::setApplicationName(PACKAGE_NAME);
    QCoreApplication::setApplicationVersion(PACKAGE_VERSION);
    QCoreApplication::setOrganizationName(PACKAGE_ORGANIZATION);
}

bool MiscFunctions::stringToFile(const QString &data, const QString &filename)
{
    QFile file(filename);
    if ( file.open(QFile::WriteOnly | QFile::Truncate) )
    {
        QTextStream out(&file);
        out << data;
        return true;
    }
    return false;
}

QString MiscFunctions::fileToString(const QString &filename)
{
    QFile file(filename);
    QString res;
    if ( file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        QTextStream in(&file);
        res = in.readAll();
    }
    return res;
}

QImage MiscFunctions::opencvMatToQImage(const cv::Mat& mat, bool deepCopy)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1)
    {
        // Set the color table (used to translate color indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i = 0; i<256; i++)
            colorTable.push_back( qRgb(i,i,i) );

        if (deepCopy)
        {
            QImage img(mat.cols, mat.rows, QImage::Format_Indexed8);
            for (int i = 0; i < img.height(); i++)
                // scanLine returns a ptr to the start of the data for that row
                memcpy( img.scanLine(i), mat.ptr(i), img.bytesPerLine() );  //correct
            return img;
        }
        else
        {
            // Copy input Mat
            const uchar *qImageBuffer = (const uchar*)mat.data;

            // Create QImage with same dimensions as input Mat
            QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
            img.setColorTable(colorTable);
            return img;
        }
    }
    else if (mat.type()==CV_16UC1)
    {
        cv::Mat ucharMatScaled;
        cv::Mat ushortMatScaled;
        cv::Mat floatMatScaled;
        double minImage, maxImage;
        cv::minMaxLoc(mat, &minImage, &maxImage);
        mat.convertTo(floatMatScaled, CV_32FC1);

        // to ensure [0-1.0]
        floatMatScaled = (floatMatScaled - minImage) / (maxImage - minImage);
        floatMatScaled.convertTo(ucharMatScaled, CV_8UC1, 255, 0);
        return opencvMatToQImage(ucharMatScaled);
    }
    else if (mat.type()==CV_32FC1)
    {
        cv::Mat ucharMatScaled;
        cv::Mat floatMatScaled;
        double minImage, maxImage;
        cv::minMaxLoc(mat, &minImage, &maxImage);

        // to ensure [0-1.0]
        floatMatScaled = (mat - minImage) / (maxImage - minImage);
        floatMatScaled.convertTo(ucharMatScaled, CV_8UC1, 255, 0);

        return opencvMatToQImage(ucharMatScaled);
    }
    else if (mat.type() == CV_32FC3)
    {
        cv::Mat ucharMatScaled;
        cv::Mat floatMatScaled;
        double minImage, maxImage;
        cv::minMaxLoc(mat, &minImage, &maxImage);

        normalize(mat, floatMatScaled, 0.0, 1.0, cv::NORM_MINMAX);
        cv::pow(floatMatScaled, 1. / 5, floatMatScaled); // apply gamma curve: img = img ** (1./5)
        mat.convertTo(ucharMatScaled, CV_8UC3, 255, 0);

        qDebug() << "type ucharMatScaled = " << MiscFunctions::matTypeToText( ucharMatScaled.type() );

        return opencvMatToQImage(ucharMatScaled);
    }

    // 8-bits unsigned, NO. OF CHANNELS=3
    else
    {
        cv::Mat rgbMat;
        bool swap = true;
        int qImageFormat = QImage::Format_RGB888;
        if(mat.type()==CV_8UC4)
        {
            qImageFormat = QImage::Format_ARGB32;
            swap = false;
            rgbMat = mat;
        }
        else if (mat.type()==CV_8UC3)
        {
            rgbMat = mat;
        }
        else
        {
            cvtColor(mat, rgbMat,CV_BGR2RGB);
        }

        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;

        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step,(QImage::Format) qImageFormat);
        if (swap) {
            return img.rgbSwapped();
        }
        return img; // deep copy !!
        //}
    }
}

cv::Mat MiscFunctions::searchDecoder(const QString &file)
{
    QImage img(file);
    if (img.isNull()) // search other decoder
    {
        bool ok;
        return FormatsManager::decode(file,ok);
    }
    return qImageToOpencvMat(img);
}

cv::Mat MiscFunctions::qImageToOpencvMat(const QImage &image)
{
    if ( image.isNull() )
        return cv::Mat();

    cv::Mat mat = cv::Mat( image.height(), image.width(), CV_8UC4, (uchar*)image.bits(), image.bytesPerLine() );
    cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3 );
    int from_to[] = { 0,0,  1,1,  2,2 };
    cv::mixChannels( &mat, 1, &mat2, 1, from_to, 3 );
    return mat2;
}

QString MiscFunctions::matTypeToText(int type)
{
    switch(  type )
    {
        case CV_8UC1:
            return "CV_8UC1";

            break;
        case CV_8UC2:
            return "CV_8UC2";

            break;
        case CV_8UC3:
            return "CV_8UC3";

            break;
        case CV_8UC4:
            return "CV_8UC4";

            break;
        case CV_8SC1:
            return "CV_8SC1";

            break;
        case CV_8SC2:
            return "CV_8SC2";

            break;
        case CV_8SC3:
            return "CV_8SC3";

            break;
        case CV_8SC4:
            return "CV_8SC4";

            break;
        case CV_16UC1:
            return "CV_16UC1";

            break;
        case CV_16UC2:
            return "CV_16UC2";

            break;
        case CV_16UC3:
            return "CV_16UC3";

            break;
        case CV_16UC4:
            return "CV_16UC4";

            break;
        case CV_16SC1:
            return "CV_16SC1";

            break;
        case CV_16SC2:
            return "CV_16SC2";

            break;
        case CV_16SC3:
            return "CV_16SC3";

            break;
        case CV_16SC4:
            return "CV_16SC4";

            break;
        case CV_32SC1:
            return "CV_32SC1";

            break;
        case CV_32SC2:
            return "CV_32SC2";

            break;
        case CV_32SC3:
            return "CV_32SC3";

            break;
        case CV_32SC4:
            return "CV_32SC4";

            break;
        case CV_32FC1:
            return "CV_32FC1";

            break;
        case CV_32FC2:
            return "CV_32FC2";

            break;
        case CV_32FC3:
            return "CV_32FC3";

            break;
        case CV_32FC4:
            return "CV_32FC4";

            break;
        case CV_64FC1:
            return "CV_64FC1";

            break;
        case CV_64FC2:
            return "CV_64FC2";

            break;
        case CV_64FC3:
            return "CV_64FC3";

            break;
        case CV_64FC4:
            return "CV_64FC4";

            break;
        default:
            break;
    }
    return "";
}

QString MiscFunctions::matDepthToText(int depth)
{
    switch(  depth )
    {
        case CV_8U:
            return "8U";

            break;
        case CV_8S:
            return "8S";

            break;
        case CV_16U:
            return "16U";

            break;
        case CV_16S:
            return "16S";

            break;
        case CV_32S:
            return "32S";

            break;
        case CV_32F:
            return "32F";

            break;
        case CV_64F:
            return "64F";

            break;
        default:
            break;
    }
    return "";
}

quint64 MiscFunctions::getFileSize(const QString &file)
{
    return QFile(file).size();
}

QString MiscFunctions::bytesToString(quint64 bytes)
{
    quint64 size = bytes;

    QString bytesStr;
    if ( !(size >> 10) )
        bytesStr = QString::number(size) + ' ' + QObject::tr("Bytes");
    else if ( !(size >> 20) )
        bytesStr = QString::number(size / 1024.0, 'f', 3) + ' ' + QObject::tr("KB","Kilobyte");
    else if ( !(size >> 30) )
        bytesStr = QString::number( (size >> 10) / 1024.0, 'f', 3 ) + ' ' + QObject::tr("MB","Megabyte");
    else if ( !(size >> 40) )
        bytesStr = QString::number( (size >> 20) / 1024.0, 'f', 3 ) + ' ' + QObject::tr("GB","Gigabyte");
    else
        bytesStr = QString::number( (size >> 30) / 1024.0, 'f', 3 ) + ' ' + QObject::tr("TB","Terabyte");

    return bytesStr;
}

QString MiscFunctions::pixelsToString(quint64 pixels)
{
    quint64 size = pixels;

    QString bytesStr;
    if ( !(size >> 10) )
        bytesStr = QString::number(size) + ' ' + QObject::tr("Pixels");
    else if ( !(size >> 20) )
        bytesStr = QString::number(size / 1000.0, 'f', 3) + ' ' + QObject::tr("Kp","Kilopixel");
    else if ( !(size >> 30) )
        bytesStr = QString::number( (size >> 10) / 1000.0, 'f', 3 ) + ' ' + QObject::tr("Mp","Megapixel");
    else if ( !(size >> 40) )
        bytesStr = QString::number( (size >> 20) / 1000.0, 'f', 3 ) + ' ' + QObject::tr("Gp","Gigapixel");
    else
        bytesStr = QString::number( (size >> 30) / 1000.0, 'f', 3 ) + ' ' + QObject::tr("Tp","Terapixel");

    return bytesStr;
}

//-----------------------------------------------------------------------------
// Computes a three channel (BGR) histogram from Paul Filitchkin histLib
//-----------------------------------------------------------------------------
void MiscFunctions::ComputeHistogramBGR(
    const cv::Mat& Image,
    cv::Mat&       HistB,
    cv::Mat&       HistG,
    cv::Mat&       HistR)
{
    const cv::Mat* pImageBGR = NULL;
    cv::Mat ImageBGR;

    switch ( Image.type() )
    {
        case CV_8UC3:
            pImageBGR = &Image;
            break;

        case CV_8UC4:
        {
            cvtColor(Image, ImageBGR, CV_RGBA2RGB);
            pImageBGR = &ImageBGR;
            break;
        }

        default:
            LogHandler::getInstance()->reportError("MiscFunctions::ComputeHistogramBGR: the data format/type is not supported by the function");
            break;
    }

    // Initialize histogram settings
    int histSize[] = {256};
    float Range[] = {0, 256}; //{0, 256} = 0 to 255
    const float* Ranges[] = {Range};
    int chanB[] = {0};
    int chanG[] = {1};
    int chanR[] = {2};

    calcHist(pImageBGR, 1, chanB, cv::Mat(), // do not use mask
             HistB,
             1,
             histSize,
             Ranges,
             true, // the histogram is uniform
             false);

    calcHist(pImageBGR, 1, chanG, cv::Mat(), // do not use mask
             HistG,
             1,
             histSize,
             Ranges,
             true, // the histogram is uniform
             false);

    calcHist(pImageBGR, 1, chanR, cv::Mat(), // do not use mask
             HistR,
             1,
             histSize,
             Ranges,
             true, // the histogram is uniform
             false);
}

//-----------------------------------------------------------------------------
// Computes a single channel (Value) histogram
//-----------------------------------------------------------------------------
void MiscFunctions::computeHistogramValue(const cv::Mat& Image, cv::Mat& Hist)
{
    // Create 1 channel image to get a value representation
    cv::Mat ImageValue = cv::Mat(Image.size(), CV_8UC1);

    switch ( Image.type() )
    {
        case CV_8UC1:
            Image.copyTo(ImageValue);
            break;

        case CV_8UC3:
        {
            cv::Mat ImageHSV = cv::Mat(Image.size(), CV_8UC3);
            std::vector<cv::Mat> ChannlesHsv;

            cvtColor(Image, ImageHSV, CV_BGR2HSV);
            cv::split(ImageHSV, ChannlesHsv);
            ImageValue = ChannlesHsv[2];
            break;
        }

        case CV_8UC4:
        {
            cv::Mat ImageHSV = cv::Mat(Image.size(), CV_8UC3);
            cv::Mat ImageBGR = cv::Mat(Image.size(), CV_8UC3);
            std::vector<cv::Mat> ChannlesHsv;

            cvtColor(Image, ImageBGR, CV_RGBA2RGB);
            cvtColor(ImageBGR, ImageHSV, CV_RGBA2RGB);
            cv::split(ImageHSV, ChannlesHsv);
            ImageValue = ChannlesHsv[2];
            break;
        }

        default:
            LogHandler::getInstance()->reportError("MiscFunctions::computeHistogramValue: the data format/type is not supported by the function");
            break;
    }

    // Initialize histogram settings
    int histSize[] = { 256 };
    float Range[] = { 0, 256 }; //{0, 256} = 0 to 255
    const float *Ranges[] = { Range };
    int channels[] = { 0 };

    calcHist(&ImageValue, 1, channels, cv::Mat(), // do not use mask
             Hist,
             1,
             histSize,
             Ranges,
             true, // the histogram is uniform
             false);
}

void MiscFunctions::computeHistogram1D(const cv::Mat& img, cv::Mat& hist)
{
    if (img.channels() != 1)
        return;

    // Initialize histogram settings
    int histSize[] = { 256 };
    float Range[] = { 0, 256 }; //{0, 256} = 0 to 255
    const float *Ranges[] = { Range };
    int channels[] = { 0 };

    calcHist(&img, 1, channels, cv::Mat(), // do not use mask
             hist,
             1,
             histSize,
             Ranges,
             true, // the histogram is uniform
             false);
}

QString MiscFunctions::dataToText(const cv::Mat& img, int x, int y)
{
    if ( img.empty() || x < 0 || y < 0 || x > (img.cols - 1) || y > (img.rows - 1) )
        return "";

    QString data;
    switch( img.type() )
    {
        case CV_8UC1:

            //for (int i = 0 ; i < img.channels(); i++)
            data += QObject::tr("Value: %1").arg( img.at<uchar>(y,x) );
            break;
        case CV_8UC2:
            data += QObject::tr("Value: %1,").arg(img.at<cv::Vec2b>(y,x)[0]);
            data += QObject::tr("Alpha: %1").arg(img.at<cv::Vec2b>(y,x)[1]);
            break;
        case CV_8UC3: // BGR !!
            data += QObject::tr("Red: %1,").arg(img.at<cv::Vec3b>(y,x)[2]);
            data += QObject::tr("Green: %1,").arg(img.at<cv::Vec3b>(y,x)[1]);
            data += QObject::tr("Blue: %1").arg(img.at<cv::Vec3b>(y,x)[0]);
            break;
        case CV_8UC4:
            data += QObject::tr("Red: %1,").arg(img.at<cv::Vec4b>(y,x)[2]);
            data += QObject::tr("Green: %1,").arg(img.at<cv::Vec4b>(y,x)[1]);
            data += QObject::tr("Blue: %1,").arg(img.at<cv::Vec4b>(y,x)[0]);
            data += QObject::tr("Alpha: %1").arg(img.at<cv::Vec4b>(y,x)[3]);
            break;
        case CV_8SC1:
            data += QObject::tr("not yet implemented (%1)").arg("CV_8SC1");
            break;
        case CV_8SC2:
            data += QObject::tr("not yet implemented (%1)").arg("CV_8SC2");
            break;
        case CV_8SC3:
            data += QObject::tr("not yet implemented (%1)").arg("CV_8SC3");
            break;
        case CV_8SC4:
            data += QObject::tr("not yet implemented (%1)").arg("CV_8SC4");
            break;
        case CV_16UC1:
            data += QObject::tr("Value: %1").arg( img.at<ushort>(y,x) );
            break;
        case CV_16UC2:
            data += QObject::tr("Value: %1,").arg(img.at<cv::Vec2s>(y,x)[0]);
            data += QObject::tr("Alpha: %1").arg(img.at<cv::Vec2s>(y,x)[1]);
            break;
        case CV_16UC3:
            data += QObject::tr("not yet implemented (%1)").arg("CV_16UC3");
            break;
        case CV_16UC4:
            data += QObject::tr("not yet implemented (%1)").arg("CV_16UC4");
            break;
        case CV_16SC1:
            data += QObject::tr("not yet implemented (%1)").arg("CV_16SC1");
            break;
        case CV_16SC2:
            data += QObject::tr("not yet implemented (%1)").arg("CV_16SC2");
            break;
        case CV_16SC3:
            data += QObject::tr("not yet implemented (%1)").arg("CV_16SC3");
            break;
        case CV_16SC4:
            data += QObject::tr("not yet implemented (%1)").arg("CV_16SC4");
            break;
        case CV_32SC1:
            data += QObject::tr("not yet implemented (%1)").arg("CV_32SC1");
            break;
        case CV_32SC2:
            data += QObject::tr("not yet implemented (%1)").arg("CV_32SC2");
            break;
        case CV_32SC3:
            data += QObject::tr("not yet implemented (%1)").arg("CV_32SC3");
            break;
        case CV_32SC4:
            data += QObject::tr("not yet implemented (%1)").arg("CV_32SC4");
            break;
        case CV_32FC1:
            data += QObject::tr("Value: %1").arg( img.at<float>(y,x) );
            break;
        case CV_32FC2:
            data += QObject::tr("Value: %1,").arg(img.at<cv::Vec2f>(y,x)[0]);
            data += QObject::tr("Alpha: %1").arg(img.at<cv::Vec2f>(y,x)[1]);
            break;
        case CV_32FC3:
            data += QObject::tr("not yet implemented (%1)").arg("CV_32FC3");
            break;
        case CV_32FC4:
            data += QObject::tr("not yet implemented (%1)").arg("CV_32FC4");
            break;
        case CV_64FC1:
            data += QObject::tr("not yet implemented (%1)").arg("CV_64FC1");
            break;
        case CV_64FC2:
            data += QObject::tr("not yet implemented (%1)").arg("CV_64FC2");
            break;
        case CV_64FC3:
            data += QObject::tr("not yet implemented (%1)").arg("CV_64FC3");
            break;
        case CV_64FC4:
            data += QObject::tr("not yet implemented (%1)").arg("CV_64FC4");
            break;
        default:
            break;
    }

    return data;
}
