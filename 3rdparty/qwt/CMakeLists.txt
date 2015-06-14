project(Qwt) 


CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# this command finds Qt4 libraries and sets all required variables
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui  REQUIRED)

INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} ${CMAKE_BINARY_DIR} "./"  )

FILE(GLOB include_file_list "*.h" )
FILE(GLOB cpp_file_list "*.cpp"  )


SET(QWT_HDRS
    ${include_file_list}
)

SET(QWT_SRCS
    ${cpp_file_list}
)

QT4_WRAP_CPP        ( QWT_MOC_SRCS ${QWT_HDRS} )

IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
  CMAKE_POLICY(SET CMP0005 OLD)
ENDIF(COMMAND CMAKE_POLICY)

add_library(qwt STATIC ${QWT_SRCS} ${QWT_MOC_SRCS})


