<#
.SYNOPSIS
  Baut mbSoundConvert mit qmake6, deployt die Qt6-Bibliotheken mit windeployqt und
  erstellt (optional) den Qt-Installer-Framework-Installer.

.DESCRIPTION
  Ablauf:  qmake6 -> make -> Staging-Ordner dist\mbSoundConvert (nur .exe)
           -> windeployqt (kopiert alle benoetigten Qt6-DLLs/Plugins dorthin)
           -> Pruefung: keine .o/.obj/moc_*/Quelltext im Staging-Ordner
           -> Portable-ZIP -> Installer (binarycreator)

  Der Installer enthaelt ausschliesslich den Inhalt von dist\mbSoundConvert
  (Programm + Bibliotheken + Lizenztexte), niemals Build-Zwischendateien oder Quellcode.

.EXAMPLE
  # MinGW-Kit
  .\scripts\build_windows.ps1 -QtDir C:\Qt\6.8.1\mingw_64 -ToolsDir C:\Qt\Tools\mingw1310_64\bin `
                              -IfwDir C:\Qt\Tools\QtInstallerFramework\4.8\bin
  # MSVC-Kit: in der "x64 Native Tools Command Prompt for VS" starten
  .\scripts\build_windows.ps1 -QtDir C:\Qt\6.8.1\msvc2022_64 -IfwDir C:\Qt\Tools\QtInstallerFramework\4.8\bin
#>
[CmdletBinding()]
param(
    [string]$QtDir = $env:QTDIR,   # Qt-Kit, z. B. C:\Qt\6.8.1\mingw_64
    [string]$ToolsDir = "",        # MinGW-bin-Ordner (nur bei MinGW-Kits noetig, falls nicht im PATH)
    [string]$IfwDir = "",          # Qt Installer Framework bin-Ordner (binarycreator.exe)
    [switch]$NoInstaller,          # nur bauen + deployen
    [switch]$Clean                 # build\ und dist\ vorher loeschen
)

$ErrorActionPreference = "Stop"
$Root    = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$Version = "1.0.0"
$Build   = Join-Path $Root "build\release"
$Dist    = Join-Path $Root "dist"
$Stage   = Join-Path $Dist "mbSoundConvert"
$PkgData = Join-Path $Root "installer\packages\de.mbsoundconvert.app\data"

function Step($t) { Write-Host "`n=== $t ===" -ForegroundColor Cyan }
function Run([string]$exe, [string[]]$a) {
    & $exe @a
    if ($LASTEXITCODE -ne 0) { throw "Fehler ($LASTEXITCODE): $exe $($a -join ' ')" }
}
function Find-Tool([string[]]$names) {
    foreach ($n in $names) { $c = Get-Command $n -ErrorAction SilentlyContinue; if ($c) { return $c.Source } }
    return $null
}

# ---------------------------------------------------------------- Umgebung
if ($QtDir)    { $env:PATH = (Join-Path $QtDir "bin") + ";" + $env:PATH }
if ($ToolsDir) { $env:PATH = $ToolsDir + ";" + $env:PATH }
if ($IfwDir)   { $env:PATH = $IfwDir + ";" + $env:PATH }

$qmake = Find-Tool @("qmake6", "qmake")
if (-not $qmake) { throw "qmake6 nicht gefunden. Bitte -QtDir angeben (z. B. C:\Qt\6.8.1\mingw_64)." }
$qtVer = (& $qmake -query QT_VERSION).Trim()
if ([int]($qtVer.Split('.')[0]) -lt 6) { throw "Qt 6 erforderlich, gefunden: $qtVer ($qmake)" }
$spec = (& $qmake -query QMAKE_XSPEC).Trim()
Write-Host "qmake: $qmake  (Qt $qtVer, Spec $spec)"

if ($spec -match "msvc") {
    $make = Find-Tool @("jom", "nmake")
    $makeArgs = @()
} else {
    $make = Find-Tool @("mingw32-make", "make")
    $makeArgs = @("-j$env:NUMBER_OF_PROCESSORS")
}
if (-not $make) { throw "Kein make-Programm gefunden (mingw32-make / jom / nmake). MSVC: 'Native Tools Command Prompt' benutzen; MinGW: -ToolsDir angeben." }

$deploy = Find-Tool @("windeployqt6", "windeployqt")
if (-not $deploy) { throw "windeployqt nicht gefunden (liegt in <Qt-Kit>\bin)." }

if ($Clean) {
    Step "Aufraeumen"
    Remove-Item -Recurse -Force $Build, $Dist -ErrorAction SilentlyContinue
}

# ---------------------------------------------------------------- Bauen
Step "qmake"
New-Item -ItemType Directory -Force $Build | Out-Null
Push-Location $Build
try {
    Run $qmake @((Join-Path $Root "mbSoundConvert.pro"), "CONFIG+=release")
    Step "Kompilieren ($make)"
    Run $make $makeArgs
} finally { Pop-Location }

$exe = Join-Path $Build "bin\mbSoundConvert.exe"
if (-not (Test-Path $exe)) { throw "Build fehlgeschlagen: $exe nicht gefunden." }

Step "Selbsttest (WAV -> MP3 mit LAME)"
$test = Join-Path $Build "bin\test_converter.exe"
if (Test-Path $test) { Run $test @() } else { Write-Warning "test_converter.exe nicht gebaut - uebersprungen." }

# ---------------------------------------------------------------- Staging + windeployqt
Step "Staging-Ordner anlegen: $Stage"
Remove-Item -Recurse -Force $Stage -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force $Stage | Out-Null
Copy-Item $exe $Stage                                   # nur die fertige .exe - keine .o/moc-Dateien
Copy-Item (Join-Path $Root "LICENSE") (Join-Path $Stage "LICENSE.txt")
Copy-Item (Join-Path $Root "THIRD_PARTY_LICENSES.md") (Join-Path $Stage "THIRD_PARTY_LICENSES.txt")

Step "windeployqt: Qt6-Bibliotheken und Plugins kopieren"
Run $deploy @("--release", "--no-translations", "--no-system-d3d-compiler", "--no-opengl-sw",
              (Join-Path $Stage "mbSoundConvert.exe"))

# ---------------------------------------------------------------- Sicherheitspruefung
Step "Pruefe Staging-Ordner (keine Build-Zwischendateien / kein Quellcode)"
$forbiddenExt  = @(".o", ".obj", ".a", ".lib", ".cpp", ".c", ".h", ".hpp", ".pro", ".pri", ".qrc", ".moc", ".pdb", ".ilk", ".exp")
$forbidden = Get-ChildItem $Stage -Recurse -File | Where-Object {
    $forbiddenExt -contains $_.Extension.ToLower() -or $_.Name -like "moc_*" -or $_.Name -like "qrc_*" -or $_.Name -like "ui_*"
}
if ($forbidden) {
    $forbidden | ForEach-Object { Write-Host "  unerwuenscht: $($_.FullName)" -ForegroundColor Red }
    throw "Unerwuenschte Dateien im Staging-Ordner - Abbruch."
}
$files = Get-ChildItem $Stage -Recurse -File
"{0} Dateien, {1:N1} MB - OK" -f $files.Count, (($files | Measure-Object Length -Sum).Sum / 1MB) | Write-Host

# ---------------------------------------------------------------- Portable ZIP
Step "Portable-ZIP"
$zip = Join-Path $Dist "mbSoundConvert-$Version-windows-portable.zip"
Remove-Item -Force $zip -ErrorAction SilentlyContinue
Compress-Archive -Path (Join-Path $Stage "*") -DestinationPath $zip
Write-Host $zip

# ---------------------------------------------------------------- Installer
if ($NoInstaller) { Write-Host "`nFertig (ohne Installer)." -ForegroundColor Green; return }

$bc = Find-Tool @("binarycreator")
if (-not $bc) { throw "binarycreator.exe nicht gefunden (Qt Installer Framework). Bitte -IfwDir angeben, z. B. C:\Qt\Tools\QtInstallerFramework\4.8\bin" }

Step "Installer-Paket befuellen (nur Programm + Bibliotheken)"
New-Item -ItemType Directory -Force $PkgData | Out-Null
Get-ChildItem $PkgData -Force | Where-Object { $_.Name -ne ".gitkeep" } | Remove-Item -Recurse -Force
Copy-Item (Join-Path $Stage "*") $PkgData -Recurse

Step "binarycreator"
$setup = Join-Path $Dist "mbSoundConvert-$Version-windows-setup.exe"
Run $bc @("--offline-only", "-c", (Join-Path $Root "installer\config\config.xml"),
          "-p", (Join-Path $Root "installer\packages"), $setup)
Write-Host "`nInstaller: $setup" -ForegroundColor Green
