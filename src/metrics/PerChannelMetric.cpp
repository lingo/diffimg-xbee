
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

#include "PerChannelMetric.h"

#include "MiscFunctions.h"
#include "LogHandler.h"

PerChannelMetric::PerChannelMetric(QObject *parent) : BaseMetric(parent)
{
    m_type = metaObject()->className();
    m_name = tr("Difference per channel");
    m_desc = tr("Difference is computed channel per channel");

    updateParameters();

    // init settings
    //loadSettings();
}

PerChannelMetric::~PerChannelMetric()
{
}

QPixmap PerChannelMetric::getLogo() const
{
    return QPixmap(":/diffimg/perchannel.png");
}

void PerChannelMetric::performDifference()
{
    absdiff(m_opencvInput1,m_opencvInput2,m_opencvDiff);
}

void PerChannelMetric::updateParameters()
{
    // m_params << new MetricParam("RMSError",tr("RMS error"),tr("Root Mean Square error"),BaseMetric::defaultThresholdRMSError);
    // m_params << new MetricParam("MeanError",tr("Mean error"),tr("Mean absolute error"),BaseMetric::defaultThresholdMeanError);
    // m_params << new MetricParam("MaxError",tr("Max error"),tr("Maximal error"),BaseMetric::defaultThresholdMaxError);
    //m_params << new MetricParam("StandardDeviation",tr("Standard deviation"),tr("Standard deviation"),BaseMetric::defaultThresholdStandardDeviation);
    //m_params << new MetricParam("ErrorNum",tr("Error num (pixels)"),tr("Number of different pixels"),BaseMetric::defaultThresholdNumPixel);
}
