

OPENCV_ROOT = $${PWD}/..

TEMPLATE          = lib
TARGET            = opencv_core

#DESTDIR           = $${QWT_ROOT}/lib

CONFIG += staticlib

HEADERS += include/opencv2/core/*.hpp


SOURCES += src/*.cpp

INCLUDEPATH += ./include ./ ../ ../3rdparty/zlib