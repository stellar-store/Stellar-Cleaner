#pragma once
#include <QObject>
#include <QList>
#include <QString>

struct InstalledApp {
    QString name;
    QString version;
    QString size;
    QString description;
    QString packageManager; // apt, snap, flatpak
    QString packageName;    // internal package id
    QString icon;
};

class UninstallManager : public QObject {
    Q_OBJECT
public:
    explicit UninstallManager(QObject *parent = nullptr);

    QList<InstalledApp> installedApps();
    bool uninstall(const InstalledApp &app);

    void refresh();

signals:
    void refreshed(const QList<InstalledApp> &apps);
    void uninstallFinished(bool success, const QString &appName);

private:
    QList<InstalledApp> m_apps;
    QList<InstalledApp> getAptApps();
    QList<InstalledApp> getSnapApps();
    QList<InstalledApp> getFlatpakApps();
};
