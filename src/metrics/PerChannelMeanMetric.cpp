
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
    if(img1.format() == QImage::Format_ARGB32_Premultiplied && img2.format() == QImage::Format_ARGB32_Premultiplied) {
        qDebug() << "premult";
        for (int y=0; y<output.height(); y++) {
            uchar *dst = output.scanLine(y);
            const QRgb *src1 = (QRgb *)img1.constScanLine(y);
            const QRgb *src2 = (QRgb *)img2.constScanLine(y);
            for(int x=0; x < output.width(); ++x, ++src1, ++src2, ++dst) {
                const QRgb pixel1 = qUnpremultiply(*src1);
                const QRgb pixel2 = qUnpremultiply(*src2);

                const uint8_t r1 = qRed(pixel1)/3;
                const uint8_t r2 = qRed(pixel2)/3;
                const uint8_t g1 = qGreen(pixel1)/3;
                const uint8_t g2 = qGreen(pixel2)/3;
                const uint8_t b1 = qBlue(pixel1)/3;
                const uint8_t b2 = qBlue(pixel2)/3;

                const uint8_t rd = qAbs(r1 - r2);
                const uint8_t rg = qAbs(g1 - g2);
                const uint8_t rb = qAbs(b1 - b2);

                *dst = rd + rg + rb;
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
                const QRgb pixel1 = *src1;
                const QRgb pixel2 = *src2;

                const uint8_t r1 = qRed(pixel1)/3;
                const uint8_t r2 = qRed(pixel2)/3;
                const uint8_t g1 = qGreen(pixel1)/3;
                const uint8_t g2 = qGreen(pixel2)/3;
                const uint8_t b1 = qBlue(pixel1)/3;
                const uint8_t b2 = qBlue(pixel2)/3;

                const uint8_t rd = qAbs(r1 - r2);
                const uint8_t rg = qAbs(g1 - g2);
                const uint8_t rb = qAbs(b1 - b2);

                *dst = rd + rg + rb;
            }
        }
    }

    m_imageDiff = output;
}
