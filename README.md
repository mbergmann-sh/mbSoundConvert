<p align="center"><img src="docs/logo.png" width="128" alt="mbSoundConvert logo"></p>

# mbSoundConvert

**WAV → MP3 batch converter with built-in player – Qt 6, LAME 4.0, Windows & Linux.**
*(WAV-zu-MP3-Batch-Konverter mit integriertem Player – Qt 6, LAME 4.0, Windows & Linux.)*

> **GitHub "About" (short description):** `Batch-convert WAV files to MP3 (LAME 4.0, default CBR 320 kbit/s). Qt 6 / qmake, drag & drop, built-in player, Windows + Linux installers.`
> **Topics:** `qt6` `qmake` `mp3` `wav` `lame` `audio-converter` `batch-converter` `cpp17` `windows` `linux`

© Michael Bergmann 2026 · [MIT License](LICENSE)

---

## Features / Funktionen

* **WAV → MP3** with the LAME encoder (**LAME 4.0 sources are included**; built as `mp3lame.dll` with MinGW, statically linked elsewhere)
* Add files via **multi-select dialog** *or* **multi drag & drop** (files and whole folders, recursive)
* **Quality presets** (default **CBR 320 kbit/s**): CBR 64–320 kbit/s, VBR V0/V2/V4/V6, plus LAME algorithm quality 0–9
* **Preferences:** source directory, target directory (or "next to the source file"), what to do if the MP3 exists (rename / overwrite / skip), number of parallel conversions
* **Built-in player** for the WAV files: play, pause, stop, seek, volume (also plays the finished MP3 – via context menu)
* Reads PCM 8/16/24/32 bit, IEEE-float 32/64 bit, WAVE_FORMAT_EXTENSIBLE, RF64; mono/stereo (any sample rate – LAME resamples)
* Correct MP3 duration in every player (Xing/Info header), results are written atomically (`.part` → final name)
* Parallel conversion on all CPU cores, progress per file and overall, cancel at any time
* About dialog with copyright, MIT license text and third-party notices
* Icons for Windows (multi-size `.ico`) and Linux (hicolor theme, PNG + SVG)

## Build (qmake6)

Requirements: **Qt 6.5+** (modules *Widgets*, *Multimedia*), a C++17 compiler, `qmake6`.
Nothing else has to be downloaded – LAME is part of this repository (`third_party/lame`).

### Linux
```bash
sudo apt install qmake6 qt6-base-dev qt6-multimedia-dev build-essential   # or use the Qt online installer
scripts/build_linux.sh                 # build + self-test   (binary: build/release/bin/mbSoundConvert)
scripts/build_linux.sh --deploy        # + dist/mbSoundConvert  (program + Qt libs + plugins)
scripts/build_linux.sh --installer     # + Qt Installer Framework setup (needs binarycreator in PATH / IFW_BIN)
```
Manual build / `make install`:
```bash
mkdir build && cd build
qmake6 ../mbSoundConvert.pro CONFIG+=release PREFIX=/usr
make -j$(nproc) && make check && sudo make install
```
`CONFIG+=system_lame` links the system `libmp3lame` instead of the bundled sources; `CONFIG+=lame_shared` builds LAME as `libmp3lame.so` (found via rpath `../lib`).

### Windows (Qt 6 + MinGW)
Install Qt 6 with the **MinGW** kit (and *Qt Installer Framework* for the setup program) via the Qt Maintenance Tool.

**Build step "windeployqt" is part of the normal build.** After every link, qmake runs
`scripts\deploy_windows.bat` (`QMAKE_POST_LINK` in `src/src.pro`). It creates `build\release\deploy` containing

| copied by the step | what |
|---|---|
| `mbSoundConvert.exe` | the program (never object files, `moc_*` or sources) |
| `mp3lame.dll` | the LAME encoder DLL (built from `third_party/lame`, `lamelib/`) |
| `Qt6Core.dll`, `Qt6Gui.dll`, `Qt6Widgets.dll`, `Qt6Multimedia.dll` … + `platforms\`, `multimedia\` … | `windeployqt` (incl. FFmpeg for playback) |
| `libstdc++-6.dll`, `libgcc_s_seh-1.dll`, `libwinpthread-1.dll` | MinGW runtime (taken from `windeployqt`, otherwise from the `g++` directory / `MINGW_BIN`) |
| `LICENSE.txt`, `THIRD_PARTY_LICENSES.txt` | license texts |

The step fails the build if something essential is missing (e.g. `platforms\qwindows.dll`, the LAME DLL or the MinGW runtime).

```bat
:: from the "Qt 6.x (MinGW) command prompt" (or simply "Build" in Qt Creator)
mkdir build && cd build
qmake ..\mbSoundConvert.pro CONFIG+=release
mingw32-make -j8
```
**The normal build now produces everything – no extra script needed:**

| result | location |
|---|---|
| program + `mp3lame.dll` | `build\release\bin\` |
| ready-to-ship folder (exe, Qt/MinGW/LAME DLLs, plugins) – created by `windeployqt` | `build\release\deploy\` |
| **setup program** (Qt Installer Framework) | **`installer\output\mbSoundConvertSetup_IFW.exe`** (Debug builds: `…_IFW_debug.exe`) |

The installer step runs **after** the deploy step succeeded and packs **only** the content of `deploy\` (no source code, no `.o`/`moc_*` files –
it refuses to build otherwise). `binarycreator.exe` is found via the environment variable `IFW_DIR` (e.g. `D:\Qt\Tools\QtInstallerFramework\4.8`),
the `PATH`, or automatically in `<Qt>\Tools\QtInstallerFramework\*\bin`. If it cannot be found, the build still succeeds and prints
`[installer] UEBERSPRUNGEN …` with instructions (in Qt Creator: *Projects → Build → Build Environment → add `IFW_DIR`*).
Packing the installer takes some seconds per link; switch it off with `CONFIG+=no_installer`.

`scripts\build_windows.ps1` runs the same build plus the self-test and additionally creates a portable ZIP and a copy of the setup in `dist\`:
```powershell
.\scripts\build_windows.ps1 -QtDir C:\Qt\6.8.1\mingw_64 -ToolsDir C:\Qt\Tools\mingw1310_64\bin `
                            -IfwDir C:\Qt\Tools\QtInstallerFramework\4.8\bin
```
Useful switches: `-NoInstaller`, `-Clean`, `-StaticLame` (link LAME into the `.exe` instead of using `mp3lame.dll`); qmake switches:
`CONFIG+=no_deploy`, `CONFIG+=no_installer`, `CONFIG+=lame_static`. With MSVC the LAME library is always linked statically.
The scripts can also be run by hand: `scripts\deploy_windows.bat`, `scripts\make_installer_windows.bat`.

## Project layout

```
mbSoundConvert.pro      qmake top-level (subdirs: lamelib, src, tests)
common.pri, lame_link.pri
src/                    application (Qt GUI) – src/core/ = Qt-free WAV reader + LAME wrapper
lamelib/                qmake project + config.h that builds libmp3lame statically
third_party/lame/       LAME 4.0 encoder sources (unmodified, LGPL)
tests/                  self-test of the conversion core (make check)
resources/              icons (PNG/ICO/SVG), Qt resource file
packaging/linux/        .desktop file, hicolor icon theme
installer/              Qt Installer Framework config + package
scripts/                build_windows.ps1, build_linux.sh, deploy_linux.sh
```

## License

mbSoundConvert is released under the **MIT License** – see [LICENSE](LICENSE).
Third-party components (LAME – LGPL, Qt 6 – LGPL v3, FFmpeg via Qt Multimedia) are listed in
[THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md).

---

## Kurzbeschreibung (Deutsch)

mbSoundConvert wandelt WAV-Dateien stapelweise in MP3 um. Dateien lassen sich per Mehrfachauswahl oder per
Drag & Drop (Dateien und Ordner) hinzufügen. Die Qualität ist über Stufen einstellbar (Voreinstellung: CBR 320 kbit/s),
Quell- und Zielverzeichnis legt man in den Voreinstellungen fest. Die WAV-Dateien können im Programm abgespielt,
pausiert und gestoppt werden. Der MP3-Encoder **LAME 4.0** liegt im Projekt (`third_party/lame`) und wird statisch
eingebunden. Gebaut wird mit **qmake6**; unter Windows sorgt `windeployqt` als automatischer Build-Schritt (nach dem Linken) dafür, dass alle
Qt-6-Bibliotheken, die MinGW-Laufzeit und die LAME-DLL (`mp3lame.dll`) im Ordner `build\deploy` liegen – erst danach wird der Installer gebaut (`installer\output\mbSoundConvertSetup_IFW.exe`). Der Installer enthält nur Programm und Bibliotheken – keinen Quellcode und keine
Zwischendateien. Lizenz: **MIT**, © Michael Bergmann 2026.
