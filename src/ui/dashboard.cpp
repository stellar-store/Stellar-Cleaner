#include "dashboard.h"
#include "../utils/systeminfo.h"
#include "../utils/fileutils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QFont>
#include <QStorageInfo>
#include <QtCharts/QChart>
#include <QtCharts/QPieSlice>
#include <QtCharts/QValueAxis>

static QWidget* makeCard(QWidget *parent = nullptr) {
    auto *w = new QWidget(parent);
    w->setObjectName("card");
    w->setMinimumHeight(100);
    return w;
}

static QLabel* makeStatVal(const QString &text = QStringLiteral("—"), QWidget *parent = nullptr) {
    auto *l = new QLabel(text, parent);
    l->setObjectName("statValue");
    QFont f = l->font();
    f.setPointSize(20);
    f.setWeight(QFont::Bold);
    l->setFont(f);
    return l;
}

static QLabel* makeStatLabel(const QString &text, QWidget *parent = nullptr) {
    auto *l = new QLabel(text.toUpper(), parent);
    l->setObjectName("statLabel");
    return l;
}

Dashboard::Dashboard(QWidget *parent) : QWidget(parent) {
    setupUI();
    setupCharts();
    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(3000);
    connect(m_refreshTimer, &QTimer::timeout, this, &Dashboard::onRefreshTimer);
    m_refreshTimer->start();
    refresh();
}

void Dashboard::setupUI() {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(24, 20, 24, 20);
    root->setSpacing(16);

    // Top info cards
    auto *cardsRow = new QHBoxLayout();
    cardsRow->setSpacing(12);

    auto makeInfoCard = [&](QLabel *&valLbl, const QString &labelText) {
        auto *card = makeCard(this);
        auto *lay  = new QVBoxLayout(card);
        lay->setContentsMargins(16,14,16,14);
        lay->addWidget(makeStatLabel(labelText, card));
        valLbl = makeStatVal("—", card);
        lay->addWidget(valLbl);
        cardsRow->addWidget(card, 1);
    };

    makeInfoCard(m_lblDistro,   "Distribución");
    makeInfoCard(m_lblKernel,   "Kernel");
    makeInfoCard(m_lblUptime,   "Uptime");
    makeInfoCard(m_lblHostname, "Host / Usuario");
    root->addLayout(cardsRow);

    // Resource monitors
    auto *resRow = new QHBoxLayout();
    resRow->setSpacing(12);

    auto makeResCard = [&](const QString &title, QLabel *&usedLbl, QLabel *&totalLbl, QProgressBar *&bar) {
        auto *card = makeCard(this);
        card->setMinimumHeight(90);
        auto *lay = new QVBoxLayout(card);
        lay->setContentsMargins(16,12,16,12);
        auto *hdr = new QHBoxLayout();
        hdr->addWidget(makeStatLabel(title, card));
        hdr->addStretch();
        usedLbl = new QLabel("—", card);
        usedLbl->setObjectName("accent");
        totalLbl = new QLabel("—", card);
        totalLbl->setObjectName("statLabel");
        hdr->addWidget(usedLbl);
        hdr->addWidget(new QLabel(" / ", card));
        hdr->addWidget(totalLbl);
        lay->addLayout(hdr);
        bar = new QProgressBar(card);
        bar->setTextVisible(false);
        bar->setFixedHeight(6);
        lay->addWidget(bar);
        lay->addStretch();
        resRow->addWidget(card, 1);
    };

    makeResCard("Disco Principal (/)", m_diskUsed, m_diskTotal, m_diskBar);
    makeResCard("Memoria RAM",          m_memUsed,  m_memTotal,  m_memBar);

    // CPU card special
    {
        auto *card = makeCard(this);
        card->setMinimumHeight(90);
        auto *lay = new QVBoxLayout(card);
        lay->setContentsMargins(16,12,16,12);
        auto *hdr = new QHBoxLayout();
        hdr->addWidget(makeStatLabel("CPU", card));
        hdr->addStretch();
        m_cpuPercent = new QLabel("—%", card);
        m_cpuPercent->setObjectName("accent");
        hdr->addWidget(m_cpuPercent);
        lay->addLayout(hdr);
        m_cpuModel = new QLabel("", card);
        m_cpuModel->setObjectName("statLabel");
        lay->addWidget(m_cpuModel);
        m_cpuBar = new QProgressBar(card);
        m_cpuBar->setTextVisible(false);
        m_cpuBar->setFixedHeight(6);
        lay->addWidget(m_cpuBar);
        resRow->addWidget(card, 1);
    }

    root->addLayout(resRow);

    // Charts row
    auto *chartsRow = new QHBoxLayout();
    chartsRow->setSpacing(12);
    m_diskChart = new QChartView(this);
    m_diskChart->setObjectName("card");
    m_diskChart->setMinimumHeight(220);
    m_diskChart->setRenderHint(QPainter::Antialiasing);
    chartsRow->addWidget(m_diskChart, 1);

    m_memChart = new QChartView(this);
    m_memChart->setObjectName("card");
    m_memChart->setMinimumHeight(220);
    m_memChart->setRenderHint(QPainter::Antialiasing);
    chartsRow->addWidget(m_memChart, 2);
    root->addLayout(chartsRow, 1);
}

void Dashboard::setupCharts() {
    // Disk pie chart
    {
        m_diskSeries = new QPieSeries();
        m_diskSeries->append("Usado", 50)->setBrush(QColor("#ef4444"));
        m_diskSeries->append("Libre", 50)->setBrush(QColor("#1e2535"));
        m_diskSeries->setHoleSize(0.55);

        auto *chart = new QChart();
        chart->addSeries(m_diskSeries);
        chart->setTitle("Disco");
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->setBackgroundBrush(Qt::transparent);
        chart->setTitleBrush(QBrush(QColor("#8892a4")));
        chart->legend()->setLabelBrush(QBrush(QColor("#8892a4")));
        chart->setMargins(QMargins(8,8,8,8));
        m_diskChart->setChart(chart);
        m_diskChart->setBackgroundBrush(Qt::transparent);
    }

    // CPU line chart
    {
        m_cpuSeries = new QLineSeries();
        m_cpuSeries->setColor(QColor("#00d4aa"));
        for (int i = 0; i < 20; ++i) {
            m_cpuSeries->append(i, 0);
            m_cpuHistory << 0;
        }

        auto *chart = new QChart();
        chart->addSeries(m_cpuSeries);
        chart->setTitle("Uso de CPU (últimos 60s)");
        chart->setBackgroundBrush(Qt::transparent);
        chart->setTitleBrush(QBrush(QColor("#8892a4")));
        chart->legend()->setVisible(false);

        auto *axX = new QValueAxis(); axX->setRange(0,20); axX->setVisible(false);
        auto *axY = new QValueAxis(); axY->setRange(0,100);
        axY->setLabelFormat("%d%%");
        axY->setLabelsColor(QColor("#4a5568"));
        axY->setGridLineColor(QColor("#1e2535"));
        chart->addAxis(axX, Qt::AlignBottom);
        chart->addAxis(axY, Qt::AlignLeft);
        m_cpuSeries->attachAxis(axX);
        m_cpuSeries->attachAxis(axY);
        chart->setMargins(QMargins(8,8,8,8));

        m_memChart->setChart(chart);
        m_memChart->setBackgroundBrush(Qt::transparent);
    }
}

void Dashboard::refresh() {
    auto &si = SystemInfo::instance();

    m_lblDistro->setText(si.distroName());

    QString kern = si.kernelVersion();
    m_lblKernel->setText(kern.left(20));

    m_lblUptime->setText(si.uptime());
    m_lblHostname->setText(si.username() + "@" + si.hostname());

    updateDiskChart();
    updateMemChart();
    updateSystemStats();
}

void Dashboard::updateDiskChart() {
    auto &si = SystemInfo::instance();
    DiskInfo d = si.diskInfo("/");
    m_diskUsed->setText(FileUtils::formatSize(d.used, 1));
    m_diskTotal->setText(FileUtils::formatSize(d.total, 1));
    m_diskBar->setMaximum(100);
    m_diskBar->setValue((int)d.usePercent);

    // Update pie
    if (m_diskSeries->count() >= 2) {
        m_diskSeries->slices().at(0)->setValue(d.used);
        m_diskSeries->slices().at(0)->setLabel(QString("Usado %1%").arg((int)d.usePercent));
        m_diskSeries->slices().at(1)->setValue(d.available);
        m_diskSeries->slices().at(1)->setLabel(QString("Libre %1%").arg(100-(int)d.usePercent));
    }
}

void Dashboard::updateMemChart() {
    auto &si = SystemInfo::instance();
    MemInfo m = si.memInfo();
    m_memUsed->setText(FileUtils::formatSize(m.used, 1));
    m_memTotal->setText(FileUtils::formatSize(m.total, 1));
    int pct = m.total > 0 ? (int)((double)m.used / m.total * 100) : 0;
    m_memBar->setMaximum(100);
    m_memBar->setValue(pct);
}

void Dashboard::updateSystemStats() {
    auto &si = SystemInfo::instance();
    CpuInfo c = si.cpuInfo();
    m_cpuPercent->setText(QString("%1%").arg((int)c.usagePercent));
    m_cpuModel->setText(c.model.left(35));
    m_cpuBar->setMaximum(100);
    m_cpuBar->setValue((int)c.usagePercent);

    // Update line chart
    m_cpuHistory.append(c.usagePercent);
    if (m_cpuHistory.size() > 20) m_cpuHistory.removeFirst();

    m_cpuSeries->clear();
    for (int i = 0; i < m_cpuHistory.size(); ++i)
        m_cpuSeries->append(i, m_cpuHistory[i]);
}

void Dashboard::onRefreshTimer() {
    updateSystemStats();
    updateMemChart();
}
