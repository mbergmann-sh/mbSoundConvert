# mbSoundConvert – Qt-Anwendung
include(../common.pri)

TEMPLATE = app
TARGET = mbSoundConvert
QT += widgets multimedia
VERSION = 1.0.0
DEFINES += MBSC_VERSION=\\\"$$VERSION\\\" QT_DEPRECATED_WARNINGS
DESTDIR = $$OUT_PWD/../bin

INCLUDEPATH += $$PWD

SOURCES += \
    main.cpp \
    AboutDialog.cpp \
    AppSettings.cpp \
    ConvertManager.cpp \
    FileTable.cpp \
    MainWindow.cpp \
    SettingsDialog.cpp \
    core/Converter.cpp \
    core/WavReader.cpp

HEADERS += \
    AboutDialog.h \
    AppSettings.h \
    ConvertManager.h \
    FileTable.h \
    MainWindow.h \
    PathUtil.h \
    SettingsDialog.h \
    core/Converter.h \
    core/File.h \
    core/WavReader.h

RESOURCES += $$PWD/../resources/resources.qrc

include(../lame_link.pri)

msvc: QMAKE_CXXFLAGS += /utf-8

# ------------------------------------------------------------------ Windows
win32 {
    # Programm-Icon (mehrere Größen 16–256 px) + Versionsinformationen der .exe
    RC_ICONS = $$PWD/../resources/icons/app.ico
    QMAKE_TARGET_COMPANY = "Michael Bergmann"
    QMAKE_TARGET_PRODUCT = "mbSoundConvert"
    QMAKE_TARGET_DESCRIPTION = "mbSoundConvert - WAV to MP3 batch converter"
    QMAKE_TARGET_COPYRIGHT = "Copyright (c) 2026 Michael Bergmann (MIT License)"
    RC_LANG = 0x0407
}

# ------------------------------------------------------------------ Linux
unix:!macx {
    # RPATH relativ zur Programmdatei: gebündelte Qt-Bibliotheken in ../lib werden gefunden
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/../lib\'"

    # make install  (PREFIX=/usr  ->  /usr/bin, /usr/share/applications, /usr/share/icons/hicolor)
    isEmpty(PREFIX): PREFIX = /usr/local
    target.path = $$PREFIX/bin
    desktop.path = $$PREFIX/share/applications
    desktop.files = $$PWD/../packaging/linux/mbSoundConvert.desktop
    icons.path = $$PREFIX/share/icons
    icons.files = $$PWD/../packaging/linux/icons/hicolor
    INSTALLS += target desktop icons
}
