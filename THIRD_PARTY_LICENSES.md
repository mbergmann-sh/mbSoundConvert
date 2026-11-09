# Third-party components / Drittanbieter-Komponenten

mbSoundConvert itself is released under the **MIT License** (see `LICENSE`,
© Michael Bergmann 2026). It uses the following third-party software, which is
licensed separately:

## LAME 4.0 – MP3 encoder library (libmp3lame)
* Location: `third_party/lame/` (unmodified upstream sources, only the files
  needed for the encoder; the autotools/NASM/frontend parts were left out).
  The only project-specific file is `lamelib/config/config.h`, which replaces
  the output of `./configure`.
* License: GNU Library General Public License (LGPL) version 2 or later – full
  text in `third_party/lame/COPYING`, usage notes in `third_party/lame/LICENSE`.
* **Windows/MinGW builds use LAME as a separate DLL (`mp3lame.dll`)**, which can be replaced by the user
  with a modified build of the same sources (`qmake6 CONFIG+=release` rebuilds it) – this satisfies the LGPL.
  MSVC builds and `CONFIG+=lame_static` link LAME statically instead; since the complete source code of
  mbSoundConvert is public (MIT), the program can be relinked with a modified LAME. If you redistribute a
  closed-source derivative, ship LAME as DLL/shared library (default on MinGW, `CONFIG+=lame_shared` on Linux)
  or provide the object files.
* Note: LAME may be covered by software patents in some jurisdictions; the
  relevant MP3 patents have expired in most countries. See `third_party/lame/README`.

## Qt 6 (Widgets, Multimedia, Core, Gui)
* License: LGPL v3 (open-source edition) – https://www.qt.io/licensing
* Qt is used as shared libraries / plugins; the installer ships the unmodified
  Qt libraries deployed by `windeployqt` (Windows) or `scripts/deploy_linux.sh`.
  You may replace them with your own build of the same Qt version.
* Qt Multimedia bundles **FFmpeg** (LGPL v2.1+) for audio playback.

## Qt Installer Framework (build time only)
* Used to create the setup program; not part of the installed application.
