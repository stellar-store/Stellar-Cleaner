#include "uninstallmanager.h"
#include "../utils/sudohelper.h"
#include <QProcess>
#include <QRegularExpression>

UninstallManager::UninstallManager(QObject *parent) : QObject(parent) {}

QList<InstalledApp> UninstallManager::installedApps() {
    QList<InstalledApp> all;
    all << getAptApps();
    all << getSnapApps();
    all << getFlatpakApps();
    return all;
}

QList<InstalledApp> UninstallManager::getAptApps() {
    QList<InstalledApp> apps;
    QProcess p;
    p.start("dpkg-query", {"-W", "-f=${Package}|${Version}|${Installed-Size}|${binary:Summary}\n"});
    p.waitForFinished(15000);

    for (const auto &line : p.readAllStandardOutput().split('\n')) {
        QStringList parts = QString(line).split('|');
        if (parts.size() < 4) continue;
        InstalledApp a;
        a.packageManager = "apt";
        a.packageName    = parts[0].trimmed();
        a.name           = parts[0].trimmed();
        a.version        = parts[1].trimmed();
        qint64 kb = parts[2].trimmed().toLongLong();
        a.size = kb > 1024 ? QString("%1 MB").arg(kb/1024) : QString("%1 KB").arg(kb);
        a.description    = parts[3].trimmed();
        if (!a.name.isEmpty()) apps << a;
    }
    return apps;
}

QList<InstalledApp> UninstallManager::getSnapApps() {
    QList<InstalledApp> apps;
    QProcess p;
    p.start("snap", {"list"});
    p.waitForFinished(10000);

    bool first = true;
    for (const auto &line : p.readAllStandardOutput().split('\n')) {
        if (first) { first = false; continue; }
        QStringList parts = QString(line).trimmed().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() < 3) continue;
        InstalledApp a;
        a.packageManager = "snap";
        a.name           = parts[0];
        a.packageName    = parts[0];
        a.version        = parts[1];
        a.description    = parts.mid(4).join(' ');
        apps << a;
    }
    return apps;
}

QList<InstalledApp> UninstallManager::getFlatpakApps() {
    QList<InstalledApp> apps;
    QProcess p;
    p.start("flatpak", {"list", "--columns=name,application,version,size:app"});
    p.waitForFinished(10000);

    bool first = true;
    for (const auto &line : p.readAllStandardOutput().split('\n')) {
        if (first) { first = false; continue; }
        QStringList parts = QString(line).split('\t');
        if (parts.size() < 2) continue;
        InstalledApp a;
        a.packageManager = "flatpak";
        a.name           = parts[0].trimmed();
        a.packageName    = parts.size() > 1 ? parts[1].trimmed() : a.name;
        a.version        = parts.size() > 2 ? parts[2].trimmed() : "";
        a.size           = parts.size() > 3 ? parts[3].trimmed() : "";
        apps << a;
    }
    return apps;
}

bool UninstallManager::uninstall(const InstalledApp &app) {
    int rc = 1;
    if (app.packageManager == "apt") {
        rc = SudoHelper::instance().runWithSudo("apt-get",
            {"remove", "--purge", "-y", app.packageName});
    } else if (app.packageManager == "snap") {
        rc = SudoHelper::instance().runWithSudo("snap",
            {"remove", app.packageName});
    } else if (app.packageManager == "flatpak") {
        rc = SudoHelper::instance().runWithSudo("flatpak",
            {"uninstall", "-y", app.packageName});
    }
    emit uninstallFinished(rc == 0, app.name);
    return rc == 0;
}

void UninstallManager::refresh() {
    m_apps = installedApps();
    emit refreshed(m_apps);
}
