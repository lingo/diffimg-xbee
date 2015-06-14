################################################################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
################################################################

# qmake project file for building the qwt libraries

QWT_ROOT = $${PWD}/.
#include( $${QWT_ROOT}/qwtconfig.pri )
#include( $${QWT_ROOT}/qwtbuild.pri )
#include( $${QWT_ROOT}/qwtfunctions.pri )

TEMPLATE          = lib
TARGET            = qwt

DESTDIR           = $${QWT_ROOT}/lib

contains(QWT_CONFIG, QwtDll) {

    CONFIG += dll
    win32|symbian: DEFINES += QT_DLL QWT_DLL QWT_MAKEDLL
}
else {
    CONFIG += staticlib
} 

contains(QWT_CONFIG, QwtFramework) {

    CONFIG += lib_bundle
}

HEADERS += \
    qwt_abstract_scale_draw.h \
    qwt_clipper.h \
    qwt_color_map.h \
    qwt_column_symbol.h \
    qwt_curve_fitter.h \
    qwt_dyngrid_layout.h \
    qwt_global.h \
    qwt_interval.h \
    qwt_legend.h \
    qwt_legend_item.h \
    qwt_legend_itemmanager.h \
    qwt_math.h \
    qwt_null_paintdevice.h \
    qwt_painter.h \
    qwt_plot.h \
    qwt_plot_canvas.h \
    qwt_plot_curve.h \
    qwt_plot_dict.h \
    qwt_plot_grid.h \
    qwt_plot_histogram.h \
    qwt_plot_item.h \
    qwt_plot_layout.h \
    qwt_plot_seriesitem.h \
    qwt_point_3d.h \
    qwt_point_polar.h \
    qwt_scale_div.h \
    qwt_scale_draw.h \
    qwt_scale_engine.h \
    qwt_scale_map.h \
    qwt_scale_widget.h \
    qwt_series_data.h \
    qwt_spline.h \
    qwt_symbol.h \
    qwt_text.h \
    qwt_text_engine.h \
    qwt_text_label.h 

SOURCES += \
    qwt_abstract_scale_draw.cpp \
    qwt_clipper.cpp \
    qwt_color_map.cpp \
    qwt_column_symbol.cpp \
    qwt_curve_fitter.cpp \
    qwt_dyngrid_layout.cpp \
    qwt_interval.cpp \
    qwt_legend.cpp \
    qwt_legend_item.cpp \
    qwt_math.cpp \
    qwt_null_paintdevice.cpp \
    qwt_painter.cpp \
    qwt_plot.cpp \
    qwt_plot_axis.cpp \
    qwt_plot_canvas.cpp \
    qwt_plot_curve.cpp \
    qwt_plot_dict.cpp \
    qwt_plot_grid.cpp \
    qwt_plot_histogram.cpp \
    qwt_plot_item.cpp \
    qwt_plot_layout.cpp \
    qwt_plot_seriesitem.cpp \
    qwt_plot_xml.cpp \
    qwt_point_3d.cpp \
    qwt_point_polar.cpp \
    qwt_scale_div.cpp \
    qwt_scale_draw.cpp \
    qwt_scale_engine.cpp \
    qwt_scale_map.cpp \
    qwt_scale_widget.cpp \
    qwt_series_data.cpp \
    qwt_spline.cpp \
    qwt_symbol.cpp \
    qwt_text.cpp \
    qwt_text_engine.cpp \
    qwt_text_label.cpp 


    DEFINES += QWT_NO_SVG

# Install directives

#target.path    = $${QWT_INSTALL_LIBS}

#doc.files      = $${QWT_ROOT}/doc/html 
#unix:doc.files += $${QWT_ROOT}/doc/man
#doc.path       = $${QWT_INSTALL_DOCS}

#INSTALLS       = target doc

CONFIG(lib_bundle) {

    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.files = $${HEADERS}
    FRAMEWORK_HEADERS.path = Headers
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS
}
else {

    #headers.files  = $${HEADERS}
    #headers.path   = $${QWT_INSTALL_HEADERS}
    #INSTALLS += headers
}
