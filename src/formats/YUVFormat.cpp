
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
#include <QtCore/QFileInfo>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "YUVFormat.h"

#include "MiscFunctions.h"
#include "LogHandler.h"
#include "libyuv.h"

YUVFormat::YUVFormat(QObject *parent) : BaseFormat(parent)
{
    m_type = metaObject()->className();
    m_name = tr("Raw YUV");
    m_desc = tr("YUV uses a color space with a reduced bandwidth for chrominance components. Support only in raw format.");

    m_properties << new FormatProperty( tr("Size X"), tr("rows of the image in pixel"),QString::number( 352 ) );
    m_properties << new FormatProperty( tr("Size Y"), tr("colums of the image in pixel"),QString::number( 288 ) );
    m_properties << new FormatProperty( tr("Sampling"), tr("Sampling format of the image. Can be I420, I422, ... please see fourcc.org for a complete list"),"I420");

    m_mapStringToType.insert("I420",  libyuv::FOURCC_I420  ); 
    m_mapStringToType.insert( "I422",  libyuv::FOURCC_I422  ); 
    m_mapStringToType.insert( "I444",  libyuv::FOURCC_I444  ); 
    m_mapStringToType.insert( "I411",  libyuv::FOURCC_I411  ); 
    m_mapStringToType.insert( "I400",  libyuv::FOURCC_I400  ); 
    m_mapStringToType.insert( "NV21",  libyuv::FOURCC_NV21  ); 
    m_mapStringToType.insert( "NV12",  libyuv::FOURCC_NV12  ); 
    m_mapStringToType.insert( "YUY2",  libyuv::FOURCC_YUY2  ); 
    m_mapStringToType.insert( "UYVY",  libyuv::FOURCC_UYVY  ); 
    // 2 Secondary YUV formats: row biplanar.
    m_mapStringToType.insert( "M420",  libyuv::FOURCC_M420  ); 
    m_mapStringToType.insert( "Q420",  libyuv::FOURCC_Q420  ); 
    // 1 Primary Compressed YUV format.
    m_mapStringToType.insert( "MJPG",  libyuv::FOURCC_MJPG  ); 
    // 5 Auxiliary YUV variations: 3 with U and V planes are swapped, 1 Alias. 
    m_mapStringToType.insert( "YV12",  libyuv::FOURCC_YV12  ); 
    m_mapStringToType.insert( "YV16",  libyuv::FOURCC_YV16  ); 
    m_mapStringToType.insert( "YV24",  libyuv::FOURCC_YV24  ); 
    m_mapStringToType.insert( "YU12",  libyuv::FOURCC_YU12  );   // Linux version of I420.
    m_mapStringToType.insert( "J420",  libyuv::FOURCC_J420  ); 
    m_mapStringToType.insert( "J400",  libyuv::FOURCC_J400  ); 
    // 14 Auxiliary aliases.  CanonicalFourCC() maps these to canonical fourcc.
    m_mapStringToType.insert( "IYUV",  libyuv::FOURCC_IYUV  );   // Alias for I420.
    m_mapStringToType.insert( "YU16",  libyuv::FOURCC_YU16  );   // Alias for I422.
    m_mapStringToType.insert( "YU24",  libyuv::FOURCC_YU24  );   // Alias for I444.
    m_mapStringToType.insert( "YUYV",  libyuv::FOURCC_YUYV  );   // Alias for YUY2.
    m_mapStringToType.insert( "YUVS",  libyuv::FOURCC_YUVS  );   // Alias for YUY2 on Mac.
    m_mapStringToType.insert( "HDYC",  libyuv::FOURCC_HDYC  );   // Alias for UYVY.
    m_mapStringToType.insert( "2VUY",  libyuv::FOURCC_2VUY  );   // Alias for UYVY on Mac.
    m_mapStringToType.insert( "JPEG",  libyuv::FOURCC_JPEG  );   // Alias for MJPG.
    m_mapStringToType.insert( "DMB1",  libyuv::FOURCC_DMB1  );   // Alias for MJPG on Mac.
    m_mapStringToType.insert( "H264",  libyuv::FOURCC_H264 );

    m_properties.last()->availableValues = m_mapStringToType.keys();

    // reverse map ...
    foreach ( const QString &key, m_mapStringToType.keys() )
    m_mapTypeToString.insert(m_mapStringToType[key],key);

    // init settings
    loadSettings();
}

YUVFormat::~YUVFormat()
{
}

cv::Mat YUVFormat::decode(const QString &file, bool &ok)
{
    cv::Mat mat;
    ok = false;

    LogHandler::getInstance()->reportDebug( QString("Try to decode %1 %2x%3 sampling %4").arg( QFileInfo(file).fileName() )
                                                .arg( m_properties[SIZE_X]->value.toString() )
                                                .arg( m_properties[SIZE_Y]->value.toString() )
                                                .arg( m_properties[SAMPLING]->value.toString() )
                                            );

    // check parameters validity
    if (m_properties[SIZE_X]->value.toInt() < 0 ||
        m_properties[SIZE_Y]->value.toInt() < 0 )
    {
        LogHandler::getInstance()->reportError( QString("Invalid image size %1x%2").arg( m_properties[SIZE_X]->value.toString() ).arg( m_properties[SIZE_Y]->value.toString() ) );
        return mat;
    }

    if ( !m_mapStringToType.keys().contains( m_properties[SAMPLING]->value.toString() ) )
    {
        LogHandler::getInstance()->reportError( QString("Invalid sampling mode %1").arg( m_properties[SAMPLING]->value.toString() ) );
        return mat;
    }

    mat = decodeYUV(m_properties[SIZE_X]->value.toInt(),
                    m_properties[SIZE_Y]->value.toInt(),
                    m_properties[SAMPLING]->value.toString(),
                    file,
                    ok
                    );

    return mat;
}

bool YUVFormat::accept(const QString&file)
{
    qDebug() << QFileInfo(file).suffix() << file;

    // search for YUV extension ...
    return QFileInfo(file).suffix().compare("yuv",Qt::CaseInsensitive) == 0;
}

cv::Mat YUVFormat::decodeYUV(int width, int height, const QString &sampling, const QString &file, bool &ok)
{
    cv::Mat mat;
    ok = false;

    FILE *fin = fopen(file.toStdString().c_str(), "rb");
    if (!fin)
    {
        LogHandler::getInstance()->reportError( QString("Unable to open file %1").arg(file) );
        return mat;
    }

    size_t totalSize = 0;

    switch (m_mapStringToType[sampling])
    {
        default:
        case libyuv::FOURCC_NV21:
        case libyuv::FOURCC_NV12:
        case libyuv::FOURCC_I420:
        case libyuv::FOURCC_M420:
        case libyuv::FOURCC_Q420:
        case libyuv::FOURCC_J420:
        {
            totalSize = width * height * 1.5;
            break;
        }
        case libyuv::FOURCC_I422:
        case libyuv::FOURCC_UYVY:
        {
            totalSize = width * height * 2;
            break;
        }
        case libyuv::FOURCC_I444:
        case libyuv::FOURCC_YV24:
        {
            totalSize = width * height * 3;
            break;
        }
        case libyuv::FOURCC_I411:
        {
            totalSize = width * height * 2;
            break;
        }
    }

    unsigned char * yuvBuf = (unsigned char*)malloc( totalSize * sizeof(unsigned char) );

    size_t bytesRead = fread(yuvBuf, sizeof(unsigned char), totalSize, fin);
    if (bytesRead != totalSize)
    {
        free (yuvBuf);
        LogHandler::getInstance()->reportError( QString("File size don't match requirement!!") );
        return mat;
    }

    unsigned char* argbBuf = (unsigned char*)malloc( width * height * 5 * sizeof(unsigned char) );
    memset ( argbBuf,0,width * height * 4 * sizeof(unsigned char) );

    /*
       int ConvertToARGB(const uint8* src_frame, size_t src_size,
       uint8* dst_argb, int dst_stride_argb,
       int crop_x, int crop_y,
       int src_width, int src_height,
       int crop_width, int crop_height,
       enum RotationMode rotation,
       uint32 format);
     */

    int res = libyuv::ConvertToARGB(yuvBuf, totalSize,
                                    argbBuf, width * 4,
                                    0, 0,
                                    width, height,
                                    width, height,
                                    libyuv::kRotate0,
                                    m_mapStringToType[sampling]);

    if (res < 0)
    {
        free (yuvBuf);
        free (argbBuf);
        return mat;
    }

    cv::Mat R = cv::Mat( height, width, CV_8UC1,0.0 );
    cv::Mat G = cv::Mat( height, width, CV_8UC1,0.0 );
    cv::Mat B = cv::Mat( height, width, CV_8UC1,0.0);

    unsigned char *ptARGB = argbBuf;

    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {
            R.at<unsigned char>(j,i) = *ptARGB++;
            G.at<unsigned char>(j,i) = *ptARGB++;
            B.at<unsigned char>(j,i) = *ptARGB++;
            ptARGB++;         // alpha is ignored
        }

    ok = true;

    std::vector<cv::Mat> array_to_merge;
    array_to_merge.push_back(B);
    array_to_merge.push_back(G);
    array_to_merge.push_back(R);
    cv::merge(array_to_merge, mat);

    free (yuvBuf);
    free (argbBuf);

    fclose (fin);
    return mat;
}
