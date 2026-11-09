// SPDX-License-Identifier: MIT
#pragma once

#include "core/Converter.h"

#include <QByteArray>
#include <QList>
#include <QString>

// Eine auswählbare Qualitätsstufe (CBR-Bitrate oder VBR-Qualität).
struct QualityPreset {
    QString id;
    QString label;
    mbsc::RateMode mode;
    int value;  // CBR: kbit/s – VBR: Qualität 0 (beste) … 9
};

const QList<QualityPreset>& qualityPresets();
QualityPreset presetById(const QString& id);   // Kopie; unbekannte ID -> Voreinstellung (CBR 320)
QString defaultPresetId();

enum class OverwritePolicy { Rename = 0, Overwrite = 1, Skip = 2 };

// Persistente Voreinstellungen (QSettings). Alle Funktionen sind thread-agnostisch,
// werden aber nur aus dem GUI-Thread aufgerufen.
namespace AppSettings {

QString defaultSourceDir();
QString defaultTargetDir();

QString sourceDir();
void setSourceDir(const QString& dir);

QString targetDir();
void setTargetDir(const QString& dir);

bool targetSameAsSource();
void setTargetSameAsSource(bool on);

QString presetId();
void setPresetId(const QString& id);

int algorithmQuality();            // LAME -q 0…9, Standard 2
void setAlgorithmQuality(int q);

int parallelJobs();                // Standard: halbe Kernanzahl (1…4)
void setParallelJobs(int n);
int defaultParallelJobs();

OverwritePolicy overwritePolicy(); // Standard: umbenennen
void setOverwritePolicy(OverwritePolicy p);

int volume();                      // 0…100
void setVolume(int v);

QByteArray windowGeometry();
void setWindowGeometry(const QByteArray& g);

void resetToDefaults();            // Verzeichnisse, Qualität, Verarbeitung (nicht Fenster/Lautstärke)

} // namespace AppSettings
