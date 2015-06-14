

TEMPLATE          = lib
TARGET            = png

CONFIG += staticlib

HEADERS += *.h

SOURCES += *.c


INCLUDEPATH += ../zlib

win32:DEFINES += _CRT_NONSTDC_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS


