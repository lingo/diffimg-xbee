
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

#ifndef _WIPEITEM_H_
#define _WIPEITEM_H_

// Qt
#include <QGraphicsItem>

class QGraphicsSceneMouseEvent;

class WipeItem : public QGraphicsItem
{

public:

    WipeItem(QGraphicsItem *parent = 0);
    ~WipeItem();

    virtual QRectF	boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    void setImage1(const QImage &img);
    void setImage2(const QImage &img);
    void setWipeMethod(int method);

protected:

    virtual void	mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void	mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void	mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:

    QPixmap m_pix1;
    QPixmap m_pix2;
    bool m_wipe;
    QPointF m_wipePoint;
    int m_wipeMethod;

};

#endif // _WIPEITEM_H_
