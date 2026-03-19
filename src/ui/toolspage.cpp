#include "toolspage.h"
#include "../utils/fileutils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QtConcurrent>

ToolsPage::ToolsPage(QWidget *parent) : QWidget(parent) {
    m_startupMgr  = new StartupManager(this);
    m_procMgr     = new ProcessManager(this);
    m_uninstallMgr= new UninstallManager(this);
    m_procTimer   = new QTimer(this);
    m_procTimer->setInterval(5000);
    connect(m_procTimer, &QTimer::timeout, this, &ToolsPage::onProcessRefresh);
    setupUI();
}

void ToolsPage::setupUI() {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);

    // Header
    auto *tb = new QWidget(this);
    tb->setObjectName("pageHeader");
    tb->setFixedHeight(56);
    auto *tbLay = new QHBoxLayout(tb);
    tbLay->setContentsMargins(20,8,20,8);
    auto *ttl = new QLabel("Herramientas del Sistema", tb);
    ttl->setObjectName("pageTitle");
    tbLay->addWidget(ttl);
    auto *sub = new QLabel("  Gestión de inicio, procesos y aplicaciones instaladas", tb);
    sub->setObjectName("pageSubtitle");
    tbLay->addWidget(sub);
    tbLay->addStretch();
    root->addWidget(tb);

    m_tabs = new QTabWidget(this);
    m_tabs->setContentsMargins(16,16,16,16);

    auto *startupW = new QWidget();
    auto *processW = new QWidget();
    auto *uninstW  = new QWidget();

    setupStartupTab(startupW);
    setupProcessTab(processW);
    setupUninstallTab(uninstW);

    m_tabs->addTab(startupW, "🚀  Inicio Automático");
    m_tabs->addTab(processW, "📋  Procesos");
    m_tabs->addTab(uninstW,  "📦  Aplicaciones Instaladas");

    connect(m_tabs, &QTabWidget::currentChanged, this, [this](int idx) {
        if (idx == 0) onStartupTabShow();
        else if (idx == 1) onProcessTabShow();
        else if (idx == 2) onUninstallTabShow();
    });

    root->addWidget(m_tabs, 1);
    onStartupTabShow();
}

void ToolsPage::setupStartupTab(QWidget *p) {
    auto *lay = new QVBoxLayout(p);
    lay->setContentsMargins(12,12,12,12);

    auto *btnRow = new QHBoxLayout();
    auto *btnRefresh = new QPushButton("🔄  Actualizar");
    btnRefresh->setObjectName("btnSecondary");
    m_btnStartupToggle = new QPushButton("⏸  Deshabilitar");
    m_btnStartupToggle->setObjectName("btnWarn");
    m_btnStartupToggle->setEnabled(false);
    auto *btnRemove = new QPushButton("🗑  Eliminar entrada");
    btnRemove->setObjectName("btnDanger");
    btnRemove->setEnabled(false);
    m_btnStartupToggle = m_btnStartupToggle;
    btnRow->addWidget(btnRefresh);
    btnRow->addStretch();
    btnRow->addWidget(m_btnStartupToggle);
    btnRow->addWidget(btnRemove);
    lay->addLayout(btnRow);

    m_startupTree = new QTreeWidget(p);
    m_startupTree->setColumnCount(5);
    m_startupTree->setHeaderLabels({"Nombre","Tipo","Estado","Comando","Descripción"});
    m_startupTree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_startupTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_startupTree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_startupTree->header()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_startupTree->header()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_startupTree->setAlternatingRowColors(true);
    lay->addWidget(m_startupTree, 1);

    connect(btnRefresh, &QPushButton::clicked, this, &ToolsPage::populateStartup);
    connect(m_startupTree, &QTreeWidget::itemSelectionChanged, this, [this, btnRemove]() {
        bool sel = !m_startupTree->selectedItems().isEmpty();
        m_btnStartupToggle->setEnabled(sel);
        btnRemove->setEnabled(sel);
    });
    connect(m_btnStartupToggle, &QPushButton::clicked, this, &ToolsPage::onStartupToggle);
    connect(btnRemove, &QPushButton::clicked, this, &ToolsPage::onStartupRemove);
}

void ToolsPage::setupProcessTab(QWidget *p) {
    auto *lay = new QVBoxLayout(p);
    lay->setContentsMargins(12,12,12,12);

    auto *btnRow = new QHBoxLayout();
    auto *btnRefresh = new QPushButton("🔄  Actualizar");
    btnRefresh->setObjectName("btnSecondary");
    m_btnKill = new QPushButton("⛔  Terminar Proceso");
    m_btnKill->setObjectName("btnDanger");
    m_btnKill->setEnabled(false);
    m_lblProcStatus = new QLabel("", p);
    m_lblProcStatus->setObjectName("statLabel");

    btnRow->addWidget(btnRefresh);
    btnRow->addWidget(new QLabel("  Auto-refresh cada 5s", p));
    btnRow->addStretch();
    btnRow->addWidget(m_lblProcStatus);
    btnRow->addWidget(m_btnKill);
    lay->addLayout(btnRow);

    m_procTable = new QTableWidget(0, 6, p);
    m_procTable->setHorizontalHeaderLabels({"PID","Nombre","Usuario","CPU%","RAM","Comando"});
    m_procTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_procTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_procTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_procTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_procTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_procTable->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    m_procTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_procTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_procTable->setAlternatingRowColors(true);
    m_procTable->setSortingEnabled(true);
    lay->addWidget(m_procTable, 1);

    connect(btnRefresh, &QPushButton::clicked, this, &ToolsPage::onProcessRefresh);
    connect(m_procTable, &QTableWidget::itemSelectionChanged, this,
        [this]() { m_btnKill->setEnabled(!m_procTable->selectedItems().isEmpty()); });
    connect(m_btnKill, &QPushButton::clicked, this, &ToolsPage::onProcessKill);
}

void ToolsPage::setupUninstallTab(QWidget *p) {
    auto *lay = new QVBoxLayout(p);
    lay->setContentsMargins(12,12,12,12);

    auto *btnRow = new QHBoxLayout();
    auto *btnRefresh = new QPushButton("🔄  Actualizar lista");
    btnRefresh->setObjectName("btnSecondary");
    m_btnUninstall = new QPushButton("🗑  Desinstalar");
    m_btnUninstall->setObjectName("btnDanger");
    m_btnUninstall->setEnabled(false);
    m_searchApps = new QLineEdit(p);
    m_searchApps->setPlaceholderText("🔍  Buscar aplicación...");
    m_searchApps->setMinimumWidth(250);

    btnRow->addWidget(m_searchApps);
    btnRow->addStretch();
    btnRow->addWidget(btnRefresh);
    btnRow->addWidget(m_btnUninstall);
    lay->addLayout(btnRow);

    m_appTable = new QTableWidget(0, 5, p);
    m_appTable->setHorizontalHeaderLabels({"Nombre","Versión","Tamaño","Gestor","Descripción"});
    m_appTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_appTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_appTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_appTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_appTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_appTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_appTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_appTable->setAlternatingRowColors(true);
    m_appTable->setSortingEnabled(true);
    lay->addWidget(m_appTable, 1);

    connect(btnRefresh, &QPushButton::clicked, this, &ToolsPage::onUninstallTabShow);
    connect(m_searchApps, &QLineEdit::textChanged, this, &ToolsPage::onUninstallSearch);
    connect(m_appTable, &QTableWidget::itemSelectionChanged, this,
        [this]() { m_btnUninstall->setEnabled(!m_appTable->selectedItems().isEmpty()); });
    connect(m_btnUninstall, &QPushButton::clicked, this, &ToolsPage::onUninstall);

    connect(m_uninstallMgr, &UninstallManager::uninstallFinished, this,
        [this](bool ok, const QString &name) {
            if (ok) QMessageBox::information(this, "Desinstalado",
                QString("%1 fue desinstalado correctamente.").arg(name));
            else QMessageBox::warning(this, "Error",
                QString("No se pudo desinstalar %1.").arg(name));
            onUninstallTabShow();
        });
}

void ToolsPage::populateStartup() {
    m_startupTree->clear();

    // Autostart apps
    auto *autostartCat = new QTreeWidgetItem(m_startupTree, {"Aplicaciones de Inicio","autostart","","",""});
    QFont f = autostartCat->font(0); f.setBold(true); autostartCat->setFont(0, f);

    for (const auto &e : m_startupMgr->autostartApps()) {
        auto *item = new QTreeWidgetItem(autostartCat);
        item->setText(0, e.name);
        item->setText(1, "Autostart");
        item->setText(2, e.enabled ? "✅ Activo" : "⏸ Inactivo");
        item->setText(3, e.command.left(60));
        item->setText(4, e.description);
        item->setForeground(2, QBrush(e.enabled ? QColor("#22c55e") : QColor("#f59e0b")));
        item->setData(0, Qt::UserRole, QVariant::fromValue(e));
    }
    autostartCat->setExpanded(true);

    // User services
    auto *userSvcCat = new QTreeWidgetItem(m_startupTree, {"Servicios de Usuario (systemd)","systemd","","",""});
    f = userSvcCat->font(0); f.setBold(true); userSvcCat->setFont(0, f);
    for (const auto &e : m_startupMgr->userServices()) {
        auto *item = new QTreeWidgetItem(userSvcCat);
        item->setText(0, e.name);
        item->setText(1, "systemd user");
        item->setText(2, e.enabled ? "✅ Activo" : "⏹ Inactivo");
        item->setText(3, e.command);
        item->setText(4, e.description.left(60));
        item->setForeground(2, QBrush(e.enabled ? QColor("#22c55e") : QColor("#4a5568")));
        item->setData(0, Qt::UserRole, QVariant::fromValue(e));
    }
    userSvcCat->setExpanded(true);
    m_startupTree->expandAll();
}

void ToolsPage::populateProcesses() {
    auto procs = m_procMgr->processes();
    m_procTable->setSortingEnabled(false);
    m_procTable->setRowCount(procs.size());
    for (int i = 0; i < procs.size(); ++i) {
        const auto &p = procs[i];
        m_procTable->setItem(i, 0, new QTableWidgetItem(QString::number(p.pid)));
        m_procTable->setItem(i, 1, new QTableWidgetItem(p.name));
        m_procTable->setItem(i, 2, new QTableWidgetItem(p.user));
        m_procTable->setItem(i, 3, new QTableWidgetItem(QString("%1%").arg(p.cpuPercent, 0,'f',1)));
        m_procTable->setItem(i, 4, new QTableWidgetItem(FileUtils::formatSize(p.memBytes, 0)));
        m_procTable->setItem(i, 5, new QTableWidgetItem(p.command.left(80)));
        m_procTable->item(i,0)->setData(Qt::UserRole, p.pid);
        if (p.memBytes > 500*1024*1024)
            m_procTable->item(i,4)->setForeground(QBrush(QColor("#ef4444")));
    }
    m_procTable->setSortingEnabled(true);
    m_lblProcStatus->setText(QString("%1 procesos").arg(procs.size()));
}

void ToolsPage::populateApps(const QString &filter) {
    m_appTable->setSortingEnabled(false);
    m_appTable->setRowCount(0);
    for (const auto &a : m_allApps) {
        if (!filter.isEmpty() &&
            !a.name.contains(filter, Qt::CaseInsensitive) &&
            !a.description.contains(filter, Qt::CaseInsensitive)) continue;
        int row = m_appTable->rowCount();
        m_appTable->insertRow(row);
        m_appTable->setItem(row, 0, new QTableWidgetItem(a.name));
        m_appTable->setItem(row, 1, new QTableWidgetItem(a.version));
        m_appTable->setItem(row, 2, new QTableWidgetItem(a.size));
        auto *pmItem = new QTableWidgetItem(a.packageManager);
        if (a.packageManager == "snap")    pmItem->setForeground(QBrush(QColor("#f59e0b")));
        if (a.packageManager == "flatpak") pmItem->setForeground(QBrush(QColor("#0099ff")));
        m_appTable->setItem(row, 3, pmItem);
        m_appTable->setItem(row, 4, new QTableWidgetItem(a.description));
        m_appTable->item(row,0)->setData(Qt::UserRole, QVariant::fromValue(a));
    }
    m_appTable->setSortingEnabled(true);
}

void ToolsPage::onStartupTabShow()  { populateStartup(); }
void ToolsPage::onProcessTabShow()  { populateProcesses(); m_procTimer->start(); }
void ToolsPage::onUninstallTabShow() {
    m_procTimer->stop();
    m_allApps = m_uninstallMgr->installedApps();
    populateApps();
}

void ToolsPage::onProcessRefresh() { populateProcesses(); }

void ToolsPage::onProcessKill() {
    auto sel = m_procTable->selectedItems();
    if (sel.isEmpty()) return;
    int row = sel.first()->row();
    int pid = m_procTable->item(row, 0)->data(Qt::UserRole).toInt();
    QString name = m_procTable->item(row, 1)->text();
    if (QMessageBox::question(this, "Terminar proceso",
        QString("¿Terminar el proceso '%1' (PID %2)?").arg(name).arg(pid),
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
    m_procMgr->killProcess(pid);
    QTimer::singleShot(500, this, &ToolsPage::onProcessRefresh);
}

void ToolsPage::onStartupToggle() {
    auto sel = m_startupTree->selectedItems();
    if (sel.isEmpty()) return;
    auto *item = sel.first();
    if (!item->parent()) return; // category
    StartupEntry e = item->data(0, Qt::UserRole).value<StartupEntry>();
    m_startupMgr->setEnabled(e, !e.enabled);
    populateStartup();
}

void ToolsPage::onStartupRemove() {
    auto sel = m_startupTree->selectedItems();
    if (sel.isEmpty()) return;
    auto *item = sel.first();
    if (!item->parent()) return;
    StartupEntry e = item->data(0, Qt::UserRole).value<StartupEntry>();
    if (QMessageBox::question(this, "Eliminar entrada de inicio",
        QString("¿Eliminar '%1' del inicio automático?").arg(e.name),
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
    m_startupMgr->removeEntry(e);
    populateStartup();
}

void ToolsPage::onUninstall() {
    auto sel = m_appTable->selectedItems();
    if (sel.isEmpty()) return;
    int row = sel.first()->row();
    InstalledApp app = m_appTable->item(row,0)->data(Qt::UserRole).value<InstalledApp>();
    if (QMessageBox::question(this, "Desinstalar aplicación",
        QString("¿Desinstalar '%1' (%2)?").arg(app.name, app.packageManager),
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;
    m_btnUninstall->setEnabled(false);
    m_btnUninstall->setText("Desinstalando...");
    QtConcurrent::run([this, app]() mutable {
        m_uninstallMgr->uninstall(app);
        QMetaObject::invokeMethod(m_btnUninstall, "setText",
            Qt::QueuedConnection, Q_ARG(QString, "🗑  Desinstalar"));
    });
}

void ToolsPage::onUninstallSearch(const QString &text) {
    populateApps(text);
}
