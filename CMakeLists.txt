cmake_minimum_required(VERSION 3.9)

project(diffimg LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Qwt REQUIRED)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
add_executable(diffimg
    # Main sources
    src/AboutDialog.cpp
    src/AboutDialog.h
    src/AppSettings.cpp
    src/AppSettings.h
    src/ChannelCurve.cpp
    src/ChannelCurve.h
    src/DiffImgWindow.cpp
    src/DiffImgWindow.h
    src/FilesDialog.cpp
    src/FilesDialog.h
    src/FilesManager.cpp
    src/FilesManager.h
    src/ImageNavigator.cpp
    src/ImageNavigator.h
    src/ImageView.cpp
    src/ImageView.h
    src/IMessageHandler.h
    src/LogHandler.cpp
    src/LogHandler.h
    src/main.cpp
    src/MiscFunctions.cpp
    src/MiscFunctions.h
    src/PropertyWidget.cpp
    src/PropertyWidget.h
    src/WipeItem.cpp
    src/WipeItem.h
    src/WipeMethod.h

    # Metrics
    src/metrics/BaseMetric.cpp
    src/metrics/BaseMetric.h
    src/metrics/MetricsManager.cpp
    src/metrics/MetricsManager.h
    src/metrics/MetricsRegistering.cpp
    src/metrics/MetricsRegistering.h
    src/metrics/PerceptualMetric.cpp
    src/metrics/PerceptualMetric.h
    src/metrics/PerChannelMeanMetric.cpp
    src/metrics/PerChannelMeanMetric.h
    src/metrics/PerChannelMetric.cpp
    src/metrics/PerChannelMetric.h
    src/metrics/PerLuminanceMetric.cpp
    src/metrics/PerLuminanceMetric.h

    # UI files
    ui/AboutDialog.ui
    ui/DiffImgWindow.ui
    ui/FilesDialog.ui

    # Resource files
    res/diffimg.qrc

    ## 3rd party
    # Perceptual diff
    3rdparty/perceptualdiff/LPyramid.h
    3rdparty/perceptualdiff/Metric.h
    3rdparty/perceptualdiff/CompareArgs.h
    3rdparty/perceptualdiff/RGBAImage.h
    3rdparty/perceptualdiff/LPyramid.cpp
    3rdparty/perceptualdiff/Metric.cpp
    3rdparty/perceptualdiff/CompareArgs.cpp
    3rdparty/perceptualdiff/RGBAImage.cpp

    # Qt property browser
    3rdparty/qtpropertybrowser/qteditorfactory.cpp
    3rdparty/qtpropertybrowser/qteditorfactory.h
    3rdparty/qtpropertybrowser/qtpropertybrowser.cpp
    3rdparty/qtpropertybrowser/qtpropertybrowser.h
    3rdparty/qtpropertybrowser/qtpropertybrowserutils.cpp
    3rdparty/qtpropertybrowser/qtpropertybrowserutils_p.h
    3rdparty/qtpropertybrowser/qtpropertymanager.cpp
    3rdparty/qtpropertybrowser/qtpropertymanager.h
    3rdparty/qtpropertybrowser/qttreepropertybrowser.cpp
    3rdparty/qtpropertybrowser/qttreepropertybrowser.h
    3rdparty/qtpropertybrowser/qtvariantproperty.cpp
    3rdparty/qtpropertybrowser/qtvariantproperty.h
    )

target_include_directories(diffimg PRIVATE
    src
    src/metrics

    3rdparty/perceptualdiff
    3rdparty/qtpropertybrowser
    )
target_link_libraries(diffimg PRIVATE Qwt::Qwt Qt5::Widgets)
install(TARGETS diffimg DESTINATION bin)
