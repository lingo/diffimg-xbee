

OPENCV_ROOT = $${PWD}/..

TEMPLATE          = lib
TARGET            = opencv_highgui

#DESTDIR           = $${QWT_ROOT}/lib

CONFIG += staticlib

HEADERS += include/opencv2/highgui/*.hpp \
            src/grfmt*.hpp \
            src/bitstrm.hpp \
            src/precomp.hpp \
            src/utils.hpp 
            
# src/cap_ffmpeg_impl.hpp

   
SOURCES += src/grfmt*.cpp \
    src/cap.cpp \
    src/cap_images.cpp \
    src/loadsave.cpp \
    src/bitstrm.cpp \
    src/precomp.cpp \
    src/utils.cpp \
    src/window.cpp 
    
# src/cap_ffmpeg.cpp 



INCLUDEPATH += ./include ./ ../ \
               ../core/include \
               ../imgproc/include \
               ../3rdparty/libtiff \
               ../3rdparty/libpng \
               ../3rdparty/ \
               ../3rdparty/libjasper \
               ../3rdparty/openexr \
               ../3rdparty/openexr/IlmImf \
               ../3rdparty/openexr/Imath \
               ../3rdparty/openexr/Iex \
               ../3rdparty/openexr/Half \
               ../3rdparty/libjpeg \
               ../3rdparty/zlib
               