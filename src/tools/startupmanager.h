#pragma once
#include <QObject>
#include <QString>
#include <QList>

struct StartupEntry {
    QString name;
    QString command;
    QString path;          // .desktop file path or service unit
    QString description;
    bool    enabled  = true;
    bool    isUser   = true;  // false = system service
    QString type;            // "autostart", "systemd", "cron"
    QString icon;
};

class StartupManager : public QObject {
    Q_OBJECT
public:
    explicit StartupManager(QObject *parent = nullptr);

    QList<StartupEntry> autostartApps();
    QList<StartupEntry> systemdServices();
    QList<StartupEntry> userServices();

    bool setEnabled(const StartupEntry &entry, bool enabled);
    bool removeEntry(const StartupEntry &entry);

    void refresh();

signals:
    void refreshed();

private:
    QList<StartupEntry> m_entries;
    QList<StartupEntry> parseAutostartDir(const QString &path, bool isUser);
    QList<StartupEntry> parseSystemdUnits(bool user);
};
