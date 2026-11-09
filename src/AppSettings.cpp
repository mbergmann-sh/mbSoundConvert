// SPDX-License-Identifier: MIT
#include "AppSettings.h"

#include <QDir>
#include <QObject>
#include <QSettings>
#include <QStandardPaths>
#include <QThread>

#include <algorithm>

using mbsc::RateMode;

const QList<QualityPreset>& qualityPresets() {
    static const QList<QualityPreset> presets = {
        {QStringLiteral("cbr320"), QObject::tr("CBR 320 kbit/s – maximale Qualität (Voreinstellung)"), RateMode::Cbr, 320},
        {QStringLiteral("cbr256"), QObject::tr("CBR 256 kbit/s"), RateMode::Cbr, 256},
        {QStringLiteral("cbr192"), QObject::tr("CBR 192 kbit/s"), RateMode::Cbr, 192},
        {QStringLiteral("cbr160"), QObject::tr("CBR 160 kbit/s"), RateMode::Cbr, 160},
        {QStringLiteral("cbr128"), QObject::tr("CBR 128 kbit/s – Standard-Qualität"), RateMode::Cbr, 128},
        {QStringLiteral("cbr96"),  QObject::tr("CBR 96 kbit/s"), RateMode::Cbr, 96},
        {QStringLiteral("cbr64"),  QObject::tr("CBR 64 kbit/s – Sprache"), RateMode::Cbr, 64},
        {QStringLiteral("vbr0"),   QObject::tr("VBR V0 – ca. 245 kbit/s, beste variable Qualität"), RateMode::Vbr, 0},
        {QStringLiteral("vbr2"),   QObject::tr("VBR V2 – ca. 190 kbit/s, sehr gut"), RateMode::Vbr, 2},
        {QStringLiteral("vbr4"),   QObject::tr("VBR V4 – ca. 165 kbit/s, gut"), RateMode::Vbr, 4},
        {QStringLiteral("vbr6"),   QObject::tr("VBR V6 – ca. 115 kbit/s, kompakt"), RateMode::Vbr, 6},
    };
    return presets;
}

QString defaultPresetId() { return QStringLiteral("cbr320"); }

QualityPreset presetById(const QString& id) {
    const auto& list = qualityPresets();
    for (const auto& p : list)
        if (p.id == id) return p;
    return list.first();
}

namespace AppSettings {

namespace {
QString musicDir() {
    QString d = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    if (d.isEmpty()) d = QDir::homePath();
    return d;
}
} // namespace

QString defaultSourceDir() { return musicDir(); }
QString defaultTargetDir() { return QDir(musicDir()).filePath(QStringLiteral("mbSoundConvert")); }

QString sourceDir() {
    QSettings s;
    const QString d = s.value(QStringLiteral("paths/source")).toString();
    return d.isEmpty() ? defaultSourceDir() : d;
}
void setSourceDir(const QString& dir) { QSettings().setValue(QStringLiteral("paths/source"), dir); }

QString targetDir() {
    QSettings s;
    const QString d = s.value(QStringLiteral("paths/target")).toString();
    return d.isEmpty() ? defaultTargetDir() : d;
}
void setTargetDir(const QString& dir) { QSettings().setValue(QStringLiteral("paths/target"), dir); }

bool targetSameAsSource() { return QSettings().value(QStringLiteral("paths/targetSameAsSource"), false).toBool(); }
void setTargetSameAsSource(bool on) { QSettings().setValue(QStringLiteral("paths/targetSameAsSource"), on); }

QString presetId() {
    const QString id = QSettings().value(QStringLiteral("encoder/preset"), defaultPresetId()).toString();
    return presetById(id).id;
}
void setPresetId(const QString& id) { QSettings().setValue(QStringLiteral("encoder/preset"), id); }

int algorithmQuality() { return std::clamp(QSettings().value(QStringLiteral("encoder/algorithm"), 2).toInt(), 0, 9); }
void setAlgorithmQuality(int q) { QSettings().setValue(QStringLiteral("encoder/algorithm"), std::clamp(q, 0, 9)); }

int defaultParallelJobs() { return std::clamp(QThread::idealThreadCount() / 2, 1, 4); }
int parallelJobs() {
    const int maxJobs = std::max(1, QThread::idealThreadCount());
    return std::clamp(QSettings().value(QStringLiteral("processing/parallel"), defaultParallelJobs()).toInt(), 1, maxJobs);
}
void setParallelJobs(int n) { QSettings().setValue(QStringLiteral("processing/parallel"), std::max(1, n)); }

OverwritePolicy overwritePolicy() {
    const int v = QSettings().value(QStringLiteral("processing/overwrite"), int(OverwritePolicy::Rename)).toInt();
    return (v >= 0 && v <= 2) ? static_cast<OverwritePolicy>(v) : OverwritePolicy::Rename;
}
void setOverwritePolicy(OverwritePolicy p) { QSettings().setValue(QStringLiteral("processing/overwrite"), int(p)); }

int volume() { return std::clamp(QSettings().value(QStringLiteral("player/volume"), 80).toInt(), 0, 100); }
void setVolume(int v) { QSettings().setValue(QStringLiteral("player/volume"), std::clamp(v, 0, 100)); }

QByteArray windowGeometry() { return QSettings().value(QStringLiteral("window/geometry")).toByteArray(); }
void setWindowGeometry(const QByteArray& g) { QSettings().setValue(QStringLiteral("window/geometry"), g); }

void resetToDefaults() {
    QSettings s;
    s.remove(QStringLiteral("paths"));
    s.remove(QStringLiteral("encoder"));
    s.remove(QStringLiteral("processing"));
}

} // namespace AppSettings
