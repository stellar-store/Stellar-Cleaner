#pragma once
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>

class Dashboard : public QWidget {
    Q_OBJECT
public:
    explicit Dashboard(QWidget *parent = nullptr);

    void refresh();

private slots:
    void onRefreshTimer();

private:
    void setupUI();
    void setupCharts();
    void updateSystemStats();
    void updateDiskChart();
    void updateMemChart();

    // Stat cards
    QLabel *m_lblDistro;
    QLabel *m_lblKernel;
    QLabel *m_lblUptime;
    QLabel *m_lblHostname;

    // Disk
    QLabel *m_diskUsed;
    QLabel *m_diskTotal;
    QProgressBar *m_diskBar;

    // Memory
    QLabel *m_memUsed;
    QLabel *m_memTotal;
    QProgressBar *m_memBar;

    // CPU
    QLabel *m_cpuPercent;
    QLabel *m_cpuModel;
    QProgressBar *m_cpuBar;

    // Charts
    QChartView *m_diskChart;
    QChartView *m_memChart;
    QPieSeries *m_diskSeries;
    QLineSeries *m_cpuSeries;

    QTimer *m_refreshTimer;
    QList<double> m_cpuHistory;
};
