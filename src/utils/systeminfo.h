#pragma once
#include <QString>
#include <QMap>

struct DiskInfo {
    QString device;
    QString mountPoint;
    QString filesystem;
    qint64  total    = 0;
    qint64  used     = 0;
    qint64  available = 0;
    float   usePercent = 0.0f;
};

struct MemInfo {
    qint64 total    = 0;
    qint64 used     = 0;
    qint64 free     = 0;
    qint64 cached   = 0;
    qint64 buffers  = 0;
    qint64 swapTotal = 0;
    qint64 swapUsed  = 0;
};

struct CpuInfo {
    float usagePercent = 0.0f;
    int   cores        = 0;
    QString model;
};

class SystemInfo {
public:
    static SystemInfo &instance();

    // Distribution info
    QString distroName();
    QString distroVersion();
    QString kernelVersion();
    QString hostname();
    QString username();

    // Hardware
    DiskInfo diskInfo(const QString &mountPoint = "/");
    QList<DiskInfo> allDisks();
    MemInfo memInfo();
    CpuInfo cpuInfo();

    // Package manager detection
    enum PackageManager { APT, DNF, PACMAN, ZYPPER, UNKNOWN };
    PackageManager detectPackageManager();
    QString packageManagerName();

    // System
    bool isRoot();
    bool hasSudo();
    QString homeDir();
    QMap<QString, QString> installedBrowsers();

    // Uptime
    QString uptime();

private:
    SystemInfo() = default;
    QString parseOsRelease(const QString &key);
};
