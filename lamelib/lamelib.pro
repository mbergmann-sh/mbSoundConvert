# libmp3lame 4.0 als statische Bibliothek (reiner Encoder, ohne autotools)
include(../common.pri)

TEMPLATE = lib
TARGET = mp3lame
CONFIG += staticlib c99 warn_off
CONFIG -= qt
DESTDIR = $$OUT_PWD/../lib

LAME = $$PWD/../third_party/lame

!exists($$LAME/libmp3lame/lame.c) {
    error("LAME-Quellen fehlen: $$LAME/libmp3lame/lame.c")
}

DEFINES += HAVE_CONFIG_H
INCLUDEPATH += $$PWD/config $$LAME/include $$LAME/libmp3lame $$LAME

win32-msvc*: DEFINES += _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS

SOURCES += \
    $$LAME/libmp3lame/VbrTag.c \
    $$LAME/libmp3lame/bitstream.c \
    $$LAME/libmp3lame/encoder.c \
    $$LAME/libmp3lame/fft.c \
    $$LAME/libmp3lame/gain_analysis.c \
    $$LAME/libmp3lame/id3tag.c \
    $$LAME/libmp3lame/lame.c \
    $$LAME/libmp3lame/mpglib_interface.c \
    $$LAME/libmp3lame/newmdct.c \
    $$LAME/libmp3lame/presets.c \
    $$LAME/libmp3lame/psymodel.c \
    $$LAME/libmp3lame/quantize.c \
    $$LAME/libmp3lame/quantize_pvt.c \
    $$LAME/libmp3lame/reservoir.c \
    $$LAME/libmp3lame/set_get.c \
    $$LAME/libmp3lame/tables.c \
    $$LAME/libmp3lame/takehiro.c \
    $$LAME/libmp3lame/util.c \
    $$LAME/libmp3lame/vbrquantize.c \
    $$LAME/libmp3lame/version.c

HEADERS += \
    $$LAME/include/lame.h \
    $$PWD/config/config.h
