#pragma once
#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include "../utils/scheduler.h"

class SettingsPage : public QWidget {
    Q_OBJECT
public:
    explicit SettingsPage(QWidget *parent = nullptr);
private slots:
    void onSave();
    void onReset();
private:
    void setupUI();
    void loadSettings();

    QCheckBox  *m_chkAutoClean;
    QComboBox  *m_cbFrequency;
    QSpinBox   *m_spinJournalDays;
    QCheckBox  *m_chkNotify;
    QCheckBox  *m_chkConfirmClean;
    QCheckBox  *m_chkAutostart;
    QLabel     *m_lblNextRun;
    QPushButton *m_btnSave;
};
