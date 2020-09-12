find_package(Qt5 COMPONENTS Widgets REQUIRED)
# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(_SRCS
  qteditorfactory.cpp
  qtpropertybrowser.cpp
  qtpropertybrowserutils.cpp
  qtpropertymanager.cpp
  qttreepropertybrowser.cpp
  qtvariantproperty.cpp
  )

file(GLOB _IMPL_HDRS *.h)
file(GLOB _PUBLIC_HDRS Qt*)

set(_UI_FORMS
  )

set(_RESOURCES
  qtpropertybrowser.qrc
  )


set(TARGET_NAME ${PROJECT_NAME})

add_library(${TARGET_NAME}
  ${_SRCS}
  ${_UI_SRCS}
  ${_QRC_SRCS}
  ${_IMPL_HDRS}
  )

target_link_libraries(${TARGET_NAME} Qt5::Widgets)

######################### Installation Stuff ###########################
include(GenerateExportHeader)
generate_export_header(${TARGET_NAME})
