
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

#include <QApplication>
#include <QPixmap>
#include <QImageReader>
#include <QImage>

#include "MiscFunctions.h"
#include "LogHandler.h"
#include "AppSettings.h"

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
