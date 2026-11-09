# Bindet libmp3lame in ein Teilprojekt ein (Header + Bibliothek).
#
#   Standard:            mitgelieferte LAME-4.0-Quellen (third_party/lame), gebaut vom Teilprojekt
#                        lamelib/ (liegt in <build>/lib; als DLL siehe common.pri)
#   CONFIG+=system_lame: systemweite libmp3lame (Linux: libmp3lame-dev)

system_lame {
    LIBS += -lmp3lame
} else {
    INCLUDEPATH += $$PWD/third_party/lame/include
    LIBS += -L$$OUT_PWD/../lib -lmp3lame
}
unix: LIBS += -lm
