# Gemeinsame Einstellungen aller Teilprojekte
CONFIG += c++17

# Nur ein Build-Typ pro Lauf (einfachere Ausgabeverzeichnisse, auch unter Windows)
win32 {
    CONFIG -= debug_and_release debug_and_release_target
}
!CONFIG(debug, debug|release): CONFIG += release

# ---- LAME: DLL oder statisch? ---------------------------------------------------------
#   Windows + MinGW : Standard = DLL (mp3lame.dll, wird neben die .exe kopiert und mit deployt)
#                     CONFIG+=lame_static  -> LAME fest in die .exe einbinden
#   Windows + MSVC  : immer statisch (lame.def passt nicht zum reinen Encoder-Build)
#   Linux           : Standard = statisch;  CONFIG+=lame_shared -> libmp3lame.so (+ rpath ../lib)
win32-g++*:!lame_static: CONFIG += lame_shared
win32-msvc*: CONFIG -= lame_shared
