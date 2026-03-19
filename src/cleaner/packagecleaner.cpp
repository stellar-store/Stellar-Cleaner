#include "packagecleaner.h"
#include "../utils/fileutils.h"
#include "../utils/sudohelper.h"
#include <QProcess>
#include <QFile>

PackageCleaner::PackageCleaner(QObject *parent) : BaseCleaner(parent) {
    m_pm = SystemInfo::instance().detectPackageManager();

    m_entries = {
        {
            "apt_cache",    "Caché de APT",       "Paquetes .deb descargados en /var/cache/apt",
            "/var/cache/apt/archives", "Paquetes", 0, true, true, false
        },
        {
            "dnf_cache",    "Caché de DNF",       "Caché de paquetes DNF/YUM",
            "/var/cache/dnf",          "Paquetes", 0, false, true, false
        },
        {
            "pacman_cache", "Caché de Pacman",    "Paquetes viejos en /var/cache/pacman",
            "/var/cache/pacman/pkg",   "Paquetes", 0, false, true, false
        },
        {
            "snap_old",     "Revisiones Viejas de Snap","Versiones snap antiguas sin usar",
            "/var/lib/snapd/snaps",    "Snap",     0, true,  true, false
        },
        {
            "flatpak_unused","Datos Flatpak sin Usar","Runtimes y datos Flatpak no usados",
            "/var/lib/flatpak",        "Flatpak",  0, true,  true, false
        },
        {
            "orphan_pkgs",  "Paquetes Huérfanos", "Dependencias instaladas que ya no se usan",
            "",                        "Paquetes", 0, false, true, false
        }
    };

    // Disable entries not relevant for detected PM
    for (auto &e : m_entries) {
        if (e.id == "apt_cache"    && m_pm != SystemInfo::APT)    e.selected = false;
        if (e.id == "dnf_cache"    && m_pm != SystemInfo::DNF)    e.selected = false;
        if (e.id == "pacman_cache" && m_pm != SystemInfo::PACMAN) e.selected = false;
    }
}

void PackageCleaner::analyze() {
    m_analyzing = true;
    emit analyzeStarted();

    int total = m_entries.size();
    for (int i = 0; i < total; ++i) {
        auto &e = m_entries[i];
        emit analyzeProgress((i * 100) / total, e.name);

        if (e.id == "apt_cache") {
            e.sizeBytes = FileUtils::dirSize("/var/cache/apt/archives");
        } else if (e.id == "dnf_cache") {
            e.sizeBytes = FileUtils::dirSize("/var/cache/dnf");
        } else if (e.id == "pacman_cache") {
            e.sizeBytes = FileUtils::dirSize("/var/cache/pacman/pkg");
        } else if (e.id == "snap_old") {
            e.sizeBytes = getSnapRevisionWaste();
        } else if (e.id == "flatpak_unused") {
            e.sizeBytes = FileUtils::dirSize("/var/lib/flatpak/runtime");
        } else if (e.id == "orphan_pkgs") {
            auto orphans = orphanedPackages();
            e.description = QString("%1 paquetes huérfanos encontrados").arg(orphans.size());
            e.sizeBytes = 0; // Hard to calculate without removal
        }
        e.analyzed = true;
    }

    m_analyzing = false;
    emit analyzeFinished(totalSize());
}

qint64 PackageCleaner::clean(const QList<CleanEntry> &selected) {
    m_cleaning = true;
    emit cleanStarted();
    qint64 freed = 0;
    int total = selected.size();

    for (int i = 0; i < total; ++i) {
        const auto &e = selected[i];
        emit cleanProgress((i * 100) / total, e.name);
        emitLog(QString("[Package] Cleaning: %1").arg(e.name));

        qint64 b = 0;
        if (e.id == "apt_cache") {
            SudoHelper::instance().cleanAptCache(&b);
            freed += b;
        } else if (e.id == "dnf_cache") {
            SudoHelper::instance().cleanDnfCache(&b);
            freed += b;
        } else if (e.id == "pacman_cache") {
            SudoHelper::instance().cleanPacmanCache(&b);
            freed += b;
        } else if (e.id == "snap_old") {
            // Remove old snap revisions
            QProcess p;
            p.start("bash", {"-c",
                "snap list --all | awk '/disabled/{print $1, $3}' | "
                "while read snapname revision; do "
                "  sudo snap remove \"$snapname\" --revision=\"$revision\"; "
                "done"
            });
            p.waitForFinished(60000);
            freed += e.sizeBytes;
        } else if (e.id == "flatpak_unused") {
            SudoHelper::instance().runWithSudo("flatpak", {"uninstall", "--unused", "-y"});
            freed += e.sizeBytes;
        } else if (e.id == "orphan_pkgs") {
            auto orphans = orphanedPackages();
            if (!orphans.isEmpty()) {
                removePackages(orphans);
            }
        }
    }

    m_cleaning = false;
    emit cleanFinished(freed);
    return freed;
}

QStringList PackageCleaner::orphanedPackages() {
    switch (m_pm) {
        case SystemInfo::APT:    return getAptOrphans();
        case SystemInfo::PACMAN: return getPacmanOrphans();
        default: return {};
    }
}

QStringList PackageCleaner::getAptOrphans() {
    QProcess p;
    // Use deborphan if available
    p.start("which", {"deborphan"});
    p.waitForFinished(2000);
    if (p.exitCode() == 0) {
        p.start("deborphan");
        p.waitForFinished(10000);
        QString out = p.readAllStandardOutput().trimmed();
        if (out.isEmpty()) return {};
        return out.split('\n');
    }
    // Fallback: apt-get autoremove --dry-run
    p.start("apt-get", {"autoremove", "--dry-run"});
    p.waitForFinished(10000);
    QStringList pkgs;
    QString out = p.readAllStandardOutput();
    for (auto &line : out.split('\n')) {
        if (line.trimmed().startsWith("Remv ")) {
            pkgs << line.split(' ')[1].trimmed();
        }
    }
    return pkgs;
}

QStringList PackageCleaner::getPacmanOrphans() {
    QProcess p;
    p.start("pacman", {"-Qtdq"});
    p.waitForFinished(10000);
    QString out = p.readAllStandardOutput().trimmed();
    if (out.isEmpty()) return {};
    return out.split('\n');
}

QStringList PackageCleaner::getFlatpakUnused() {
    QProcess p;
    p.start("flatpak", {"list", "--unused"});
    p.waitForFinished(10000);
    return QString::fromUtf8(p.readAllStandardOutput()).trimmed().split('\n');
}

qint64 PackageCleaner::getSnapRevisionWaste() {
    QProcess p;
    p.start("bash", {"-c", "snap list --all | grep disabled"});
    p.waitForFinished(5000);
    QString out = p.readAllStandardOutput();
    int count = out.split('\n').size();
    // Estimate ~100MB per disabled snap revision
    return (qint64)count * 100 * 1024 * 1024;
}

bool PackageCleaner::removePackages(const QStringList &pkgs) {
    if (pkgs.isEmpty()) return true;
    QStringList args;
    switch (m_pm) {
        case SystemInfo::APT:
            args << "apt-get" << "autoremove" << "-y";
            break;
        case SystemInfo::DNF:
            args = {"dnf", "autoremove", "-y"};
            break;
        case SystemInfo::PACMAN:
            args = {"pacman", "-Rns", "--noconfirm"};
            args << pkgs;
            break;
        default: return false;
    }
    return SudoHelper::instance().runWithSudo(args[0], args.mid(1)) == 0;
}
