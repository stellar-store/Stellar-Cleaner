#include "privacycleaner.h"
#include "../utils/fileutils.h"
#include <QDir>
#include <QFile>
#include <QTextStream>

PrivacyCleaner::PrivacyCleaner(QObject *parent) : BaseCleaner(parent) {
    QString home = QDir::homePath();

    m_entries = {
        {
            "recent_files",  "Archivos Recientes",     "Lista de archivos abiertos recientemente",
            home + "/.local/share/recently-used.xbel", "Privacidad", 0, true, false, false
        },
        {
            "bash_history",  "Historial de Bash",      "~/.bash_history",
            home + "/.bash_history",                   "Privacidad", 0, false, false, false
        },
        {
            "zsh_history",   "Historial de Zsh",       "~/.zsh_history",
            home + "/.zsh_history",                    "Privacidad", 0, false, false, false
        },
        {
            "fish_history",  "Historial de Fish",      "~/.local/share/fish/fish_history",
            home + "/.local/share/fish/fish_history",  "Privacidad", 0, false, false, false
        },
        {
            "gtk_bookmarks", "Marcadores GTK",         "Lugares recientes de GTK",
            home + "/.config/gtk-3.0/bookmarks",       "Privacidad", 0, true, false, false
        },
        {
            "kde_recent",    "Recientes de KDE",       "Actividades recientes de KDE Plasma",
            home + "/.local/share/RecentDocuments",    "Privacidad", 0, true, false, false
        },
        {
            "xsession_errors","Errores de Sesión X",   "~/.xsession-errors",
            home + "/.xsession-errors",                "Sistema",   0, true, false, false
        },
        {
            "gnome_run",     "Historial de Ejecutar",  "Historial del diálogo 'Ejecutar' de GNOME",
            home + "/.config/gnome-control-center",   "Privacidad", 0, true, false, false
        },
        {
            "thumbnails_fail","Miniaturas Fallidas",   "Miniaturas de archivos que ya no existen",
            home + "/.cache/thumbnails/fail",          "Caché",     0, true, false, false
        },
        {
            "wine_temp",     "Archivos Temp de Wine",  "Temporales de Wine/Proton",
            home + "/.wine/drive_c/windows/temp",     "Wine",      0, true, false, false
        }
    };
}

void PrivacyCleaner::analyze() {
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

qint64 PrivacyCleaner::clean(const QList<CleanEntry> &selected) {
    m_cleaning = true;
    emit cleanStarted();
    qint64 freed = 0;
    int total = selected.size();

    for (int i = 0; i < total; ++i) {
        const auto &e = selected[i];
        emit cleanProgress((i * 100) / total, e.name);
        emitLog(QString("[Privacy] Cleaning: %1").arg(e.name));

        QFileInfo fi(e.path);

        if (e.id == "recent_files") {
            // Truncate the file rather than delete (app recreates it)
            QFile f(e.path);
            if (f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                QTextStream s(&f);
                s << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                  << "<bookmark-file version=\"1.0\" xmlns=\"http://www.freedesktop.org/standards/desktop-bookmarks\">\n"
                  << "</bookmark-file>\n";
                freed += fi.size();
            }
        } else if (e.id == "bash_history" || e.id == "zsh_history" || e.id == "fish_history") {
            freed += fi.size();
            QFile::remove(e.path);
        } else if (fi.isDir()) {
            freed += FileUtils::deleteDir(e.path, true, false);
        } else {
            freed += FileUtils::deleteFile(e.path);
        }
    }

    m_cleaning = false;
    emit cleanFinished(freed);
    return freed;
}
