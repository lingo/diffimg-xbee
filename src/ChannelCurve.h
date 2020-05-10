#pragma once

// QWT
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>

class ChannelCurve : public QwtPlotCurve
{
public:
    ChannelCurve(const QString &title);

    void setColor(const QColor &color);
};
