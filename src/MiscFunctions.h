
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

#ifndef _MISCFUNCTIONS_H_
#define _MISCFUNCTIONS_H_

#include <QtCore/QString>
#include <QtGui/QImage>

namespace cv
{
class Mat;
}

struct DiffStruct
{
    QImage maskDiffImage; // mask image
    QImage diffImage; // diff image
    unsigned int nbErr;
    double meanError;
    double stantdardDeviation;
    double rmsDiff;

    // histograms
    int minError;
    int maxError;
    double valueR[2 * 256];
    double valueG[2 * 256];
    double valueB[2 * 256];
};

class MiscFunctions
{
public:

    enum DiffMethodType
    {
        METHOD_BYCHANNEL = 0,
        METHOD_BYCHANNELMEAN,
        METHOD_BYLIGHTNESS,
        METHOD_END
    };

    // images function
    static QString getAvailablesImageFormats();
    static QStringList getAvailablesImageFormatsList();
    static QMap<QString, QString> getLongImageFormats();

    // translations functions
    static QString getTranslationsPath( const QString &refLang = QString() );
    static QString getTranslationsFile(const QString &lang);
    static QMap<QString, QString> getAvailableLanguages();
    static void setDefaultLanguage();
    static void setLanguage(const QString& lang);

    static void updateApplicationIdentity();

    static bool stringToFile(const QString &data, const QString &filename);
    static QString fileToString(const QString &filename);

    static QString matTypeToText(int);
    static QString matDepthToText(int);
    static quint64 getFileSize(const QString &file);
    static QString bytesToString(quint64 bytes);
    static QString pixelsToString(quint64 pixels);

    // opencv tools
    static QImage opencvMatToQImage(const cv::Mat& mat, bool deepCopy = false);
    static cv::Mat qImageToOpencvMat(const QImage &);
    static cv::Mat searchDecoder(const QString &file);
    static void computeHistogramValue(const cv::Mat& Image, cv::Mat& Hist);
    static void ComputeHistogramBGR(const cv::Mat& Image,cv::Mat& HistB, cv::Mat& HistG, cv::Mat& HistR);
    static void computeHistogram1D(const cv::Mat& Image, cv::Mat& Hist);
    static QString dataToText(const cv::Mat& img, int x, int y);


};

#endif // _MISCFUNCTIONS_H_
