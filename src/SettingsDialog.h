// SPDX-License-Identifier: MIT
#pragma once

#include <QDialog>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QSpinBox;

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);

    void accept() override;

private slots:
    void browseSource();
    void browseTarget();
    void restoreDefaults();
    void updateEnabledState();

private:
    void loadFromSettings();

    QLineEdit* sourceEdit_ = nullptr;
    QLineEdit* targetEdit_ = nullptr;
    QCheckBox* sameDirCheck_ = nullptr;
    QComboBox* presetCombo_ = nullptr;
    QSpinBox* algoSpin_ = nullptr;
    QComboBox* overwriteCombo_ = nullptr;
    QSpinBox* jobsSpin_ = nullptr;
};
