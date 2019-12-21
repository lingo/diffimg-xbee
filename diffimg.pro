QT += core gui widgets

TARGET = diffimg
TEMPLATE = app

include(3rdparty/libyuv/libyuv.pri)
include(3rdparty/perceptualdiff/perceptualdiff.pri)
include(3rdparty/qtpropertybrowser/qtpropertybrowser.pri)

# directories setup
#isEmpty( INSTALL_MANDIR )  : INSTALL_MANDIR  = $$INSTALL_PREFIX/share/man
#isEmpty( INSTALL_DATADIR ) : INSTALL_DATADIR = $$INSTALL_PREFIX/share
#
#isEmpty( MYAPP_INSTALL_DESKTOP ) : MYAPP_INSTALL_DESKTOP = $$INSTALL_DATADIR/applications
#isEmpty( MYAPP_INSTALL_PIXMAPS ) : MYAPP_INSTALL_PIXMAPS = $$INSTALL_DATADIR/pixmaps
#isEmpty( MYAPP_INSTALL_DATA )    : MYAPP_INSTALL_DATA    = $$INSTALL_DATADIR/$$TARGET
#isEmpty( MYAPP_INSTALL_DIR )     : MYAPP_INSTALL_DIR     = $$INSTALL_LIBDIR/$$TARGET
#isEmpty( MYAPP_INSTALL_BINDIR )  : MYAPP_INSTALL_BINDIR  = $$INSTALL_BINDIR
#isEmpty( MYAPP_INSTALL_MAN )     : MYAPP_INSTALL_MAN     = $$INSTALL_MANDIR/man1
#isEmpty( MYAPP_INSTALL_TRANS )   : MYAPP_INSTALL_TRANS   = $$INSTALL_DATADIR/$$TARGET/locale

HEADERS += \
    $$PWD/src/AboutDialog.h \
    $$PWD/src/AppSettings.h \
    $$PWD/src/DiffImgWindow.h \
    $$PWD/src/FilesDialog.h \
    $$PWD/src/FilesManager.h \
    $$PWD/src/ImageNavigator.h \
    $$PWD/src/ImageView.h \
    $$PWD/src/LogHandler.h \
    $$PWD/src/WipeItem.h \
    $$PWD/src/WipeMethod.h \
    $$PWD/src/metrics/BaseMetric.h \
    $$PWD/src/metrics/MetricsManager.h \
    $$PWD/src/metrics/MetricsRegistering.h \
    $$PWD/src/metrics/PerceptualMetric.h \
    $$PWD/src/metrics/PerChannelMeanMetric.h \
    $$PWD/src/metrics/PerChannelMetric.h \
    $$PWD/src/metrics/PerLuminanceMetric.h \
    $$PWD/src/formats/BaseFormat.h \
    $$PWD/src/formats/FormatsManager.h \
    $$PWD/src/formats/FormatsRegistering.h \
    $$PWD/src/formats/YUVFormat.h \
    $$PWD/src/MiscFunctions.h \
    $$PWD/src/PropertyWidget.h \

SOURCES +=  \
    $$PWD/src/AboutDialog.cpp \
    $$PWD/src/AppSettings.cpp \
    $$PWD/src/DiffImgWindow.cpp \
    $$PWD/src/FilesDialog.cpp \
    $$PWD/src/FilesManager.cpp \
    $$PWD/src/ImageNavigator.cpp \
    $$PWD/src/ImageView.cpp \
    $$PWD/src/LogHandler.cpp \
    $$PWD/src/WipeItem.cpp \
    $$PWD/src/main.cpp \
    $$PWD/src/metrics/BaseMetric.cpp \
    $$PWD/src/metrics/MetricsManager.cpp \
    $$PWD/src/metrics/MetricsRegistering.cpp \
    $$PWD/src/metrics/PerceptualMetric.cpp \
    $$PWD/src/metrics/PerChannelMeanMetric.cpp \
    $$PWD/src/metrics/PerChannelMetric.cpp \
    $$PWD/src/metrics/PerLuminanceMetric.cpp \
    $$PWD/src/formats/BaseFormat.cpp \
    $$PWD/src/formats/FormatsManager.cpp \
    $$PWD/src/formats/FormatsRegistering.cpp \
    $$PWD/src/formats/YUVFormat.cpp \
    $$PWD/src/MiscFunctions.cpp \
    $$PWD/src/PropertyWidget.cpp \

FORMS += \
    $$PWD/ui/AboutDialog.ui \
    $$PWD/ui/DiffImgWindow.ui \
    $$PWD/ui/FilesDialog.ui

INCLUDEPATH += \
    $${DESTDIR} \
    $$PWD/src \
    $$PWD/src/metrics \
    $$PWD/src/formats

INCLUDEPATH += /usr/include/qwt
LIBS += -lqwt

LIBS += -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lopencv_core

INCLUDEPATH += /usr/include/opencv4

RESOURCES += $$PWD/res/$${TARGET}.qrc

#TRANSLATIONS =  $$PWD/lang/$${TARGET}_fr.ts # french 
#TRANSLATIONS += $$PWD/lang/$${TARGET}_cs.ts # Czech
#TRANSLATIONS += $$PWD/lang/$${TARGET}_it.ts # Italian
#TRANSLATIONS += $$PWD/lang/$${TARGET}_vi.ts # Vietnamese
#TRANSLATIONS += $$PWD/lang/$${TARGET}_pt.ts # Portuguese
#TRANSLATIONS += $$PWD/lang/$${TARGET}_sv.ts # Swedish
#TRANSLATIONS += $$PWD/lang/$${TARGET}_zh.ts # Chinese
#TRANSLATIONS += $$PWD/lang/$${TARGET}_ro.ts # Romanian
#TRANSLATIONS += $$PWD/lang/$${TARGET}_de.ts # German
#TRANSLATIONS += $$PWD/lang/$${TARGET}_ru.ts # Russian
#
#macx:ICON = res/$${TARGET}.icns
#macx:QMAKE_INFO_PLIST =  build/Info.plist
#macx:QMAKE_POST_LINK += ./postbuild-macx.sh

#win32:RC_FILE = $$PWD/res/$${TARGET}.rc

#win32-msvc*:DEFINES += _CRT_NONSTDC_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS

#macx {
#    # add auto creation of info.plist file
#    VERSION = $$system(grep VERSION $$PWD/src/AppSettings.h | awk \' { print $NF }\' | sed   \'s/\"//g\')
#    createplist.commands = $$PWD/misc/Info.plist.sh $$VERSION > $$PWD/misc/Info.plist
#    createplist.target = Info.plist1
#    createplist.depends = FORCE
#    PRE_TARGETDEPS += Info.plist1
#    QMAKE_EXTRA_TARGETS += createplist
#}

# .qm file for mingw and before setup.nsi
#win32-g++ {
#    # add auto compilation of .ts files
#    QMAKE_EXTRA_COMPILERS += copyQtTrans
#    for(TRANS,TRANSLATIONS){
#    lang = $$replace(TRANS, .*_([^/]*)\\.ts, \\1)
#    GLOBALTRANSLATIONS += $$[QT_INSTALL_TRANSLATIONS]/qt_$${lang}.qm
#    }
#    copyQtTrans.input         =  GLOBALTRANSLATIONS
#    copyQtTrans.output        = ${QMAKE_FILE_BASE}.qm
#    copyQtTrans.commands      = cp ${QMAKE_FILE_IN} $$PWDlang/
#    copyQtTrans.CONFIG       += no_link target_predeps
#    
#    # add auto compilation of .ts files
#    QMAKE_EXTRA_COMPILERS += lrelease 
#    lrelease.input         = TRANSLATIONS
#    lrelease.output        = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
#    lrelease.commands      = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm $$PWD/lang/${QMAKE_FILE_BASE}.qm
#    lrelease.CONFIG       += no_link target_predeps
#}
#
#
#unix:!macx {
#    # add auto compilation of .ts files
#    QMAKE_EXTRA_COMPILERS += lrelease
#    lrelease.input         = TRANSLATIONS
#    lrelease.output        = ${QMAKE_FILE_BASE}.qm
#    lrelease.commands      = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm $$PWD/lang/${QMAKE_FILE_BASE}.qm
#    lrelease.CONFIG       += no_link target_predeps
#
#    # to copy executable to /usr/local/bin directory
#    starter.files = ./release/$${TARGET}
#    starter.path = $$MYAPP_INSTALL_BINDIR
#
#    transl.files = $$PWD/lang/*.qm
#    transl.path = $$MYAPP_INSTALL_TRANS
#    
#    data.files += $$PWD/CREDITS.txt $$PWD/README.txt $$PWD/AUTHORS.txt $$PWD/Changelog.txt
#    data.path = $$MYAPP_INSTALL_DATA
#
#    # generate desktop file  
#    VERSION = $$system(grep VERSION $$PWD/src/AppSettings.h | awk \' { print $NF }\' | sed   \'s/\"//g\')
#    system( sh  diffimg-desktop.sh  $$INSTALL_PREFIX $$VERSION > diffimg.desktop )
#    desktop.files += $${TARGET}.desktop
#    desktop.path = $$MYAPP_INSTALL_DESKTOP
#
#    icons.files += $$PWD/res/$${TARGET}.png
#    icons.path = $$MYAPP_INSTALL_PIXMAPS
#
#    # generate manpage
#    system( sh  diffimg-manpage.sh > diffimg.1 )
#    system( gzip -9 -f diffimg.1  )
#    manual.files += diffimg.1.gz
#    manual.path = $$MYAPP_INSTALL_MAN
#
#    INSTALLS += starter transl data desktop icons manual
#}



