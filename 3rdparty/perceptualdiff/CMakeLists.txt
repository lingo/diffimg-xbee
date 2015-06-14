PROJECT (PerceptualDiff)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

SET(DIFF_SRC  LPyramid.cpp RGBAImage.cpp CompareArgs.cpp Metric.cpp OpenCVImageLoader.cpp)

ADD_LIBRARY(PerceptualDiff STATIC ${DIFF_SRC} ${})

IF (MSVC)
# Turn off deprecation warnings
ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_WARNINGS)
ENDIF(MSVC)



