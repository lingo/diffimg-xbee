
QT += core gui widgets

CONFIG += ordered debug_and_release

TARGET = diffimg
TEMPLATE = app

# directories setup
isEmpty( INSTALL_PREFIX )  : INSTALL_PREFIX  = /usr/local

isEmpty( INSTALL_BINDIR )  : INSTALL_BINDIR  = $$INSTALL_PREFIX/bin
isEmpty( INSTALL_LIBDIR )  {
    INSTALL_LIBDIR  = $$INSTALL_PREFIX/lib
    #installing to lib64 on 64-bit platforms:
    contains (QMAKE_HOST.arch, x86_64) : INSTALL_LIBDIR = $$INSTALL_PREFIX/lib64
}

isEmpty( INSTALL_MANDIR )  : INSTALL_MANDIR  = $$INSTALL_PREFIX/share/man
isEmpty( INSTALL_DATADIR ) : INSTALL_DATADIR = $$INSTALL_PREFIX/share

isEmpty( MYAPP_INSTALL_DESKTOP ) : MYAPP_INSTALL_DESKTOP = $$INSTALL_DATADIR/applications
isEmpty( MYAPP_INSTALL_PIXMAPS ) : MYAPP_INSTALL_PIXMAPS = $$INSTALL_DATADIR/pixmaps
isEmpty( MYAPP_INSTALL_DATA )    : MYAPP_INSTALL_DATA    = $$INSTALL_DATADIR/$$TARGET
isEmpty( MYAPP_INSTALL_DIR )     : MYAPP_INSTALL_DIR     = $$INSTALL_LIBDIR/$$TARGET
isEmpty( MYAPP_INSTALL_BINDIR )  : MYAPP_INSTALL_BINDIR  = $$INSTALL_BINDIR
isEmpty( MYAPP_INSTALL_MAN )     : MYAPP_INSTALL_MAN     = $$INSTALL_MANDIR/man1
isEmpty( MYAPP_INSTALL_TRANS )   : MYAPP_INSTALL_TRANS   = $$INSTALL_DATADIR/$$TARGET/locale

HEADERS += \
    ../src/AboutDialog.h \
    ../src/AppSettings.h \
    ../src/DiffImgWindow.h \
    ../src/FilesDialog.h \
    ../src/FilesManager.h \
    ../src/ImageNavigator.h \
    ../src/ImageView.h \
    ../src/LogHandler.h \
    ../src/WipeItem.h \
    ../src/WipeMethod.h \
    ../src/metrics/BaseMetric.h \
    ../src/metrics/MetricsManager.h \
    ../src/metrics/MetricsRegistering.h \
    ../src/metrics/PerceptualMetric.h \
    ../src/metrics/PerChannelMeanMetric.h \
    ../src/metrics/PerChannelMetric.h \
    ../src/metrics/PerLuminanceMetric.h \
    ../src/formats/BaseFormat.h \
    ../src/formats/FormatsManager.h \
    ../src/formats/FormatsRegistering.h \
    ../src/formats/YUVFormat.h \
    ../src/MiscFunctions.h \ 
    ../src/PropertyWidget.h \
    ../src/PaypalButton.h
    
SOURCES +=  \
    ../src/AboutDialog.cpp \
    ../src/AppSettings.cpp \
    ../src/DiffImgWindow.cpp \
    ../src/FilesDialog.cpp \
    ../src/FilesManager.cpp \
    ../src/ImageNavigator.cpp \
    ../src/ImageView.cpp \
    ../src/LogHandler.cpp \
    ../src/WipeItem.cpp \
    ../src/main.cpp \
    ../src/metrics/BaseMetric.cpp \
    ../src/metrics/MetricsManager.cpp \
    ../src/metrics/MetricsRegistering.cpp \
    ../src/metrics/PerceptualMetric.cpp \
    ../src/metrics/PerChannelMeanMetric.cpp \
    ../src/metrics/PerChannelMetric.cpp \
    ../src/metrics/PerLuminanceMetric.cpp \
    ../src/formats/BaseFormat.cpp \
    ../src/formats/FormatsManager.cpp \
    ../src/formats/FormatsRegistering.cpp \
    ../src/formats/YUVFormat.cpp \
    ../src/MiscFunctions.cpp \ 
    ../src/PropertyWidget.cpp \
    ../src/PaypalButton.cpp
    
FORMS += \
    ../ui/*.ui
    

INCLUDEPATH += \
    $${DESTDIR} \
    ../3rdparty/qtpropertybrowser \
    ../3rdparty/perceptualdiff \
    ../3rdparty/libyuv/include \
    ../src \
    ../src/metrics \
    ../src/formats

# bundled libs are only available on visual
# Dont cross the streams. It would be bad ! :)
win32|os2-g++ {

INCLUDEPATH += \
    ../3rdparty/qwt \
    ../3rdparty/opencv/include \
    ../3rdparty/opencv/core/include \
    ../3rdparty/opencv/highgui/include \
    ../3rdparty/opencv/imgproc/include 

LIBS += -L../3rdparty/qwt/lib -lqwt \
        -L../3rdparty/libyuv/lib -llibyuv \
        -L../3rdparty/qtpropertybrowser/lib -lqtpropertybrowser \
        -L../3rdparty/perceptualdiff/lib -lperceptualdiff \
        -L../3rdparty/opencv/imgproc/release -lopencv_imgproc  \
        -L../3rdparty/opencv/highgui/release -lopencv_highgui \
        -L../3rdparty/opencv/core/release -lopencv_core \
        -L../3rdparty/opencv/3rdparty/openexr/release -lopenexr \
        -L../3rdparty/opencv/3rdparty/libjasper/release -ljasper \
        -L../3rdparty/opencv/3rdparty/libpng/release -lpng \
        -L../3rdparty/opencv/3rdparty/libtiff/release -ltiff \
        -L../3rdparty/opencv/3rdparty/libjpeg/release -ljpeg \
        -L../3rdparty/opencv/3rdparty/zlib/release -lz
        
}

os2-g++ {
LIBS += -lpthread
}

macx {
INCLUDEPATH +=../3rdparty/qwt
LIBS += -L../3rdparty/qwt/lib -lqwt \
        -L../3rdparty/qtpropertybrowser/lib -lqtpropertybrowser \
        -L../3rdparty/libyuv/lib -llibyuv \
        -L../3rdparty/perceptualdiff/lib -lperceptualdiff \
         -lopencv_imgproc  \
         -lopencv_highgui \
         -lopencv_core 
}
unix:!macx {

INCLUDEPATH += /usr/include/qwt 

LIBS += -lqwt \
        -L../3rdparty/qtpropertybrowser/lib -lqtpropertybrowser \
        -L../3rdparty/libyuv/lib -llibyuv \
        -L../3rdparty/perceptualdiff/lib -lperceptualdiff \
         -lopencv_imgcodecs \
         -lopencv_imgproc  \
         -lopencv_highgui \
         -lopencv_core 
}

INCLUDEPATH += /usr/include/opencv4

    
RESOURCES += ../res/$${TARGET}.qrc


TRANSLATIONS =  ../lang/$${TARGET}_fr.ts # french 
TRANSLATIONS += ../lang/$${TARGET}_cs.ts # Czech
TRANSLATIONS += ../lang/$${TARGET}_it.ts # Italian
TRANSLATIONS += ../lang/$${TARGET}_vi.ts # Vietnamese
TRANSLATIONS += ../lang/$${TARGET}_pt.ts # Portuguese
TRANSLATIONS += ../lang/$${TARGET}_sv.ts # Swedish
TRANSLATIONS += ../lang/$${TARGET}_zh.ts # Chinese
TRANSLATIONS += ../lang/$${TARGET}_ro.ts # Romanian
TRANSLATIONS += ../lang/$${TARGET}_de.ts # German
TRANSLATIONS += ../lang/$${TARGET}_ru.ts # Russian

macx:ICON = ../res/$${TARGET}.icns
macx:QMAKE_INFO_PLIST =  ./Info.plist
macx:QMAKE_POST_LINK += ./postbuild-macx.sh

win32:RC_FILE = ../res/$${TARGET}.rc

win32-msvc*:DEFINES += _CRT_NONSTDC_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS
*-g++*!macx:QMAKE_CXXFLAGS += -Wno-enum-compare -Wno-switch

Release:DESTDIR = release
Release:OBJECTS_DIR = release
Release:MOC_DIR = release
Release:RCC_DIR = release
Release:UI_DIR = release

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug
Debug:MOC_DIR = debug
Debug:RCC_DIR = debug
Debug:UI_DIR = debug

macx {
    # add auto creation of info.plist file
    VERSION = $$system(grep VERSION ../src/AppSettings.h | awk \' { print $NF }\' | sed   \'s/\"//g\')
    createplist.commands = $$PWD/Info.plist.sh $$VERSION > $$PWD/Info.plist
    createplist.target = Info.plist1
    createplist.depends = FORCE
    PRE_TARGETDEPS += Info.plist1
    QMAKE_EXTRA_TARGETS += createplist
}

# .qm file for mingw and before setup.nsi
win32-g++ {
    # add auto compilation of .ts files
    QMAKE_EXTRA_COMPILERS += copyQtTrans
    for(TRANS,TRANSLATIONS){
    lang = $$replace(TRANS, .*_([^/]*)\\.ts, \\1)
    GLOBALTRANSLATIONS += $$[QT_INSTALL_TRANSLATIONS]/qt_$${lang}.qm
    }
    copyQtTrans.input         =  GLOBALTRANSLATIONS
    copyQtTrans.output        = ${QMAKE_FILE_BASE}.qm
    copyQtTrans.commands      = cp ${QMAKE_FILE_IN} ../lang/
    copyQtTrans.CONFIG       += no_link target_predeps
    
    # add auto compilation of .ts files
    QMAKE_EXTRA_COMPILERS += lrelease 
    lrelease.input         = TRANSLATIONS
    lrelease.output        = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
    lrelease.commands      = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm ../lang/${QMAKE_FILE_BASE}.qm
    lrelease.CONFIG       += no_link target_predeps
}


unix:!macx {
    # add auto compilation of .ts files
    QMAKE_EXTRA_COMPILERS += lrelease
    lrelease.input         = TRANSLATIONS
    lrelease.output        = ${QMAKE_FILE_BASE}.qm
    lrelease.commands      = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm ../lang/${QMAKE_FILE_BASE}.qm
    lrelease.CONFIG       += no_link target_predeps

    # to copy executable to /usr/local/bin directory
    starter.files = ./release/$${TARGET}
    starter.path = $$MYAPP_INSTALL_BINDIR

    transl.files = ../lang/*.qm
    transl.path = $$MYAPP_INSTALL_TRANS
    
    data.files += ../CREDITS.txt ../README.txt ../AUTHORS.txt ../Changelog.txt
    data.path = $$MYAPP_INSTALL_DATA

    # generate desktop file  
    VERSION = $$system(grep VERSION ../src/AppSettings.h | awk \' { print $NF }\' | sed   \'s/\"//g\')
    system( sh  diffimg-desktop.sh  $$INSTALL_PREFIX $$VERSION > diffimg.desktop )
    desktop.files += $${TARGET}.desktop
    desktop.path = $$MYAPP_INSTALL_DESKTOP

    icons.files += ../res/$${TARGET}.png
    icons.path = $$MYAPP_INSTALL_PIXMAPS

    # generate manpage
    system( sh  diffimg-manpage.sh > diffimg.1 )
    system( gzip -9 -f diffimg.1  )
    manual.files += diffimg.1.gz
    manual.path = $$MYAPP_INSTALL_MAN

    INSTALLS += starter transl data desktop icons manual
}



