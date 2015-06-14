
/*
   RGBAImage.cpp
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
//#include "FreeImage.h"
#include <cstdio>
#include <cstring>

RGBAImage* RGBAImage::DownSample() const
{
    if (Width <=1 || Height <=1)
        return NULL;

    int nw = Width / 2;
    int nh = Height / 2;
    RGBAImage* img = new RGBAImage( nw, nh, Name.c_str() );
    for (int y = 0; y < nh; y++)
        for (int x = 0; x < nw; x++)
        {
            int d[4];

            // Sample a 2x2 patch from the parent image.
            d[0] = Get(2 * x + 0, 2 * y + 0);
            d[1] = Get(2 * x + 1, 2 * y + 0);
            d[2] = Get(2 * x + 0, 2 * y + 1);
            d[3] = Get(2 * x + 1, 2 * y + 1);
            int rgba = 0;

            // Find the average color.
            for (int i = 0; i < 4; i++)
            {
                int c = ( d[0] >> (8 * i) ) & 0xFF;
                c += ( d[1] >> (8 * i) ) & 0xFF;
                c += ( d[2] >> (8 * i) ) & 0xFF;
                c += ( d[3] >> (8 * i) ) & 0xFF;
                c /= 4;
                rgba |= (c & 0xFF) << (8 * i);
            }
            img->Set(x, y, rgba);
        }
    return img;
}

