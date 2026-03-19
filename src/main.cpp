#include <QApplication>
#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QTranslator>
#include <QLocale>
#include <QMessageBox>
#include <unistd.h>
#include "mainwindow.h"
#include "utils/systeminfo.h"

int main(int argc, char *argv[]) {
    // High-DPI support
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app(argc, argv);
    app.setApplicationName("Stellar Cleaner");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Stellar Cleaner");
    app.setOrganizationDomain("stellarcleaner.io");

    // Use Fusion style as base (dark theme overrides it)
    app.setStyle(QStyleFactory::create("Fusion"));

    // Set default font
    QFont defaultFont("Noto Sans", 10);
    if (!QFontDatabase().families().contains("Noto Sans")) {
        defaultFont = QFont("DejaVu Sans", 10);
    }
    app.setFont(defaultFont);

    // Warn if running as root
    if (geteuid() == 0) {
        QMessageBox::warning(nullptr, "Advertencia",
            "Estás ejecutando Stellar Cleaner como root.\n"
            "Todas las operaciones se ejecutarán con privilegios máximos.\n"
            "Úsalo con precaución.");
    }

    // Check Linux
    QFile osRelease("/etc/os-release");
    if (!osRelease.exists()) {
        QMessageBox::critical(nullptr, "Sistema no compatible",
            "Stellar Cleaner solo funciona en sistemas Linux.");
        return 1;
    }

    MainWindow w;
    w.show();

    return app.exec();
}
