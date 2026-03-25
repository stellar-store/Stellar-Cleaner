#include "mainwindow.h"
#include "utils/systeminfo.h"
#include "utils/fileutils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QStatusBar>
#include <QCloseEvent>
#include <QSettings>
#include <QMessageBox>
#include <QIcon>
#include <QPainterPath>
#include <QRegion>
#include <QResizeEvent>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Stellar Cleaner");
    setWindowFlags(Qt::FramelessWindowHint);  // Remove system window frame
    setAttribute(Qt::WA_TranslucentBackground);  // Enable transparent background for rounded corners
    setMinimumSize(1100, 700);
    resize(1280, 800);
    setWindowIcon(QIcon(":/icons/stellarcleaner.png"));

    loadStyleSheet();
    setupUI();
    setupStatusBar();

    // Restore geometry
    QSettings s("Stellar Cleaner", "Window");
    if (s.contains("geometry")) restoreGeometry(s.value("geometry").toByteArray());
    
    // Apply rounded corners on initial show
    applyRoundedCorners();
}

void MainWindow::loadStyleSheet() {
    QFile f(":/styles/dark.qss");
    if (f.open(QIODevice::ReadOnly)) {
        qApp->setStyleSheet(QTextStream(&f).readAll());
    }
}

void MainWindow::setupUI() {
    auto *centralW = new QWidget(this);
    centralW->setObjectName("centralWidget");
    setCentralWidget(centralW);

    auto *mainLay = new QVBoxLayout(centralW);
    mainLay->setContentsMargins(0,0,0,0);
    mainLay->setSpacing(0);

    // Title bar
    m_titleBar = new TitleBar(centralW);
    mainLay->addWidget(m_titleBar);

    // Connect title bar buttons
    connect(m_titleBar, &TitleBar::minimizeRequested, this, &QWidget::showMinimized);
    connect(m_titleBar, &TitleBar::closeRequested, this, &QWidget::close);
    connect(m_titleBar, &TitleBar::maximizeRequested, this, [this]() {
        if (isMaximized()) {
            showNormal();
            m_titleBar->setMaximized(false);
        } else {
            showMaximized();
            m_titleBar->setMaximized(true);
        }
    });

    // Body: sidebar + pages
    auto *bodyW = new QWidget(centralW);
    auto *bodyLay = new QHBoxLayout(bodyW);
    bodyLay->setContentsMargins(0,0,0,0);
    bodyLay->setSpacing(0);

    m_sidebar = new Sidebar(bodyW);
    bodyLay->addWidget(m_sidebar);

    m_pages = new QStackedWidget(bodyW);
    m_pages->setObjectName("contentArea");
    bodyLay->addWidget(m_pages, 1);

    mainLay->addWidget(bodyW, 1);

    // Create pages
    m_dashboard   = new Dashboard(m_pages);
    m_cleanerPage = new CleanerPage(m_pages);
    m_analyzerPage= new AnalyzerPage(m_pages);
    m_kernelPage  = new KernelPage(m_pages);
    m_optimizerPage = new OptimizerPage(m_pages);
    m_toolsPage   = new ToolsPage(m_pages);
    m_settingsPage= new SettingsPage(m_pages);

    m_pages->addWidget(m_dashboard);    // 0
    m_pages->addWidget(m_cleanerPage);  // 1
    m_pages->addWidget(m_analyzerPage); // 2
    m_pages->addWidget(m_kernelPage);   // 3
    m_pages->addWidget(m_optimizerPage); // 4
    m_pages->addWidget(m_toolsPage);    // 5
    m_pages->addWidget(m_settingsPage); // 6

    connect(m_sidebar, &Sidebar::pageRequested, this, &MainWindow::onPageRequested);
    connect(m_cleanerPage, &CleanerPage::totalFreedChanged,
            this, &MainWindow::onTotalFreedChanged);
    connect(m_kernelPage, &KernelPage::totalFreedChanged,
            this, &MainWindow::onTotalFreedChanged);
    connect(m_optimizerPage, &OptimizerPage::totalFreedChanged,
            this, &MainWindow::onTotalFreedChanged);

    m_pages->setCurrentIndex(0);
    m_sidebar->setActiveIndex(0);
}

void MainWindow::setupStatusBar() {
    auto *sb = statusBar();
    sb->setStyleSheet("QStatusBar { background:#0a0d14; color:#4a5568; "
                      "border-top:1px solid #1e2535; font-size:11px; }");

    m_statusDistro = new QLabel(this);
    m_statusUser   = new QLabel(this);
    m_statusFreed  = new QLabel(this);
    m_statusFreed->setStyleSheet("color:#00d4aa;");

    auto &si = SystemInfo::instance();
    m_statusDistro->setText("🐧 " + si.distroName() + " " + si.distroVersion());
    m_statusUser->setText("👤 " + si.username() + "@" + si.hostname());

    sb->addWidget(m_statusDistro);
    sb->addWidget(new QLabel(" | "));
    sb->addWidget(m_statusUser);
    sb->addWidget(new QLabel(" | "));
    sb->addWidget(new QLabel("⚙️ " + si.packageManagerName()));
    sb->addPermanentWidget(m_statusFreed);
    
    // Add about button
    m_aboutBtn = new QPushButton("ℹ️ Acerca de", this);
    m_aboutBtn->setMaximumWidth(120);
    m_aboutBtn->setStyleSheet(
        "QPushButton { background:#00d4aa; color:#0f1117; border:none; "
        "padding:4px 10px; border-radius:4px; font-weight:bold; font-size:10px; }"
        "QPushButton:hover { background:#00e4ba; }"
        "QPushButton:pressed { background:#00c494; }"
    );
    connect(m_aboutBtn, &QPushButton::clicked, this, &MainWindow::showAbout);
    sb->addPermanentWidget(m_aboutBtn);
    sb->addPermanentWidget(new QLabel("Stellar Cleaner v1.0.0  "));
}

void MainWindow::onPageRequested(int idx) {
    if (idx < m_pages->count()) {
        m_pages->setCurrentIndex(idx);
        m_sidebar->setActiveIndex(idx);
        // Update title bar
        QStringList titles = {
            "Panel de Control", "Limpiador del Sistema",
            "Analizador de Disco", "Limpiador del Kernel",
            "Optimizador de Rendimiento", "Herramientas", "Configuración"
        };
        if (idx < titles.size())
            m_titleBar->setTitle("Stellar Cleaner — " + titles[idx]);
    }
}

void MainWindow::onTotalFreedChanged(qint64 bytes) {
    m_totalFreedSession += bytes;
    m_statusFreed->setText(QString("✅ Liberados esta sesión: %1  ")
        .arg(FileUtils::formatSize(m_totalFreedSession)));
    m_titleBar->setFreedBytes(m_totalFreedSession);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings s("Stellar Cleaner", "Window");
    s.setValue("geometry", saveGeometry());
    event->accept();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    m_titleBar->setMaximized(isMaximized());
    applyRoundedCorners();  // Apply rounded corners on resize
}

void MainWindow::applyRoundedCorners() {
    // Create a rounded rectangle path
    QPainterPath path;
    const int radius = 7;  // Radius of rounded corners in pixels
    path.addRoundedRect(rect(), radius, radius);
    
    // Apply the path as a mask to create rounded corners
    setMask(QRegion(path.toFillPolygon().toPolygon()));
}

void MainWindow::showAbout() {
    QMessageBox::information(this, "Acerca de Stellar Cleaner",
        "<h2>Stellar Cleaner v1.0.0</h2>"
        "<p><b>Un potente limpiador de sistema para Linux</b></p>"
        "<h3>Características:</h3>"
        "<ul>"
        "<li>🧹 Limpieza de archivos temporales</li>"
        "<li>🔍 Análisis completo del disco</li>"
        "<li>🌐 Limpieza de navegadores y caché</li>"
        "<li>📦 Gestor de paquetes</li>"
        "<li>🛠️ Herramientas avanzadas del sistema</li>"
        "</ul>"
        "<p><b>© 2026 - Todos los derechos reservados</b></p>"
    );
}
