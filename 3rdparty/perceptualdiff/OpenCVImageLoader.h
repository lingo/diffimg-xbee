
/*
   OpenCVImageLoader.h
   Copyright (C) 2006 Yangli Hector Yee

   This program is free software; you can redistribute it and/or modify it under the terms of the
   GNU General Public License as published by the Free Software Foundation; either version 2 of the License,
   or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program;
   if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _OPENCVIMAGELOADER_H
#define _OPENCVIMAGELOADER_H

#include <string>

/** Class encapsulating an image containing R,G,B,A channels.
 *
 * Internal representation assumes data is in the ABGR format, with the RGB
 * color channels premultiplied by the alpha value.  Premultiplied alpha is
 * often also called "associated alpha" - see the tiff 6 specification for some
 * discussion - http://partners.adobe.com/asn/developer/PDFS/TN/TIFF6.pdf
 *
 */
 
class RGBAImage;

namespace cv
{
class Mat;
}

class OpenCVImageLoader
{

public:

    static bool WriteToFile(RGBAImage *, const char* filename);
    static RGBAImage* ReadFromFile(const char* filename);
    static RGBAImage* MatToRGBAImage(const cv::Mat &);
    static cv::Mat RGBAImageToMat(RGBAImage *);

};

#endif // _OPENCVIMAGELOADER_H
