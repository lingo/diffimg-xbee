PROJECT (PerceptualDiff)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

# this command finds Qt4 libraries and sets all required variables
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)

# add some useful macros and variables
# (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)
INCLUDE(${QT_USE_FILE})


FILE(GLOB include_file_list "*.h" )
FILE(GLOB cpp_file_list "*.cpp"  )

SET(MYLIB_HDRS
    ${include_file_list}
)

SET(MYLIB_SRCS
    ${cpp_file_list}
)

IF(CMAKE_COMPILER_IS_GNUCXX )
    ADD_DEFINITIONS( -Wall -Wextra )
ENDIF(CMAKE_COMPILER_IS_GNUCXX )

# qt4 definitinos
QT4_WRAP_CPP        ( MYLIB_MOC_SRCS ${MYLIB_HDRS} )
QT4_AUTOMOC         ( ${MYLIB_SRCS} )


ADD_LIBRARY(qtpropertybrowser STATIC ${MYLIB_SRCS} ${MYLIB_MOC_SRCS} )

IF (MSVC)
# Turn off deprecation warnings
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_WARNINGS)
ENDIF(MSVC)



