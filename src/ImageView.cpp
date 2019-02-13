
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

// Qt
#include <QtCore/QtDebug>

#include <QMouseEvent>
#include <QGraphicsScene>

#include <QGraphicsPixmapItem>

//#include <QtOpenGL/QGLWidget>

#include "ImageView.h"
#include "ImageNavigator.h"
#include "WipeItem.h"
#include "WipeMethod.h"

const double     scaleFactor = 1.3; //How fast we zoom

ImageView::ImageView(QWidget * parent) : QGraphicsView(parent),
    m_navigator(NULL),
    m_imageItem(NULL),
    m_maskItem(NULL),
    m_navigatorSize(0.15f),
    m_navigatorMargin(10),
    m_showNavigator(true),
    m_maskOpacity(0.5f),
    m_showMask(true),
    m_drag(false),
    m_showMarker(false),
    m_wipeMode(false),
    m_wipeItem(NULL),
    m_wipeMethod(WipeMethod::WIPE_HORIZONTAL)
{
    // OpenGL
    //setViewport(new QGLWidget(this));

    // GUI
    setContentsMargins(1,1,1,1);

    // Initialize properties
    setMouseTracking(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setViewportUpdateMode (QGraphicsView::FullViewportUpdate); // because artifact (view rect diff scene rect !!)

    // Center
    m_centerPoint = QPointF(0.0f,0.0f);
    centerOn(m_centerPoint);
    initBackground();

    QGraphicsScene *scene = new QGraphicsScene(this);

    // trace change in the scene (in order to view more than only scene rect
    connect( scene,SIGNAL( sceneRectChanged ( const QRectF &) ),this,SLOT( updateSceneRect(const QRectF &) ) );
    setScene(scene);

    // navigator windows
    m_navigator = new ImageNavigator(this);
    connect( m_navigator, SIGNAL( moveView(const QPointF &) ), this, SLOT( setCenter(const QPointF &) ) );
    connect( m_navigator, SIGNAL( moveView(const QPointF &) ), this, SIGNAL( somethingChanged(const QPointF &) ) ); // must inform "connected" views
}

ImageView::~ImageView()
{
}

void ImageView::setMaskOpacity(qreal opa)
{
    m_maskOpacity = opa;
    if (m_maskItem)
        m_maskItem->setOpacity(m_maskOpacity);
}

qreal ImageView::maskOpacity() const
{
    return m_maskOpacity;
}

void ImageView::setEnabledMask(bool val)
{
    m_showMask = val;
    if (m_maskItem)
    {
        m_maskItem->setVisible(m_showMask);
        viewport()->update();
    }
}

bool ImageView::isMaskEnabled() const
{
    return m_showMask;
}

void ImageView::setEnabledMarker(bool val)
{
    m_showMarker = val;
    viewport()->update();
}

bool ImageView::isMarkerEnabled() const
{
    return m_showMarker;
}

void ImageView::setShowOverview(bool val)
{
    m_showNavigator = val;
    viewport()->update();
}

void ImageView::setScale(qreal val)
{
    setMatrix( QMatrix() );
    scale(val,val);
    emit scaleChanged( transform().m22() );
}

qreal ImageView::getScale() const
{
    return transform().m22();
}

void ImageView::setImage(const QImage &newImg)
{
    m_navigator->setImage( newImg );  // reset overview

    if (!m_imageItem)
        m_imageItem = scene()->addPixmap( QPixmap::fromImage(newImg) );
    else
        m_imageItem->setPixmap( QPixmap::fromImage(newImg) );
    scene()->setSceneRect( newImg.rect() );
}

void ImageView::setMask(const QImage &newMask)
{
    if (!m_maskItem)
        m_maskItem = scene()->addPixmap( QPixmap::fromImage(newMask) );
    else
        m_maskItem->setPixmap( QPixmap::fromImage(newMask) );
    m_maskItem->setOpacity(m_maskOpacity);
    m_maskItem->setVisible(m_showMask);
}

void ImageView::updateSceneRect(const QRectF &rect)
{
    // the view can "see" sceneRect x 2
    setSceneRect( rect.adjusted(-rect.width() / 2,-rect.height() / 2,rect.width() / 2,rect.height() / 2) );
}

bool ImageView::isImageInside()
{
    // view rect is completely inside the image rect ?
    if ( scene() )
        return mapToScene( rect() ).boundingRect().contains( scene()->sceneRect() );
    else
        return false;
}

void ImageView::paintEvent(QPaintEvent* event)
{
    QPainter painter( viewport() );

    // draw background
    painter.drawTiledPixmap(rect(), m_tileBg);
    QGraphicsView::paintEvent(event);

    if (!isImageInside() && m_showNavigator)
    {
        if ( m_navigator->isVisible() )
        {
            // top left placement
            m_navigator->move(m_navigatorMargin, m_navigatorMargin);
        }
        else
        {
            m_navigator->show();
            m_navigator->update();
        }
    }
    else
    {
        m_navigator->hide();
    }

    if (m_showMarker)
        drawMarker(painter);
}

bool ImageView::event(QEvent *e)
{
    switch ( e->type() )
    {
        case QEvent::Leave:
            emit mouseMoved( QPointF() );
            break;
        default:
            break;
    }
    return QGraphicsView::event(e);
}

void ImageView::initBackground()
{
    // recreate background tile
    m_tileBg = QPixmap(16, 16);
    m_tileBg.fill(Qt::white);
    QPainter pt(&m_tileBg);
    QColor color(202, 202, 202);
    pt.fillRect(0, 0, 8, 8, color);
    pt.fillRect(8, 8, 8, 8, color);
    pt.end();
}

void ImageView::fitScale()
{
    if ( scene() )
    {
        fitInView(scene()->sceneRect(), Qt::KeepAspectRatio );
        setCenter( scene()->sceneRect().center() );
        emit scaleChanged( transform().m22() );
        emit somethingChanged();
    }
}

void ImageView::resetScale()
{
    setMatrix( QMatrix() );
    setCenter( scene()->sceneRect().center() );
    emit scaleChanged( transform().m22() );
    emit somethingChanged();
}

void ImageView::mousePressEvent(QMouseEvent *event)
{
    m_drag = false;
    if (event->button() == Qt::LeftButton)
    {
        m_drag = true;
    }
    QGraphicsView::mousePressEvent(event);
}

void ImageView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_drag = false;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

/**
   *Handles the mouse move event
 */
void ImageView::mouseMoveEvent(QMouseEvent* event)
{
    QPointF mousePos = QGraphicsView::mapToScene( event->pos() );
    emit mouseMoved(mousePos);
    m_centerPoint = QGraphicsView::mapToScene( rect().center() );
    QGraphicsView::mouseMoveEvent(event);
    if (m_drag)
        emit somethingChanged();
}

/**
 * Zoom the view in and out.
 */

void ImageView::zoomOut()
{
    zoom(1.0/scaleFactor);
}

void ImageView::zoomIn()
{
    zoom(scaleFactor);
}

void ImageView::zoom(double factor)
{
    //Get the position of the mouse before scaling, in scene coords
    QPointF pointBeforeScale( QGraphicsView::mapToScene( mapFromGlobal(QCursor::pos()) ) );

    //Get the original screen centerpoint
    QPointF screenCenter = mapToScene( rect().center() );

    scale(factor, factor);

    emit scaleChanged( transform().m22() );

    //Get the position after scaling, in scene coords
    QPointF pointAfterScale( QGraphicsView::mapToScene( mapFromGlobal(QCursor::pos()) ) );

    //Get the offset of how the screen moved
    QPointF offset = pointBeforeScale - pointAfterScale;

    //Adjust to the new center for correct zooming
    QPointF newCenter = screenCenter + offset;
    setCenter(newCenter);

    // mouse position has changed!!
 //   emit mouseMoved( QGraphicsView::mapToScene( event->pos() ) );
    emit mouseMoved( QGraphicsView::mapToScene( mapFromGlobal(QCursor::pos()) ) );
    emit somethingChanged();
}

void ImageView::wheelEvent(QWheelEvent* event)
{
    //Scale the view ie. do the zoom
    if(event->delta() > 0 )
        zoomIn();
    else
        zoomOut();
 
/*
    if ( scaled )
    {
        //Get the position after scaling, in scene coords
        QPointF pointAfterScale( QGraphicsView::mapToScene( event->pos() ) );

        //Get the offset of how the screen moved
        QPointF offset = pointBeforeScale - pointAfterScale;

        //Adjust to the new center for correct zooming
        QPointF newCenter = screenCenter + offset;
        setCenter(newCenter);
    }

    // mouse position has changed!!
    emit mouseMoved( QGraphicsView::mapToScene( event->pos() ) );
    emit somethingChanged();
    */
}

void ImageView::setMarkerPosition(const QPointF &p)
{
    m_posMarker = mapFromScene(p);
    if (m_showMarker)
        viewport()->update();
}

void ImageView::setCenter(const QPointF& position)
{
    m_centerPoint = position;
    centerOn(m_centerPoint);
}

void ImageView::resizeEvent(QResizeEvent* event)
{
    //Scale the view ie. do the zoom
    double heightscale;

    // Not on the width direction widthscale = (double)event->size().width() / event->oldSize().width();
    heightscale = (double)event->size().height() / event->oldSize().height();

    // Not on the width direction double doscale = widthscale<heightscale?widthscale:heightscale;
    if ( heightscale > 0 )
    {
        scale(heightscale,heightscale);
        emit scaleChanged( transform().m22() );
    }

    // Set center
    centerOn(m_centerPoint);

    // navigator
    m_navigator->setViewPortRect( geometry() );

    //Call the subclass resize so the scrollbars are updated correctly
    QGraphicsView::resizeEvent(event);
}

void ImageView::drawLines(const QPointF &center, QPainter &p)
{
    int markerSize = 10;

    // horizontal lines
    p.drawLine( QPointF( center.x() - markerSize, center.y() ),
                QPointF( center.x() - markerSize / 2, center.y() ) );
    p.drawLine( QPointF( center.x() + markerSize / 2, center.y() ),
                QPointF( center.x() + markerSize, center.y() ) );

    // vertical lines
    p.drawLine( QPointF(center.x(), center.y() - markerSize ),
                QPointF(center.x(), center.y() - markerSize / 2) );
    p.drawLine( QPointF(center.x(), center.y() + markerSize / 2 ),
                QPointF(center.x(), center.y() + markerSize) );
}

void ImageView::drawMarker(QPainter &p)
{
    // draw a darker line to "highlight" cross
    p.save();

    QPen pen = p.pen();
    pen.setWidthF(3.0);
    pen.setColor( QColor(255,255,255,255) );
    p.setPen(pen);

    drawLines(m_posMarker, p);

    pen.setWidthF(2.0);
    pen.setColor( QColor(50,50,50,255) );
    p.setPen(pen);

    drawLines(m_posMarker, p);

    p.restore();
}

void ImageView::setWipeMode(bool val)
{
    m_wipeMode = val;
    if (val)
    {
        if (m_wipeItem)
            m_wipeItem->show();
    }
    else
    {
        if (m_wipeItem)
            m_wipeItem->hide();
    }
}

void ImageView::createWipeItem()
{
    if (!m_wipeItem)
    {
        m_wipeItem = new WipeItem();
        m_wipeItem->setWipeMethod(m_wipeMethod);
        scene()->addItem(m_wipeItem);
    }
    m_wipeItem->show();
}

void ImageView::setWipeMethod(int method)
{
    m_wipeMethod = method;
    if (m_wipeItem)
        m_wipeItem->setWipeMethod(m_wipeMethod);
}

void ImageView::setWipeImage1(const QImage &img)
{
    createWipeItem();

    m_wipeItem->setImage1(img);
}

void ImageView::setWipeImage2(const QImage &img)
{
    createWipeItem();
    m_wipeItem->setImage2(img);
}
