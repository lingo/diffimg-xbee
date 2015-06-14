
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

#ifndef _IMAGENAVIGATOR_H_
#define _IMAGENAVIGATOR_H_

#include <QtGui/QWidget>

class QGraphicsView;

class ImageNavigator :   public QWidget
{
    Q_OBJECT

public:

    explicit ImageNavigator(QGraphicsView * parent = 0, Qt::WindowFlags f = 0);
    virtual ~ImageNavigator() {}

    void setImage(const QImage &img);

    void setTransforms(QTransform* worldMatrix, QTransform* imgMatrix);

    void setViewPortRect(const QRectF &viewPortRect);

    virtual QSize sizeHint() const;

signals:

    void moveView(const QPointF &dxy);

protected:

    void resizeImg();
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent* event);
    QTransform getScaledImageMatrix();

private:

    QImage m_img;
    QImage m_imgT;
    QTransform* m_worldMatrix;
    QTransform* m_imgMatrix;
    QRectF m_viewPortRect;
    QPointF m_lastPos;
    QPointF m_enterPos;
    QColor m_backgroundColor;
    QGraphicsView *m_view;
};

#endif // _IMAGENAVIGATOR_H_
