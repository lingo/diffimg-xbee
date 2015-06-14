
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

#include "PerChannelMeanMetric.h"

#include "MiscFunctions.h"
#include "LogHandler.h"

PerChannelMeanMetric::PerChannelMeanMetric(QObject *parent) : BaseMetric(parent)
{
    m_type = metaObject()->className();
    m_name = tr("Difference per channel mean");
    m_desc = tr("Difference is computed between both mean values defined as mean(x,y) = (R(x,y)+G(x,y)+B(x,y))/3");

    // init settings
    //loadSettings();
}

PerChannelMeanMetric::~PerChannelMeanMetric()
{
}

QPixmap PerChannelMeanMetric::getLogo() const
{
    return QPixmap(":/diffimg/perchannelmean.png");
}

void PerChannelMeanMetric::performDifference()
{
    // create two "mean" images
    cv::Mat mean1 = mat2mean(m_opencvInput1);
    cv::Mat mean2 = mat2mean(m_opencvInput2);
    absdiff(mean1,mean2,m_opencvDiff);

    //cv::imwrite("d:/toto2.png",mean1);
}

cv::Mat PerChannelMeanMetric::mat2mean(const cv::Mat& src)
{
    std::vector<cv::Mat> planes( src.channels() );
    cv::split(src,planes);
    float factor = 1.0 / src.channels();
    cv::Mat mean = factor * planes[0];
    for (int i = 1; i < m_opencvInput1.channels(); i++)
        mean += (factor * planes[i]);
    return mean;
}
