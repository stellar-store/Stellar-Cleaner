#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QStatusBar>
#include <QLabel>
#include <QPushButton>
#include "ui/sidebar.h"
#include "ui/titlebar.h"
#include "ui/dashboard.h"
#include "ui/cleanerpage.h"
#include "ui/analyzerpage.h"
#include "ui/kernelpage.h"
#include "ui/optimizerpage.h"
#include "ui/toolspage.h"
#include "ui/settingspage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onPageRequested(int idx);
    void onTotalFreedChanged(qint64 bytes);
    void showAbout();

private:
    void setupUI();
    void loadStyleSheet();
    void setupStatusBar();
    void applyRoundedCorners();

    TitleBar      *m_titleBar;
    Sidebar       *m_sidebar;
    QStackedWidget *m_pages;

    Dashboard     *m_dashboard;
    CleanerPage   *m_cleanerPage;
    AnalyzerPage  *m_analyzerPage;
    KernelPage    *m_kernelPage;
    OptimizerPage *m_optimizerPage;
    ToolsPage     *m_toolsPage;
    SettingsPage  *m_settingsPage;

    QLabel *m_statusDistro;
    QLabel *m_statusUser;
    QLabel *m_statusFreed;
    QPushButton *m_aboutBtn;
    qint64 m_totalFreedSession = 0;
};
