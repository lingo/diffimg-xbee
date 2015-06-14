
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

#ifndef _YUVFORMATS_H_
#define _YUVFORMATS_H_

#include <QtCore/QSet>
#include <QtCore/QMap>

#include "BaseFormat.h"

class YUVFormat : public BaseFormat
{
    Q_OBJECT

public:

    explicit YUVFormat(QObject *parent = 0);
    virtual ~YUVFormat();

    virtual cv::Mat decode(const QString &file, bool &ok);
    
    virtual bool accept(const QString&file);

signals:

protected:

private slots:

private:

    enum typProperties
    {
        SIZE_X = 0,
        SIZE_Y,
        SAMPLING
    };

    cv::Mat decodeYUV(int width , int height, const QString &sampling, const QString &file, bool &ok);

    QSet<QString> m_availableSamplingMode;
    QMap<QString,int> m_mapStringToType;
    QMap<int,QString> m_mapTypeToString;

};

#endif // _YUVFORMATS_H_

