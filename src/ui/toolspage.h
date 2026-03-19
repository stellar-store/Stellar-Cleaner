#pragma once
#include <QWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include "../tools/startupmanager.h"
#include "../tools/processmanager.h"
#include "../tools/uninstallmanager.h"

class ToolsPage : public QWidget {
    Q_OBJECT
public:
    explicit ToolsPage(QWidget *parent = nullptr);

private slots:
    void onStartupTabShow();
    void onProcessTabShow();
    void onUninstallTabShow();
    void onProcessRefresh();
    void onProcessKill();
    void onStartupToggle();
    void onStartupRemove();
    void onUninstall();
    void onUninstallSearch(const QString &text);

private:
    void setupUI();
    void setupStartupTab(QWidget *parent);
    void setupProcessTab(QWidget *parent);
    void setupUninstallTab(QWidget *parent);
    void populateStartup();
    void populateProcesses();
    void populateApps(const QString &filter = "");

    QTabWidget     *m_tabs;
    QTableWidget   *m_procTable;
    QTableWidget   *m_appTable;
    QTreeWidget    *m_startupTree;
    QPushButton    *m_btnKill;
    QPushButton    *m_btnStartupToggle;
    QPushButton    *m_btnUninstall;
    QLabel         *m_lblProcStatus;
    QLineEdit      *m_searchApps;
    QTimer         *m_procTimer;

    StartupManager  *m_startupMgr;
    ProcessManager  *m_procMgr;
    UninstallManager *m_uninstallMgr;
    QList<InstalledApp> m_allApps;
};
