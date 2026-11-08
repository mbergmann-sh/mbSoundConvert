<p align="center"><img src="docs/logo.png" width="128" alt="mbSoundConvert logo"></p>

# mbSoundConvert

**WAV → MP3 batch converter with built-in player – Qt 6, LAME 4.0, Windows & Linux.**
*(WAV-zu-MP3-Batch-Konverter mit integriertem Player – Qt 6, LAME 4.0, Windows & Linux.)*

> **GitHub "About" (short description):** `Batch-convert WAV files to MP3 (LAME 4.0, default CBR 320 kbit/s). Qt 6 / qmake, drag & drop, built-in player, Windows + Linux installers.`
> **Topics:** `qt6` `qmake` `mp3` `wav` `lame` `audio-converter` `batch-converter` `cpp17` `windows` `linux`

© Michael Bergmann 2026 · [MIT License](LICENSE)

---

## Features / Funktionen

* **WAV → MP3** with the LAME encoder (**LAME 4.0 sources are included** and statically linked)
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
`CONFIG+=system_lame` links the system `libmp3lame` instead of the bundled sources.

### Windows
Install Qt 6 (MinGW or MSVC kit) and, for the installer, the *Qt Installer Framework* via the Qt Maintenance Tool.
```powershell
.\scripts\build_windows.ps1 -QtDir C:\Qt\6.8.1\mingw_64 -ToolsDir C:\Qt\Tools\mingw1310_64\bin `
                            -IfwDir C:\Qt\Tools\QtInstallerFramework\4.8\bin
```
The script runs **qmake → make → self-test → `windeployqt`** into `dist\mbSoundConvert` and only **then** builds
`dist\mbSoundConvert-1.0.0-windows-setup.exe` (plus a portable ZIP). It aborts if object files, `moc_*` files or
source code ended up in the package – the installer contains **only the executable, the Qt/FFmpeg libraries, plugins and license texts**.
Use `-NoInstaller` to stop after the deploy step.

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
eingebunden. Gebaut wird mit **qmake6**; unter Windows sorgt `windeployqt` vor dem Bau des Installers dafür, dass alle
Qt-6-Bibliotheken vorhanden sind. Der Installer enthält nur Programm und Bibliotheken – keinen Quellcode und keine
Zwischendateien. Lizenz: **MIT**, © Michael Bergmann 2026.
