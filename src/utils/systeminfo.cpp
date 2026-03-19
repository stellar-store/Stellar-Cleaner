#include "systeminfo.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QThread>
#include <sys/statvfs.h>
#include <unistd.h>

SystemInfo &SystemInfo::instance() {
    static SystemInfo inst;
    return inst;
}

QString SystemInfo::parseOsRelease(const QString &key) {
    QFile f("/etc/os-release");
    if (!f.open(QIODevice::ReadOnly)) return {};
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith(key + "=")) {
            QString val = line.mid(key.length() + 1);
            val.remove('"');
            return val;
        }
    }
    return {};
}

QString SystemInfo::distroName() {
    QString name = parseOsRelease("NAME");
    return name.isEmpty() ? "Linux" : name;
}

QString SystemInfo::distroVersion() {
    return parseOsRelease("VERSION_ID");
}

QString SystemInfo::kernelVersion() {
    QProcess p;
    p.start("uname", {"-r"});
    p.waitForFinished(2000);
    return p.readAllStandardOutput().trimmed();
}

QString SystemInfo::hostname() {
    QProcess p;
    p.start("hostname", {});
    p.waitForFinished(1000);
    return p.readAllStandardOutput().trimmed();
}

QString SystemInfo::username() {
    return qgetenv("USER");
}

DiskInfo SystemInfo::diskInfo(const QString &mountPoint) {
    DiskInfo info;
    info.mountPoint = mountPoint;

    struct statvfs stat;
    if (statvfs(mountPoint.toLocal8Bit().constData(), &stat) == 0) {
        info.total     = (qint64)stat.f_blocks * stat.f_frsize;
        info.available = (qint64)stat.f_bavail  * stat.f_frsize;
        info.used      = info.total - (qint64)stat.f_bfree * stat.f_frsize;
        info.usePercent = info.total > 0 ? (float)info.used / info.total * 100.0f : 0;
    }
    return info;
}

QList<DiskInfo> SystemInfo::allDisks() {
    QList<DiskInfo> disks;
    QFile f("/proc/mounts");
    if (!f.open(QIODevice::ReadOnly)) return disks;

    QTextStream in(&f);
    while (!in.atEnd()) {
        QStringList parts = in.readLine().split(' ');
        if (parts.size() < 3) continue;
        QString dev = parts[0];
        QString mp  = parts[1];
        QString fs  = parts[2];

        // Only real filesystems
        if (!dev.startsWith("/dev/")) continue;
        if (fs == "tmpfs" || fs == "devtmpfs" || fs == "proc") continue;

        DiskInfo d = diskInfo(mp);
        d.device     = dev;
        d.filesystem = fs;
        if (d.total > 0) disks << d;
    }
    return disks;
}

MemInfo SystemInfo::memInfo() {
    MemInfo info;
    QFile f("/proc/meminfo");
    if (!f.open(QIODevice::ReadOnly)) return info;

    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine();
        auto parts = line.split(QRegularExpression("\\s+"));
        if (parts.size() < 2) continue;
        qint64 kb = parts[1].toLongLong() * 1024;
        QString key = parts[0];
        if      (key == "MemTotal:")    info.total    = kb;
        else if (key == "MemFree:")     info.free     = kb;
        else if (key == "Cached:")      info.cached   = kb;
        else if (key == "Buffers:")     info.buffers  = kb;
        else if (key == "SwapTotal:")   info.swapTotal = kb;
        else if (key == "SwapFree:")    info.swapUsed  = info.swapTotal - kb;
    }
    info.used = info.total - info.free - info.cached - info.buffers;
    return info;
}

CpuInfo SystemInfo::cpuInfo() {
    CpuInfo info;

    // Read /proc/cpuinfo for model and cores
    QFile f("/proc/cpuinfo");
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream in(&f);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("model name")) {
                info.model = line.split(":").last().trimmed();
            }
            if (line.startsWith("processor")) {
                info.cores++;
            }
        }
    }

    // CPU usage via /proc/stat
    auto readStat = [&]() -> QPair<qint64,qint64> {
        QFile sf("/proc/stat");
        if (!sf.open(QIODevice::ReadOnly)) return {0,0};
        QString line = sf.readLine();
        QStringList p = line.split(QRegularExpression("\\s+"));
        qint64 idle   = p.size() > 4 ? p[4].toLongLong() + p[5].toLongLong() : 0;
        qint64 total  = 0;
        for (int i=1; i<p.size(); i++) total += p[i].toLongLong();
        return {idle, total};
    };

    auto s1 = readStat();
    QThread::msleep(100);
    auto s2 = readStat();

    qint64 dIdle  = s2.first  - s1.first;
    qint64 dTotal = s2.second - s1.second;
    info.usagePercent = dTotal > 0 ? (1.0f - (float)dIdle/dTotal) * 100.0f : 0;

    return info;
}

SystemInfo::PackageManager SystemInfo::detectPackageManager() {
    if (QFile::exists("/usr/bin/apt-get") || QFile::exists("/usr/bin/apt"))
        return APT;
    if (QFile::exists("/usr/bin/dnf"))   return DNF;
    if (QFile::exists("/usr/bin/pacman")) return PACMAN;
    if (QFile::exists("/usr/bin/zypper")) return ZYPPER;
    return UNKNOWN;
}

QString SystemInfo::packageManagerName() {
    switch (detectPackageManager()) {
        case APT:    return "APT (Debian/Ubuntu)";
        case DNF:    return "DNF (Fedora/RHEL)";
        case PACMAN: return "Pacman (Arch)";
        case ZYPPER: return "Zypper (openSUSE)";
        default:     return "Unknown";
    }
}

bool SystemInfo::isRoot() {
    return geteuid() == 0;
}

bool SystemInfo::hasSudo() {
    QProcess p;
    p.start("sudo", {"-n", "true"});
    p.waitForFinished(3000);
    return p.exitCode() == 0;
}

QString SystemInfo::homeDir() {
    return QDir::homePath();
}

QMap<QString, QString> SystemInfo::installedBrowsers() {
    QMap<QString,QString> browsers;
    QStringList paths = {
        "/usr/bin/firefox", "/usr/bin/firefox-esr",
        "/usr/bin/google-chrome", "/usr/bin/google-chrome-stable",
        "/usr/bin/chromium", "/usr/bin/chromium-browser",
        "/usr/bin/brave-browser",
        "/usr/bin/opera",
        "/usr/bin/vivaldi",
        "/usr/bin/microsoft-edge", "/usr/bin/microsoft-edge-stable",
        "/opt/google/chrome/google-chrome",
        "/snap/bin/firefox", "/snap/bin/chromium"
    };

    QMap<QString,QString> labels = {
        {"/usr/bin/firefox",         "Firefox"},
        {"/usr/bin/firefox-esr",     "Firefox ESR"},
        {"/usr/bin/google-chrome",   "Google Chrome"},
        {"/usr/bin/google-chrome-stable", "Google Chrome"},
        {"/usr/bin/chromium",        "Chromium"},
        {"/usr/bin/chromium-browser","Chromium"},
        {"/usr/bin/brave-browser",   "Brave Browser"},
        {"/usr/bin/opera",           "Opera"},
        {"/usr/bin/vivaldi",         "Vivaldi"},
        {"/usr/bin/microsoft-edge",  "Microsoft Edge"},
        {"/usr/bin/microsoft-edge-stable", "Microsoft Edge"},
        {"/opt/google/chrome/google-chrome", "Google Chrome"},
        {"/snap/bin/firefox",        "Firefox (Snap)"},
        {"/snap/bin/chromium",       "Chromium (Snap)"}
    };

    for (const auto &p : paths) {
        if (QFile::exists(p)) {
            browsers[p] = labels.value(p, "Browser");
        }
    }
    return browsers;
}

QString SystemInfo::uptime() {
    QFile f("/proc/uptime");
    if (!f.open(QIODevice::ReadOnly)) return "Unknown";
    double seconds = f.readAll().split(' ')[0].toDouble();
    int days  = (int)seconds / 86400;
    int hours = ((int)seconds % 86400) / 3600;
    int mins  = ((int)seconds % 3600) / 60;

    if (days > 0)  return QString("%1d %2h %3m").arg(days).arg(hours).arg(mins);
    if (hours > 0) return QString("%1h %2m").arg(hours).arg(mins);
    return QString("%1m").arg(mins);
}
