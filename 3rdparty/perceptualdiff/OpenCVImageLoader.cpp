
/*
   FreeImageLoader.cpp
   Copyright (C) 2006 Yangli Hector Yee

   (This entire file was rewritten by Jim Tilander)

   This program is free software; you can redistribute it and/or modify it under the terms of the
   GNU General Public License as published by the Free Software Foundation; either version 2 of the License,
   or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program;
   if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "RGBAImage.h"
#include "OpenCVImageLoader.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui.hpp>

#include <cstdio>
#include <cstring>


bool OpenCVImageLoader::WriteToFile(RGBAImage *image, const char* filename)
{
    if (!image || !filename)
        return false;

    cv::Mat mat = RGBAImageToMat(image);

    /*
    int Width = image->Get_Width();
    int Height = image->Get_Height();

    cv::Mat mat = cv::Mat(Height, Width , CV_8UC4 );

    const unsigned int* source = image->Get_Data();
    for( int y = 0; y < Height; y++, source += Width )
    {
        unsigned int * ptr = mat.ptr<unsigned int>(y);
        //unsigned int* scanline = (unsigned int*)FreeImage_GetScanLine(bitmap, Height - y - 1 );
        memcpy(ptr, source, sizeof(source[0]) * Width);
    }
    */

    const bool result = cv::imwrite(filename,mat);


    return result;
}

RGBAImage* OpenCVImageLoader::ReadFromFile(const char* filename)
{
    cv::Mat img = cv::imread(filename);
    if(img.empty())
    {
        printf( "Failed to load the image %s\n", filename);
        return 0;
    }

    return  MatToRGBAImage(img);
/*
    const int w = img.cols;
    const int h = img.rows;

    RGBAImage* result = new RGBAImage(w, h, filename);

    cvtColor(img, img, CV_RGB2RGBA);

    // Copy the image over to our internal format, FreeImage has the scanlines bottom to top though.
    unsigned int* dest = result->Get_Data();

    for( int y = 0; y < h; y++, dest += w )
    {
        //const unsigned int* scanline = (const unsigned int*)FreeImage_GetScanLine(freeImage, h - y - 1 );
        const unsigned int * ptr = img.ptr<unsigned int>(y);
        memcpy(dest, ptr, sizeof(dest[0]) * w);
    }

    //FreeImage_Unload(freeImage);
    return result;
    */
}

RGBAImage* OpenCVImageLoader::MatToRGBAImage(const cv::Mat &img)
{
    if (img.empty() || (img.type() != CV_8UC3 && img.type() != CV_8UC4))
        return 0;

    cv::Mat tmpImg(img);

    const int w = img.cols;
    const int h = img.rows;

    RGBAImage* result = new RGBAImage(w, h, "unknown");

    if (img.type() == CV_8UC3)
        cvtColor(img, tmpImg, CV_RGB2RGBA);

    // Copy the image over to our internal format, FreeImage has the scanlines bottom to top though.
    unsigned int* dest = result->Get_Data();

    for( int y = 0; y < h; y++, dest += w )
    {
        //const unsigned int* scanline = (const unsigned int*)FreeImage_GetScanLine(freeImage, h - y - 1 );
        const unsigned int * ptr = tmpImg.ptr<unsigned int>(y);
        memcpy(dest, ptr, sizeof(dest[0]) * w);
    }

    return result;
}

cv::Mat OpenCVImageLoader::RGBAImageToMat(RGBAImage *image)
{
    if (!image)
        return cv::Mat();

    int Width = image->Get_Width();
    int Height = image->Get_Height();

    cv::Mat mat = cv::Mat(Height, Width , CV_8UC4 );

    const unsigned int* source = image->Get_Data();
    for( int y = 0; y < Height; y++, source += Width )
    {
        unsigned int * ptr = mat.ptr<unsigned int>(y);
        //unsigned int* scanline = (unsigned int*)FreeImage_GetScanLine(bitmap, Height - y - 1 );
        memcpy(ptr, source, sizeof(source[0]) * Width);
    }

    return mat;
}
