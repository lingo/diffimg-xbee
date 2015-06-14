

TEMPLATE          = lib
TARGET            = jpeg

CONFIG += staticlib

HEADERS += *.h

SOURCES += *.c


INCLUDEPATH += ./

win32:DEFINES += _CRT_NONSTDC_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS


