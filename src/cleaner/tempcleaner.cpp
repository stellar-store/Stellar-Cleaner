#include "tempcleaner.h"
#include "../utils/fileutils.h"
#include <QDir>
#include <QStandardPaths>

TempCleaner::TempCleaner(QObject *parent) : BaseCleaner(parent) {
    setupEntries();
}

void TempCleaner::setupEntries() {
    QString home = QDir::homePath();

    m_entries = {
        {
            "sys_tmp", "Temporales del Sistema", "Archivos en /tmp",
            "/tmp", "Sistema", 0, true, false, false
        },
        {
            "var_tmp", "Temporales de Var", "Archivos en /var/tmp (>7 días)",
            "/var/tmp", "Sistema", 0, true, false, false
        },
        {
            "user_cache", "Caché del Usuario", "Caché general de ~/.cache",
            home + "/.cache", "Usuario", 0, true, false, false
        },
        {
            "thumbnails", "Miniaturas", "Miniaturas de imágenes en ~/.cache/thumbnails",
            home + "/.cache/thumbnails", "Usuario", 0, true, false, false
        },
        {
            "user_tmp", "Temporales de Usuario", "~/.local/share/recently-used.xbel y temporales",
            home + "/.local/tmp", "Usuario", 0, true, false, false
        },
        {
            "crash_reports", "Reportes de Crashes", "Reportes de fallos del sistema",
            "/var/crash", "Sistema", 0, false, true, false
        },
        {
            "coredumps", "Core Dumps", "Volcados de memoria de procesos fallidos",
            "/var/lib/systemd/coredump", "Sistema", 0, false, true, false
        }
    };
}

void TempCleaner::analyze() {
    m_analyzing = true;
    emit analyzeStarted();

    int total = m_entries.size();
    for (int i = 0; i < total; ++i) {
        auto &e = m_entries[i];
        emit analyzeProgress((i * 100) / total, e.name);
        e.sizeBytes = FileUtils::dirSize(e.path);
        e.analyzed  = true;
    }

    m_analyzing = false;
    emit analyzeFinished(totalSize());
}

qint64 TempCleaner::clean(const QList<CleanEntry> &selected) {
    m_cleaning = true;
    emit cleanStarted();

    qint64 freed = 0;
    int total = selected.size();

    for (int i = 0; i < total; ++i) {
        const auto &e = selected[i];
        emit cleanProgress((i * 100) / total, QString("Limpiando: %1").arg(e.name));
        emitLog(QString("[Temp] Cleaning: %1").arg(e.path));

        if (e.id == "sys_tmp") {
            freed += FileUtils::deleteDir(e.path, true, false);
        } else if (e.id == "var_tmp") {
            freed += FileUtils::deleteOlderThan(e.path, 7);
        } else if (e.id == "user_cache") {
            // Don't delete subdirs that are managed by browsers
            QStringList skip = {"mozilla","google-chrome","chromium","BraveSoftware",
                                "thumbnails","mesa_shader_cache","fontconfig"};
            QDir dir(e.path);
            for (const auto &sub : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                if (!skip.contains(sub)) {
                    freed += FileUtils::deleteDir(e.path + "/" + sub, true, false);
                }
            }
        } else {
            freed += FileUtils::deleteDir(e.path, true, false);
        }

        // Reset size
        const_cast<CleanEntry&>(e).sizeBytes = 0;
    }

    m_cleaning = false;
    emit cleanFinished(freed);
    return freed;
}
