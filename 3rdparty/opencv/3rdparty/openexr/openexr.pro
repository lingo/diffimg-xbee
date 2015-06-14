

TEMPLATE          = lib
TARGET            = openexr

CONFIG += staticlib

HEADERS += Half/*.h Iex/Iex*.h IlmThread/IlmThread*.h Imath/Imath*.h IlmImf/*.h

SOURCES += Half/half.cpp Iex/*.cpp IlmThread/*.cpp Imath/*.cpp IlmImf/*.cpp


INCLUDEPATH += ./ ./Half ./Iex ./IlmThread ./Imath ./IlmImf ../zlib

win32:DEFINES += _CRT_NONSTDC_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS


