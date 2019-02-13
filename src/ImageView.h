
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

#ifndef _EDITORVIEW_H_
#define _EDITORVIEW_H_

// Qt
#include <QGraphicsView>
#include <QtCore/QPointF>

class QGraphicsPixmapItem;
class ImageNavigator;
class WipeItem;

class ImageView : public QGraphicsView
{
    Q_OBJECT

public:

    ImageView(QWidget * map);
    ~ImageView();

    //virtual void setImage(const QImage &newImg);

    void resetScale();
    void fitScale();
    qreal getScale() const;
    void setScale(qreal);

    void setShowOverview(bool);

    void setImage(const QImage &newImg);
    void setMask(const QImage &newMask);

    // functions for wipe effect
    void setWipeMode(bool);
    void setWipeMethod(int);
    void setWipeImage1(const QImage &img);
    void setWipeImage2(const QImage &img);

    // mask opacity (0.0->1.0)
    void setMaskOpacity(qreal);
    qreal maskOpacity() const;
    void setEnabledMask(bool);
    bool isMaskEnabled() const;
    void setEnabledMarker(bool);
    bool isMarkerEnabled() const;

    // Center
    const QPointF& getCenter() const
    {
        return m_centerPoint;
    }

public slots:

    void updateSceneRect(const QRectF &);
    void setCenter(const QPointF& position);
    void setMarkerPosition(const QPointF &);
    void zoomOut();
    void zoomIn();
    void zoom(double factor);

signals:

    void mouseMoved(const QPointF &pos);
    void scaleChanged(qreal);
    void somethingChanged( const QPointF &pt = QPointF() ); // scale or center point change

protected:

    //Take over the interaction
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mousePressEvent(QMouseEvent *mouseEvent);
    virtual void mouseReleaseEvent(QMouseEvent *mouseEvent);

    virtual void wheelEvent(QWheelEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual bool event(QEvent *e);

private:

    void initBackground();
    bool isImageInside();
    void drawMarker(QPainter &p);
    void drawLines(const QPointF &,QPainter &p);

    void createWipeItem();

    //Holds the current centerpoint for the view, used for panning and zooming
    QPointF m_centerPoint;
    QPixmap m_tileBg;

    ImageNavigator* m_navigator;

    QGraphicsPixmapItem *m_imageItem;
    QGraphicsPixmapItem *m_maskItem;

    // overview rendering
    float m_navigatorSize;
    float m_navigatorMargin;
    bool m_showNavigator;
    qreal m_maskOpacity;
    bool m_showMask;
    bool m_drag;
    bool m_showMarker;
    QPointF m_posMarker;
    bool m_wipeMode;
    int m_wipeMethod;
    WipeItem *m_wipeItem;
};

#endif // _EDITORVIEW_H_
