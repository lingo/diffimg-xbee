#include "ChannelCurve.h"

ChannelCurve::ChannelCurve(const QString &title) :
    QwtPlotCurve(title)
{
    setRenderHint(QwtPlotItem::RenderAntialiased);
}

void ChannelCurve::setColor(const QColor &color)
{
    QColor c = color;
    c.setAlpha(50);

    setPen(c);
    setBrush(c);
}
