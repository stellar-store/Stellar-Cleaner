#include "trashcleaner.h"
#include "../utils/fileutils.h"
#include <QDir>
#include <QDirIterator>
#include <QStorageInfo>
#include <unistd.h>

TrashCleaner::TrashCleaner(QObject *parent) : BaseCleaner(parent) {
    QString home = QDir::homePath();

    m_entries = {
        {
            "user_trash", "Papelera del Usuario", "~/.local/share/Trash",
            home + "/.local/share/Trash", "Papelera", 0, true, false, false
        },
        {
            "root_trash", "Papelera de Root", "/root/.local/share/Trash",
            "/root/.local/share/Trash",   "Papelera", 0, false, true, false
        }
    };

    // Also detect trash in mounted drives
    for (const auto &vol : QStorageInfo::mountedVolumes()) {
        QString tp = vol.rootPath() + "/.Trash-" +
                     QString::number(getuid());
        if (QDir(tp).exists()) {
            CleanEntry e;
            e.id          = "drive_trash_" + vol.rootPath();
            e.name        = QString("Papelera de %1").arg(vol.displayName());
            e.description = tp;
            e.path        = tp;
            e.category    = "Papelera";
            e.selected    = true;
            m_entries << e;
        }
    }
}

void TrashCleaner::analyze() {
    m_analyzing = true;
    emit analyzeStarted();
    for (int i = 0; i < m_entries.size(); ++i) {
        auto &e = m_entries[i];
        emit analyzeProgress((i * 100) / m_entries.size(), e.name);
        e.sizeBytes = FileUtils::dirSize(e.path + "/files");
        e.analyzed  = true;
    }
    m_analyzing = false;
    emit analyzeFinished(totalSize());
}

qint64 TrashCleaner::clean(const QList<CleanEntry> &selected) {
    m_cleaning = true;
    emit cleanStarted();
    qint64 freed = 0;
    for (int i = 0; i < selected.size(); ++i) {
        const auto &e = selected[i];
        emit cleanProgress((i * 100) / selected.size(), e.name);
        emitLog(QString("[Trash] Emptying: %1").arg(e.path));
        freed += FileUtils::deleteDir(e.path + "/files",  true, false);
        freed += FileUtils::deleteDir(e.path + "/info",   true, false);
        freed += FileUtils::deleteDir(e.path + "/expunged", true, false);
    }
    m_cleaning = false;
    emit cleanFinished(freed);
    return freed;
}
