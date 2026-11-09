// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Michael Bergmann
#include "AboutDialog.h"

#include "core/Converter.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QFile>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTextBrowser>
#include <QVBoxLayout>

namespace {
QString readResource(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return QObject::tr("(Text nicht verfügbar)");
    return QString::fromUtf8(f.readAll());
}

QPlainTextEdit* makeTextView(const QString& text, QWidget* parent) {
    auto* e = new QPlainTextEdit(parent);
    e->setReadOnly(true);
    e->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    e->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    e->setPlainText(text);
    return e;
}
} // namespace

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Über mbSoundConvert"));
    setModal(true);

    // ---- Kopfbereich: Logo + Titel + Copyright + Lizenz ----------------------------
    auto* logo = new QLabel(this);
    logo->setPixmap(QPixmap(QStringLiteral(":/icons/app_256.png"))
                        .scaled(112, 112, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logo->setAlignment(Qt::AlignTop);

    auto* head = new QLabel(this);
    head->setTextFormat(Qt::RichText);
    head->setText(
        tr("<div style='font-size:22pt; font-weight:600;'>mbSoundConvert</div>"
           "<div style='font-size:10pt;'>Version %1 &nbsp;·&nbsp; WAV → MP3 Batch-Konverter</div>"
           "<p style='font-size:11pt; margin-top:10px;'><b>© Michael Bergmann 2026</b></p>"
           "<p style='font-size:10pt;'>Lizenz: <b>MIT-Lizenz</b> (siehe Reiter „MIT-Lizenz“)</p>")
            .arg(QApplication::applicationVersion()));
    head->setTextInteractionFlags(Qt::TextSelectableByMouse);

    auto* headRow = new QHBoxLayout;
    headRow->setSpacing(18);
    headRow->addWidget(logo);
    headRow->addWidget(head, 1);

    // ---- Reiter -------------------------------------------------------------------
    auto* about = new QTextBrowser(this);
    about->setOpenExternalLinks(true);
    about->setHtml(
        tr("<p>mbSoundConvert wandelt WAV-Dateien stapelweise in MP3 um – mit Mehrfachauswahl oder "
           "Drag&nbsp;&amp;&nbsp;Drop, einstellbaren Qualitätsstufen (Voreinstellung: CBR 320&nbsp;kbit/s) "
           "und einem eingebauten Player zum Probehören.</p>"
           "<p><b>Verwendete Komponenten</b></p>"
           "<ul>"
           "<li><b>LAME %1</b> – MP3-Encoder (LGPL), eingebunden als <code>mp3lame.dll</code> bzw. fest im Programm</li>"
           "<li><b>Qt %2</b> – Oberfläche und Audiowiedergabe (LGPL v3)</li>"
           "</ul>"
           "<p><b>Lizenz</b><br>"
           "Dieses Programm ist freie Software unter der <b>MIT-Lizenz</b>: Sie dürfen es frei nutzen, "
           "kopieren, ändern und weitergeben, sofern der Copyright-Hinweis „© Michael Bergmann 2026“ "
           "und der Lizenztext erhalten bleiben. Es wird <b>ohne jede Gewährleistung</b> geliefert.</p>"
           "<p>Die Lizenzen der Drittanbieter-Komponenten finden Sie im gleichnamigen Reiter.</p>")
            .arg(QString::fromStdString(mbsc::lameVersion()), QString::fromLatin1(qVersion())));

    auto* tabs = new QTabWidget(this);
    tabs->addTab(about, tr("Über"));
    tabs->addTab(makeTextView(readResource(QStringLiteral(":/legal/LICENSE.txt")), this), tr("MIT-Lizenz"));
    tabs->addTab(makeTextView(readResource(QStringLiteral(":/legal/THIRD_PARTY_LICENSES.txt")), this),
                 tr("Drittanbieter"));

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
    buttons->button(QDialogButtonBox::Close)->setText(tr("Schließen"));
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(headRow);
    layout->addWidget(tabs, 1);
    layout->addWidget(buttons);

    resize(640, 520);
}
