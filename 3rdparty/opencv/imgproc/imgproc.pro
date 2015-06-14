

OPENCV_ROOT = $${PWD}/..

TEMPLATE          = lib
TARGET            = opencv_imgproc

#DESTDIR           = $${QWT_ROOT}/lib

CONFIG += staticlib

HEADERS += include/opencv2/imgproc/*.hpp


SOURCES += src/*.cpp

INCLUDEPATH += ./include ./ ../ ../core/include