// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Michael Bergmann
#include "MainWindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("mbSoundConvert"));
    QCoreApplication::setApplicationName(QStringLiteral("mbSoundConvert"));
    QCoreApplication::setApplicationVersion(QStringLiteral(MBSC_VERSION));
    QGuiApplication::setDesktopFileName(QStringLiteral("mbSoundConvert"));
    QApplication::setWindowIcon(QIcon(QStringLiteral(":/icons/app_256.png")));

    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("WAV → MP3 Batch-Konverter"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(QStringLiteral("dateien"), QObject::tr("WAV-Dateien oder Ordner (optional)"));
    parser.process(app);

    MainWindow w;
    w.show();
    if (!parser.positionalArguments().isEmpty()) w.addPaths(parser.positionalArguments());
    return app.exec();
}
