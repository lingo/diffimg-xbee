
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

#include "PerceptualMetric.h"

#include "MiscFunctions.h"
#include "LogHandler.h"

// Perceptual diff includes
#include "CompareArgs.h"
#include "Metric.h"
#include "RGBAImage.h"

#include "OpenCVImageLoader.h"

static int defaultLuminanceOnly = 0;
static float defaultFieldOfView = 45.0f;
static float defaultGamma = 2.2f;
static float defaultLuminance = 100.0f;
static float defaultColorFactor = 1.0f;
static int defaultDownSample = 0;

PerceptualMetric::PerceptualMetric(QObject *parent) : BaseMetric(parent),
    m_noDifference(true)
{
    m_type = metaObject()->className();
    m_name = tr("Perceptual difference");
    m_desc = tr("A perceptually based image metric. Copyright (C) 2006 Yangli Hector Yee");

    // init settings
    //loadSettings();
}

PerceptualMetric::~PerceptualMetric()
{
}

QPixmap PerceptualMetric::getLogo() const
{
    return QPixmap(":/diffimg/perceptual.png");
}

void PerceptualMetric::createInputParams()
{
    /*
       \t-fov deg       : Field of view in degrees (0.1 to 89.9)\n\
        \t-threshold p	 : #pixels p below which differences are ignored\n\
        \t-gamma g       : Value to convert rgb into linear space (default 2.2)\n\
        \t-luminance l   : White luminance (default 100.0 cdm^-2)\n\
        \t-luminanceonly : Only consider luminance; ignore chroma (color) in the comparison\n\
        \t-colorfactor   : How much of color to use, 0.0 to 1.0, 0.0 = ignore color.\n\
        \t-downsample    : How many powers of two to down sample the image.\n\
     */

    // FOV
    addInputParam( new MetricParam("FOV",tr("Field of view"),tr("Field of view in degrees (0.1 to 89.9)"),defaultFieldOfView) );

    // Gamma
    addInputParam( new MetricParam("Gamma",tr("Gamma of screen"),tr("Value to convert rgb into linear space (default 2.2)"),defaultGamma) );

    // Luminance
    addInputParam( new MetricParam("Luminance",tr("White luminance"),tr("White luminance (default 100.0 cdm^-2)"),defaultLuminance) );

    // Luminance only
    addInputParam( new MetricParam("LuminanceOnly",tr("Luminance only"),tr("Only consider luminance; ignore chroma (color) in the comparison"),defaultLuminanceOnly) );

    // FOV
    addInputParam( new MetricParam("ColorFactor",tr("Color factor"),tr("How much of color to use, 0.0 to 1.0, 0.0 = ignore color."),defaultColorFactor) );

    // FOV
    addInputParam( new MetricParam("Downsample",tr("Downsample"),tr("How many powers of two to down sample the image"),defaultDownSample) );
}

void PerceptualMetric::performDifference()
{
    CompareArgs args;
    m_noDifference = true;

    // update input params
    args.FieldOfView = getInputParam("FOV")->threshold.toFloat();
    args.Gamma = getInputParam("Gamma")->threshold.toFloat();
    args.Luminance = getInputParam("Luminance")->threshold.toFloat();
    args.LuminanceOnly = getInputParam("LuminanceOnly")->threshold.toInt();
    args.ColorFactor = getInputParam("ColorFactor")->threshold.toFloat();
    args.DownSample = getInputParam("Downsample")->threshold.toInt();
    args.ThresholdPixels = getOutputParam("ErrorNum")->threshold.toInt();

    args.ImgA = OpenCVImageLoader::MatToRGBAImage(m_opencvInput1);
    args.ImgB = OpenCVImageLoader::MatToRGBAImage(m_opencvInput2);
    args.ImgDiff = new RGBAImage(m_opencvInput1.cols, m_opencvInput1.rows,"");

    if (!args.ImgA ||!args.ImgB) // convert problem
    {
        LogHandler::getInstance()->reportError( "PerceptualDiff: Error during image conversion");
        return;
    }

    // perform comparison
    m_noDifference = Yee_Compare(args);

    // save diff image
    std::vector<cv::Mat> channels;
    cv::Mat rgbDiff = m_opencvDiff = OpenCVImageLoader::RGBAImageToMat(args.ImgDiff);
    cv::split(rgbDiff,channels);
    m_opencvDiff = channels[0];

    //OpenCVImageLoader::WriteToFile(args.ImgDiff ,"d:/tmp.png");
}
