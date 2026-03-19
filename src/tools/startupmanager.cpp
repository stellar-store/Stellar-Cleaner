#include "startupmanager.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QSettings>
#include <QRegularExpression>

StartupManager::StartupManager(QObject *parent) : QObject(parent) {
    refresh();
}

void StartupManager::refresh() {
    m_entries.clear();
    emit refreshed();
}

QList<StartupEntry> StartupManager::autostartApps() {
    QList<StartupEntry> result;
    QString home = QDir::homePath();

    QStringList dirs = {
        home + "/.config/autostart",
        "/etc/xdg/autostart"
    };

    for (const auto &dir : dirs) {
        bool isUser = dir.startsWith(home);
        result << parseAutostartDir(dir, isUser);
    }
    return result;
}

QList<StartupEntry> StartupManager::parseAutostartDir(const QString &path, bool isUser) {
    QList<StartupEntry> result;
    QDir dir(path);
    if (!dir.exists()) return result;

    for (const auto &fi : dir.entryInfoList({"*.desktop"}, QDir::Files)) {
        QSettings s(fi.absoluteFilePath(), QSettings::IniFormat);
        s.beginGroup("Desktop Entry");

        StartupEntry e;
        e.name        = s.value("Name").toString();
        e.command     = s.value("Exec").toString();
        e.description = s.value("Comment").toString();
        e.icon        = s.value("Icon").toString();
        e.path        = fi.absoluteFilePath();
        e.isUser      = isUser;
        e.type        = "autostart";

        QString hidden = s.value("Hidden", "false").toString();
        QString noDisplay = s.value("NoDisplay", "false").toString();
        e.enabled = (hidden != "true" && noDisplay != "true");

        if (!e.name.isEmpty()) result << e;
        s.endGroup();
    }
    return result;
}

QList<StartupEntry> StartupManager::userServices() {
    return parseSystemdUnits(true);
}

QList<StartupEntry> StartupManager::systemdServices() {
    return parseSystemdUnits(false);
}

QList<StartupEntry> StartupManager::parseSystemdUnits(bool user) {
    QList<StartupEntry> result;
    QProcess p;

    QStringList args = {"list-units", "--type=service",
                        "--all", "--plain", "--no-legend"};
    if (user) args.prepend("--user");

    p.start("systemctl", args);
    p.waitForFinished(10000);

    for (const auto &line : p.readAllStandardOutput().split('\n')) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) continue;

        QStringList parts = trimmed.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() < 4) continue;

        StartupEntry e;
        e.name    = parts[0];
        e.enabled = (parts[3] == "running" || parts[2] == "active");
        e.isUser  = user;
        e.type    = "systemd";
        e.command = parts[0];
        e.description = parts.mid(4).join(' ');
        result << e;
    }
    return result;
}

bool StartupManager::setEnabled(const StartupEntry &entry, bool enabled) {
    if (entry.type == "autostart") {
        QSettings s(entry.path, QSettings::IniFormat);
        s.beginGroup("Desktop Entry");
        s.setValue("Hidden", enabled ? "false" : "true");
        s.endGroup();
        return true;
    }
    if (entry.type == "systemd") {
        QProcess p;
        QStringList args;
        if (entry.isUser) args << "--user";
        args << (enabled ? "enable" : "disable") << entry.name;
        p.start("systemctl", args);
        p.waitForFinished(5000);
        return p.exitCode() == 0;
    }
    return false;
}

bool StartupManager::removeEntry(const StartupEntry &entry) {
    if (entry.type == "autostart" && entry.isUser) {
        return QFile::remove(entry.path);
    }
    return false;
}
