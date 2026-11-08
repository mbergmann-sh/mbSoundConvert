# mbSoundConvert – WAV -> MP3 Batch-Konverter (Qt 6 / qmake6)
#
#   mkdir build && cd build
#   qmake6 ../mbSoundConvert.pro CONFIG+=release
#   make            (Windows: mingw32-make / nmake / jom)
#
# Optionen:  CONFIG+=system_lame   systemweite libmp3lame statt der mitgelieferten Quellen
#            CONFIG+=no_tests      Selbsttest nicht mitbauen

TEMPLATE = subdirs
include(common.pri)

SUBDIRS += src
src.subdir = src

!system_lame {
    SUBDIRS += lamelib
    lamelib.subdir = lamelib
    src.depends = lamelib
}

!no_tests {
    SUBDIRS += tests
    tests.subdir = tests
    !system_lame: tests.depends = lamelib
}

OTHER_FILES += README.md LICENSE THIRD_PARTY_LICENSES.md
