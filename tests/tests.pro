# Selbsttest für WAV-Reader + MP3-Konvertierung (ohne Qt). Ausführen: make check
include(../common.pri)

TEMPLATE = app
TARGET = test_converter
CONFIG += console testcase
CONFIG -= qt app_bundle
DESTDIR = $$OUT_PWD/../bin

INCLUDEPATH += $$PWD/../src
SOURCES += test_converter.cpp \
           $$PWD/../src/core/Converter.cpp \
           $$PWD/../src/core/WavReader.cpp
HEADERS += $$PWD/../src/core/Converter.h \
           $$PWD/../src/core/WavReader.h \
           $$PWD/../src/core/File.h

include(../lame_link.pri)
