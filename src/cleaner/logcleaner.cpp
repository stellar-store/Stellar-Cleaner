#include "logcleaner.h"
#include "../utils/fileutils.h"
#include "../utils/sudohelper.h"
#include <QProcess>
#include <QDir>
#include <QRegularExpression>
#include <QDirIterator>

LogCleaner::LogCleaner(QObject *parent) : BaseCleaner(parent) {
    QString home = QDir::homePath();

    m_entries = {
        {
            "journal",      "Journal de Systemd",   "Logs de journald (systemctl)",
            "/var/log/journal", "Sistema", 0, false, true, false
        },
        {
            "syslog",       "Syslog / kern.log",    "Logs del kernel y sistema",
            "/var/log",      "Sistema", 0, false, true, false
        },
        {
            "user_app_logs","Logs de Aplicaciones", "Logs en ~/.local/share/apps",
            home + "/.local/share",     "Usuario", 0, true,  false, false
        },
        {
            "snap_logs",    "Logs de Snap",         "Logs de paquetes Snap",
            home + "/snap",             "Snap",    0, true,  false, false
        },
        {
            "xorg_logs",    "Logs de Xorg/Wayland", "Logs del servidor gráfico",
            "/var/log",     "Sistema",   0, false, true, false
        },
        {
            "crash_logs",   "Logs de Crashes",      "Reportes de fallos en /var/crash",
            "/var/crash",   "Sistema",   0, false, true, false
        },
        {
            "apt_logs",     "Logs de APT",          "Historial de instalaciones",
            "/var/log/apt", "Sistema",   0, false, true, false
        }
    };
}

qint64 LogCleaner::getJournalSize() {
    QProcess p;
    p.start("journalctl", {"--disk-usage"});
    p.waitForFinished(5000);
    QString out = p.readAllStandardOutput();
    // Parse: "Archived and active journals take up X.XM on disk."
    QRegularExpression re(R"((\d+\.?\d*)\s*(B|K|M|G|T))");
    auto m = re.match(out);
    if (!m.hasMatch()) return FileUtils::dirSize("/var/log/journal");

    double val = m.captured(1).toDouble();
    QString unit = m.captured(2);
    if (unit == "K") return (qint64)(val * 1024);
    if (unit == "M") return (qint64)(val * 1024 * 1024);
    if (unit == "G") return (qint64)(val * 1024 * 1024 * 1024);
    return (qint64)val;
}

void LogCleaner::analyze() {
    m_analyzing = true;
    emit analyzeStarted();

    int total = m_entries.size();
    for (int i = 0; i < total; ++i) {
        auto &e = m_entries[i];
        emit analyzeProgress((i * 100) / total, e.name);

        if (e.id == "journal") {
            e.sizeBytes = getJournalSize();
        } else if (e.id == "syslog") {
            // Count only .log, .gz files
            qint64 sz = 0;
            QDir dir("/var/log");
            for (const auto &fi : dir.entryInfoList({"*.log","*.log.*","*.gz","*.bz2"},
                                                     QDir::Files)) {
                sz += fi.size();
            }
            e.sizeBytes = sz;
        } else if (e.id == "user_app_logs") {
            qint64 sz = 0;
            QDirIterator it(e.path, {"*.log","*.log.*"}, QDir::Files,
                            QDirIterator::Subdirectories);
            while (it.hasNext()) { it.next(); sz += it.fileInfo().size(); }
            e.sizeBytes = sz;
        } else if (e.id == "snap_logs") {
            qint64 sz = 0;
            QDirIterator it(e.path, QDir::Dirs | QDir::NoDotAndDotDot);
            while (it.hasNext()) {
                it.next();
                sz += FileUtils::dirSize(it.filePath() + "/common/logs");
            }
            e.sizeBytes = sz;
        } else {
            e.sizeBytes = FileUtils::dirSize(e.path);
        }
        e.analyzed = true;
    }

    m_analyzing = false;
    emit analyzeFinished(totalSize());
}

qint64 LogCleaner::clean(const QList<CleanEntry> &selected) {
    m_cleaning = true;
    emit cleanStarted();
    qint64 freed = 0;
    int total = selected.size();

    for (int i = 0; i < total; ++i) {
        const auto &e = selected[i];
        emit cleanProgress((i * 100) / total, QString("Limpiando: %1").arg(e.name));
        emitLog(QString("[Log] Cleaning: %1").arg(e.name));

        if (e.id == "journal") {
            qint64 b = 0;
            SudoHelper::instance().runJournalClean(m_journalDays, &b);
            freed += b;
        } else if (e.id == "syslog" || e.id == "xorg_logs" || e.id == "apt_logs") {
            qint64 b = 0;
            SudoHelper::instance().cleanLogDir(e.path, &b);
            freed += b;
        } else if (e.id == "crash_logs") {
            qint64 b = 0;
            SudoHelper::instance().cleanLogDir("/var/crash", &b);
            freed += b;
        } else {
            // User logs (no root needed)
            QDirIterator it(e.path, {"*.log","*.log.*"}, QDir::Files,
                            QDirIterator::Subdirectories);
            while (it.hasNext()) {
                it.next();
                freed += it.fileInfo().size();
                QFile::remove(it.filePath());
            }
        }
    }

    m_cleaning = false;
    emit cleanFinished(freed);
    return freed;
}
