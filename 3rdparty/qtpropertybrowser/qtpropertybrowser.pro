


TEMPLATE          = lib
TARGET            = qtpropertybrowser

DESTDIR           = ./lib
QT += widgets

CONFIG += staticlib

# HEADERS += *.h
HEADERS = qttreepropertybrowser.h qtvariantproperty.h qtpropertymanager.h qtpropertybrowser.h qteditorfactory.h qtpropertybrowserutils_p.h
SOURCES = qttreepropertybrowser.cpp qtvariantproperty.cpp qtpropertymanager.cpp qtpropertybrowser.cpp qteditorfactory.cpp qtpropertybrowserutils.cpp

#SOURCES += *.cpp

# RESOURCES += *.qrc

INCLUDEPATH += ./
