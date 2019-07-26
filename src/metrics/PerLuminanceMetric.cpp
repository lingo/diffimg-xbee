
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
    Q_ASSERT(m_image1.size() == m_image2.size());

    QImage output(m_image1.width(), m_image2.height(), QImage::Format_Grayscale8);

    QImage img1 = m_image1;
    if(img1.depth() < 32){
        img1 = img1.convertToFormat(img1.hasAlphaChannel() ?  QImage::Format_ARGB32 : QImage::Format_RGB32);
    }

    QImage img2 = m_image2;
    if(img2.depth() < 32){
        img2 = img2.convertToFormat(img2.hasAlphaChannel() ?  QImage::Format_ARGB32 : QImage::Format_RGB32);
    }


    const int pixelCount = output.width()*output.height();

    if(img1.format() == QImage::Format_ARGB32_Premultiplied && img2.format() == QImage::Format_ARGB32_Premultiplied) {
        qDebug() << "premult";
        for (int y=0; y<output.height(); y++) {
            uchar *dst = output.scanLine(y);
            const QRgb *src1 = (QRgb *)img1.constScanLine(y);
            const QRgb *src2 = (QRgb *)img2.constScanLine(y);
            for(int x=0; x < output.width(); ++x, ++src1, ++src2, ++dst) {
                const QRgb pixel1 = qUnpremultiply(*src1);
                const QRgb pixel2 = qUnpremultiply(*src2);
                *dst = qAbs(qGray(pixel1) - qGray(pixel2));
            }
        }
    } else if (img1.format() == QImage::Format_ARGB32_Premultiplied && img2.format() != QImage::Format_ARGB32_Premultiplied) {
        // TODO blah
        Q_ASSERT(img1.format() == QImage::Format_ARGB32_Premultiplied && img2.format() == QImage::Format_ARGB32_Premultiplied);
    } else {
        for (int y=0; y<output.height(); y++) {
            uchar *dst = output.scanLine(y);
            const QRgb *src1 = (QRgb *)img1.constScanLine(y);
            const QRgb *src2 = (QRgb *)img2.constScanLine(y);
            for(int x=0; x < output.width(); ++x, ++src1, ++src2, ++dst) {
                *dst = qAbs(qGray(*src1) - qGray(*src2));
            }
        }
    }

    m_opencvDiff = MiscFunctions::qImageToOpencvMat(output.convertToFormat(QImage::Format_RGB32)); // TODO opencv is crap
}
