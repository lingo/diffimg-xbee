
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

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>

#include "PerLuminanceMetric.h"

#include "MiscFunctions.h"
#include "LogHandler.h"

PerLuminanceMetric::PerLuminanceMetric(QObject *parent) : BaseMetric(parent)
{
    m_type = metaObject()->className();
    m_name = tr("Difference per image luminance");
    m_desc = tr("Difference is computed between both luminance values defined as luminance(x,y) = 0.3*R(x,y)+0.59*G(x,y)+0.11*B(x,y)");

    // init settings
    //loadSettings();
}

PerLuminanceMetric::~PerLuminanceMetric()
{
}

QPixmap PerLuminanceMetric::getLogo() const
{
    return QPixmap(":/diffimg/perluminance.png");
}

void PerLuminanceMetric::performDifference()
{
    cv::Mat gray1;
    cv::Mat gray2;
    if (m_opencvInput1.channels()>=3)
    {
        cvtColor(m_opencvInput1,gray1,CV_RGB2GRAY);
        cvtColor(m_opencvInput2,gray2,CV_RGB2GRAY);
    }
    else
    {
        gray1 = m_opencvInput1;
        gray2 = m_opencvInput2;
    }

    absdiff(gray1,gray2,m_opencvDiff);

    //cv::imwrite("d:/toto2.png",gray1);
}
