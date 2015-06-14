TEMPLATE = subdirs

CONFIG += warn_on debug_and_release

SUBDIRS += qtpropertybrowser perceptualdiff libyuv

win32|os2-g++ {

SUBDIRS += qwt opencv 

}

macx {

SUBDIRS += qwt 

}

