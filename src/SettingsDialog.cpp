// SPDX-License-Identifier: MIT
#include "SettingsDialog.h"

#include "AppSettings.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QThread>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Voreinstellungen"));
    setModal(true);

    // ---- Verzeichnisse -----------------------------------------------------
    auto* pathsBox = new QGroupBox(tr("Verzeichnisse"), this);
    auto* pathsForm = new QFormLayout(pathsBox);

    sourceEdit_ = new QLineEdit(pathsBox);
    auto* sourceBtn = new QPushButton(QIcon(QStringLiteral(":/icons/folder.png")), tr("Durchsuchen …"), pathsBox);
    auto* sourceRow = new QHBoxLayout;
    sourceRow->addWidget(sourceEdit_, 1);
    sourceRow->addWidget(sourceBtn);
    pathsForm->addRow(tr("Quellverzeichnis:"), sourceRow);

    sameDirCheck_ = new QCheckBox(tr("MP3 im Ordner der jeweiligen Quelldatei speichern"), pathsBox);
    pathsForm->addRow(QString(), sameDirCheck_);

    targetEdit_ = new QLineEdit(pathsBox);
    auto* targetBtn = new QPushButton(QIcon(QStringLiteral(":/icons/folder.png")), tr("Durchsuchen …"), pathsBox);
    auto* targetRow = new QHBoxLayout;
    targetRow->addWidget(targetEdit_, 1);
    targetRow->addWidget(targetBtn);
    pathsForm->addRow(tr("Zielverzeichnis:"), targetRow);

    auto* pathsHint = new QLabel(tr("Das Quellverzeichnis ist der Startordner des Datei-Dialogs. "
                                    "Fehlende Zielordner werden automatisch angelegt."), pathsBox);
    pathsHint->setWordWrap(true);
    pathsHint->setEnabled(false);
    pathsForm->addRow(pathsHint);

    // ---- Kodierung ---------------------------------------------------------
    auto* encBox = new QGroupBox(tr("MP3-Kodierung (LAME)"), this);
    auto* encForm = new QFormLayout(encBox);

    presetCombo_ = new QComboBox(encBox);
    for (const auto& p : qualityPresets()) presetCombo_->addItem(p.label, p.id);
    presetCombo_->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    encForm->addRow(tr("Qualitätsstufe:"), presetCombo_);

    algoSpin_ = new QSpinBox(encBox);
    algoSpin_->setRange(0, 9);
    algoSpin_->setToolTip(tr("0 = beste Qualität, langsam · 2 = LAME-Empfehlung · 9 = schnell, geringere Qualität"));
    encForm->addRow(tr("Encoder-Algorithmus (0–9):"), algoSpin_);

    // ---- Verarbeitung ------------------------------------------------------
    auto* procBox = new QGroupBox(tr("Verarbeitung"), this);
    auto* procForm = new QFormLayout(procBox);

    overwriteCombo_ = new QComboBox(procBox);
    overwriteCombo_->addItem(tr("Neuen Namen vergeben (name_1.mp3)"), int(OverwritePolicy::Rename));
    overwriteCombo_->addItem(tr("Vorhandene Datei überschreiben"), int(OverwritePolicy::Overwrite));
    overwriteCombo_->addItem(tr("Datei überspringen"), int(OverwritePolicy::Skip));
    procForm->addRow(tr("Wenn die MP3 schon existiert:"), overwriteCombo_);

    jobsSpin_ = new QSpinBox(procBox);
    jobsSpin_->setRange(1, qMax(1, QThread::idealThreadCount()));
    jobsSpin_->setToolTip(tr("Anzahl gleichzeitig konvertierter Dateien"));
    procForm->addRow(tr("Parallele Konvertierungen:"), jobsSpin_);

    // ---- Buttons -----------------------------------------------------------
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults, this);
    buttons->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Abbrechen"));
    buttons->button(QDialogButtonBox::RestoreDefaults)->setText(tr("Standard"));

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(pathsBox);
    layout->addWidget(encBox);
    layout->addWidget(procBox);
    layout->addWidget(buttons);

    connect(sourceBtn, &QPushButton::clicked, this, &SettingsDialog::browseSource);
    connect(targetBtn, &QPushButton::clicked, this, &SettingsDialog::browseTarget);
    connect(sameDirCheck_, &QCheckBox::toggled, this, &SettingsDialog::updateEnabledState);
    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);
    connect(buttons->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &SettingsDialog::restoreDefaults);

    loadFromSettings();
    resize(620, sizeHint().height());
}

void SettingsDialog::loadFromSettings() {
    sourceEdit_->setText(QDir::toNativeSeparators(AppSettings::sourceDir()));
    targetEdit_->setText(QDir::toNativeSeparators(AppSettings::targetDir()));
    sameDirCheck_->setChecked(AppSettings::targetSameAsSource());
    presetCombo_->setCurrentIndex(qMax(0, presetCombo_->findData(AppSettings::presetId())));
    algoSpin_->setValue(AppSettings::algorithmQuality());
    overwriteCombo_->setCurrentIndex(qMax(0, overwriteCombo_->findData(int(AppSettings::overwritePolicy()))));
    jobsSpin_->setValue(AppSettings::parallelJobs());
    updateEnabledState();
}

void SettingsDialog::updateEnabledState() { targetEdit_->setEnabled(!sameDirCheck_->isChecked()); }

void SettingsDialog::browseSource() {
    const QString d = QFileDialog::getExistingDirectory(this, tr("Quellverzeichnis wählen"), sourceEdit_->text());
    if (!d.isEmpty()) sourceEdit_->setText(QDir::toNativeSeparators(d));
}

void SettingsDialog::browseTarget() {
    const QString d = QFileDialog::getExistingDirectory(this, tr("Zielverzeichnis wählen"), targetEdit_->text());
    if (!d.isEmpty()) targetEdit_->setText(QDir::toNativeSeparators(d));
}

void SettingsDialog::restoreDefaults() {
    sourceEdit_->setText(QDir::toNativeSeparators(AppSettings::defaultSourceDir()));
    targetEdit_->setText(QDir::toNativeSeparators(AppSettings::defaultTargetDir()));
    sameDirCheck_->setChecked(false);
    presetCombo_->setCurrentIndex(qMax(0, presetCombo_->findData(defaultPresetId())));
    algoSpin_->setValue(2);
    overwriteCombo_->setCurrentIndex(qMax(0, overwriteCombo_->findData(int(OverwritePolicy::Rename))));
    jobsSpin_->setValue(AppSettings::defaultParallelJobs());
}

void SettingsDialog::accept() {
    const QString src = QDir::fromNativeSeparators(sourceEdit_->text().trimmed());
    const QString dst = QDir::fromNativeSeparators(targetEdit_->text().trimmed());

    if (!src.isEmpty() && !QDir(src).exists()) {
        QMessageBox::warning(this, tr("Quellverzeichnis"), tr("Das Quellverzeichnis existiert nicht:\n%1").arg(src));
        return;
    }
    if (!sameDirCheck_->isChecked()) {
        if (dst.isEmpty() || !QDir().mkpath(dst)) {
            QMessageBox::warning(this, tr("Zielverzeichnis"),
                                 tr("Das Zielverzeichnis kann nicht angelegt werden:\n%1").arg(dst));
            return;
        }
    }

    AppSettings::setSourceDir(src);
    if (!dst.isEmpty()) AppSettings::setTargetDir(dst);
    AppSettings::setTargetSameAsSource(sameDirCheck_->isChecked());
    AppSettings::setPresetId(presetCombo_->currentData().toString());
    AppSettings::setAlgorithmQuality(algoSpin_->value());
    AppSettings::setOverwritePolicy(static_cast<OverwritePolicy>(overwriteCombo_->currentData().toInt()));
    AppSettings::setParallelJobs(jobsSpin_->value());
    QDialog::accept();
}
