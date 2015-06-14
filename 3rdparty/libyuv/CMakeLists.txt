PROJECT (libyuv)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR} "include" )

FILE(GLOB include_file_list "source/*.cc" "include/*.h" "include/libyuv/*.h")

SET(LIB_SRC ${include_file_list}  )

ADD_LIBRARY(libyuv STATIC ${DIFF_SRC} ${LIB_SRC})

IF (MSVC)
# Turn off deprecation warnings
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_WARNINGS)
ENDIF(MSVC)



