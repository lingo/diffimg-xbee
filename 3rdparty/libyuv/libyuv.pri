HEADERS += \
    $$PWD/include/libyuv.h \
    $$PWD/include/libyuv/basic_types.h \
    $$PWD/include/libyuv/compare.h \
    $$PWD/include/libyuv/convert.h \
    $$PWD/include/libyuv/convert_argb.h \
    $$PWD/include/libyuv/convert_from.h \
    $$PWD/include/libyuv/convert_from_argb.h \
    $$PWD/include/libyuv/cpu_id.h \
    $$PWD/include/libyuv/format_conversion.h \
    $$PWD/include/libyuv/mjpeg_decoder.h \
    $$PWD/include/libyuv/planar_functions.h \
    $$PWD/include/libyuv/rotate.h \
    $$PWD/include/libyuv/rotate_argb.h \
    $$PWD/include/libyuv/row.h \
    $$PWD/include/libyuv/scale.h \
    $$PWD/include/libyuv/scale_argb.h \
    $$PWD/include/libyuv/scale_row.h \
    $$PWD/include/libyuv/version.h \
    $$PWD/include/libyuv/video_common.h

SOURCES += \
    $$PWD/source/compare.cc \
    $$PWD/source/compare_common.cc  \
    $$PWD/source/compare_neon.cc \
    $$PWD/source/compare_posix.cc \
    $$PWD/source/compare_win.cc \
    $$PWD/source/convert.cc \
    $$PWD/source/convert_argb.cc  \
    $$PWD/source/convert_from.cc \
    $$PWD/source/convert_from_argb.cc \
    $$PWD/source/convert_jpeg.cc \
    $$PWD/source/convert_to_argb.cc \
    $$PWD/source/convert_to_i420.cc \
    $$PWD/source/cpu_id.cc \
    $$PWD/source/format_conversion.cc \
    $$PWD/source/mjpeg_decoder.cc \
    $$PWD/source/mjpeg_validate.cc \
    $$PWD/source/planar_functions.cc \
    $$PWD/source/rotate.cc \
    $$PWD/source/rotate_argb.cc \
    $$PWD/source/rotate_mips.cc \
    $$PWD/source/rotate_neon.cc \
    $$PWD/source/row_any.cc \
    $$PWD/source/row_common.cc \
    $$PWD/source/row_mips.cc \
    $$PWD/source/row_neon.cc \
    $$PWD/source/row_posix.cc \
    $$PWD/source/row_win.cc \
    $$PWD/source/scale.cc \
    $$PWD/source/scale_argb.cc \
    $$PWD/source/scale_common.cc \
    $$PWD/source/scale_mips.cc \
    $$PWD/source/scale_neon.cc \
    $$PWD/source/scale_posix.cc \
    $$PWD/source/scale_win.cc \
    $$PWD/source/video_common.cc

INCLUDEPATH += $$PWD/include \
