


TEMPLATE          = lib
TARGET            = perceptualdiff

DESTDIR           = ./lib

CONFIG += staticlib

HEADERS += LPyramid.h \
            Metric.h \
            CompareArgs.h \
            RGBAImage.h \
            OpenCVImageLoader.h
            
SOURCES += LPyramid.cpp \
            Metric.cpp \
            CompareArgs.cpp \
            RGBAImage.cpp \
            OpenCVImageLoader.cpp

INCLUDEPATH += ./ \
    ../opencv/include \
    ../opencv/core/include \
    ../opencv/highgui/include \
    ../opencv/imgproc/include \
    /usr/include/opencv4


win32-msvc:DEFINES += _CRT_NONSTDC_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS
