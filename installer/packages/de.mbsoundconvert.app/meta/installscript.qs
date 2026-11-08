// Verknuepfungen / Desktop-Eintrag fuer mbSoundConvert (Windows und Linux)

function Component() {
    var windows = (systemInfo.kernelType === "winnt");
    // "Programm jetzt starten" auf der Abschlussseite
    installer.setValue("RunProgram", windows ? "@TargetDir@/mbSoundConvert.exe"
                                             : "@TargetDir@/bin/mbSoundConvert");
    installer.setValue("RunProgramDescription", "mbSoundConvert jetzt starten");
}

Component.prototype.createOperations = function () {
    component.createOperations();   // kopiert die Dateien aus data/

    if (systemInfo.kernelType === "winnt") {
        var exe = "@TargetDir@/mbSoundConvert.exe";
        component.addOperation("CreateShortcut", exe, "@StartMenuDir@/mbSoundConvert.lnk",
            "workingDirectory=@TargetDir@", "iconPath=" + exe, "iconId=0",
            "description=WAV-zu-MP3 Batch-Konverter");
        component.addOperation("CreateShortcut", exe, "@DesktopDir@/mbSoundConvert.lnk",
            "workingDirectory=@TargetDir@", "iconPath=" + exe, "iconId=0",
            "description=WAV-zu-MP3 Batch-Konverter");
    } else if (systemInfo.kernelType === "linux") {
        component.addOperation("CreateDesktopEntry", "mbSoundConvert.desktop",
            "Version=1.0\n" +
            "Type=Application\n" +
            "Name=mbSoundConvert\n" +
            "GenericName=WAV to MP3 Converter\n" +
            "GenericName[de]=WAV-zu-MP3-Konverter\n" +
            "Comment=Batch-convert WAV files to MP3 with LAME\n" +
            "Exec=@TargetDir@/bin/mbSoundConvert %F\n" +
            "Icon=@TargetDir@/share/icons/hicolor/256x256/apps/mbSoundConvert.png\n" +
            "Terminal=false\n" +
            "Categories=AudioVideo;Audio;AudioVideoEditing;\n" +
            "MimeType=audio/x-wav;audio/wav;audio/vnd.wave;\n" +
            "StartupWMClass=mbSoundConvert");
    }
};
