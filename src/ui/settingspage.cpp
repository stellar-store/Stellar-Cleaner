#include "settingspage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QSettings>
#include <QMessageBox>

SettingsPage::SettingsPage(QWidget *parent) : QWidget(parent) {
    setupUI();
    loadSettings();
}

void SettingsPage::setupUI() {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);

    auto *tb = new QWidget(this);
    tb->setObjectName("pageHeader");
    tb->setFixedHeight(56);
    auto *tbLay = new QHBoxLayout(tb);
    tbLay->setContentsMargins(20,8,20,8);
    auto *ttl = new QLabel("Configuración", tb);
    ttl->setObjectName("pageTitle");
    tbLay->addWidget(ttl);
    tbLay->addStretch();
    m_btnSave = new QPushButton("💾  Guardar", tb);
    m_btnSave->setObjectName("btnPrimary");
    tbLay->addWidget(m_btnSave);
    auto *btnReset = new QPushButton("↩  Restablecer", tb);
    btnReset->setObjectName("btnSecondary");
    tbLay->addWidget(btnReset);
    root->addWidget(tb);

    auto *scroll = new QWidget(this);
    auto *sLay = new QVBoxLayout(scroll);
    sLay->setContentsMargins(24,20,24,20);
    sLay->setSpacing(16);

    // Scheduler group
    auto *schedGroup = new QGroupBox("PROGRAMACIÓN AUTOMÁTICA", scroll);
    auto *schedLay = new QFormLayout(schedGroup);
    schedLay->setSpacing(12);

    m_chkAutoClean = new QCheckBox("Habilitar limpieza automática programada", schedGroup);
    schedLay->addRow("", m_chkAutoClean);

    m_cbFrequency = new QComboBox(schedGroup);
    m_cbFrequency->addItems({"Diariamente","Semanalmente","Mensualmente","Al Inicio del Sistema"});
    m_cbFrequency->setCurrentIndex(1);
    schedLay->addRow("Frecuencia:", m_cbFrequency);

    m_lblNextRun = new QLabel("—", schedGroup);
    m_lblNextRun->setObjectName("accent");
    schedLay->addRow("Próxima ejecución:", m_lblNextRun);

    sLay->addWidget(schedGroup);

    // Logs group
    auto *logGroup = new QGroupBox("LOGS Y JOURNAL", scroll);
    auto *logLay = new QFormLayout(logGroup);
    logLay->setSpacing(12);

    m_spinJournalDays = new QSpinBox(logGroup);
    m_spinJournalDays->setRange(1, 365);
    m_spinJournalDays->setValue(30);
    m_spinJournalDays->setSuffix(" días");
    logLay->addRow("Conservar journal de los últimos:", m_spinJournalDays);
    sLay->addWidget(logGroup);

    // Behavior group
    auto *behavGroup = new QGroupBox("COMPORTAMIENTO", scroll);
    auto *behavLay = new QVBoxLayout(behavGroup);
    behavLay->setSpacing(8);

    m_chkNotify      = new QCheckBox("Mostrar notificaciones del sistema después de limpiar", behavGroup);
    m_chkConfirmClean= new QCheckBox("Pedir confirmación antes de limpiar", behavGroup);
    m_chkAutostart   = new QCheckBox("Iniciar Stellar Cleaner con el sistema", behavGroup);
    m_chkConfirmClean->setChecked(true);
    behavLay->addWidget(m_chkNotify);
    behavLay->addWidget(m_chkConfirmClean);
    behavLay->addWidget(m_chkAutostart);
    sLay->addWidget(behavGroup);

    // About group
    auto *aboutGroup = new QGroupBox("ACERCA DE", scroll);
    auto *aboutLay = new QVBoxLayout(aboutGroup);
    auto *aboutLbl = new QLabel(
        "<b>Stellar Cleaner v1.0.0</b><br>"
        "Limpiador de sistema profesional para Linux<br><br>"
        "Desarrollado en C++17 con Qt6<br>"
        "Compatible con Ubuntu, Debian, Fedora, Arch Linux y más<br><br>"
        "<small style='color:#4a5568'>Código abierto — Sin telemetría — Sin publicidad</small>",
        aboutGroup);
    aboutLbl->setWordWrap(true);
    aboutLay->addWidget(aboutLbl);
    sLay->addWidget(aboutGroup);
    sLay->addStretch();

    root->addWidget(scroll, 1);

    connect(m_btnSave, &QPushButton::clicked, this, &SettingsPage::onSave);
    connect(btnReset,  &QPushButton::clicked, this, &SettingsPage::onReset);
    connect(m_chkAutoClean, &QCheckBox::toggled, m_cbFrequency, &QComboBox::setEnabled);
    m_cbFrequency->setEnabled(false);
}

void SettingsPage::loadSettings() {
    QSettings s("Stellar Cleaner","Settings");
    m_chkAutoClean->setChecked(s.value("autoClean", false).toBool());
    m_cbFrequency->setCurrentIndex(s.value("frequency", 1).toInt());
    m_spinJournalDays->setValue(s.value("journalDays", 30).toInt());
    m_chkNotify->setChecked(s.value("notify", true).toBool());
    m_chkConfirmClean->setChecked(s.value("confirmClean", true).toBool());
    m_chkAutostart->setChecked(s.value("autostart", false).toBool());

    auto &sched = Scheduler::instance();
    if (sched.isEnabled()) {
        m_lblNextRun->setText(sched.nextRun().toString("dd/MM/yyyy hh:mm"));
    } else {
        m_lblNextRun->setText("Deshabilitado");
    }
}

void SettingsPage::onSave() {
    QSettings s("Stellar Cleaner","Settings");
    s.setValue("autoClean",    m_chkAutoClean->isChecked());
    s.setValue("frequency",    m_cbFrequency->currentIndex());
    s.setValue("journalDays",  m_spinJournalDays->value());
    s.setValue("notify",       m_chkNotify->isChecked());
    s.setValue("confirmClean", m_chkConfirmClean->isChecked());
    s.setValue("autostart",    m_chkAutostart->isChecked());

    auto &sched = Scheduler::instance();
    sched.setEnabled(m_chkAutoClean->isChecked());
    sched.setFrequency((Scheduler::Frequency)m_cbFrequency->currentIndex());

    QMessageBox::information(this,"Guardado","Configuración guardada correctamente.");
}

void SettingsPage::onReset() {
    if (QMessageBox::question(this, "Restablecer", "¿Restablecer toda la configuración?",
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
    QSettings("Stellar Cleaner","Settings").clear();
    loadSettings();
}
