#include "fileutils.h"
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QStringList>

namespace FileUtils {

qint64 dirSize(const QString &path) {
    qint64 total = 0;
    QFileInfo info(path);
    if (!info.exists()) return 0;
    if (info.isFile()) return info.size();

    QDirIterator it(path,
        QDir::Files | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot,
        QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

    while (it.hasNext()) {
        it.next();
        total += it.fileInfo().size();
    }
    return total;
}

qint64 fileSize(const QString &path) {
    return QFileInfo(path).size();
}

QString formatSize(qint64 bytes, int decimals) {
    if (bytes < 0) bytes = 0;
    if (bytes < 1024LL)
        return QString("%1 B").arg(bytes);
    if (bytes < 1024LL * 1024)
        return QString("%1 KB").arg(QString::number(bytes / 1024.0, 'f', decimals));
    if (bytes < 1024LL * 1024 * 1024)
        return QString("%1 MB").arg(QString::number(bytes / (1024.0 * 1024), 'f', decimals));
    return QString("%1 GB").arg(QString::number(bytes / (1024.0 * 1024 * 1024), 'f', decimals));
}

qint64 deleteFile(const QString &path) {
    QFileInfo fi(path);
    if (!fi.exists()) return 0;
    qint64 sz = fi.size();
    if (QFile::remove(path)) return sz;
    return 0;
}

qint64 deleteDir(const QString &path, bool recursive, bool removeRoot) {
    if (!pathExists(path)) return 0;
    if (isSystemPath(path)) return 0;

    qint64 freed = 0;
    QDir dir(path);

    if (recursive) {
        QDirIterator it(path,
            QDir::Files | QDir::Hidden | QDir::System | QDir::NoDotAndDotDot,
            QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            freed += it.fileInfo().size();
            QFile::remove(it.filePath());
        }
        // Remove empty subdirs
        QDirIterator dit(path,
            QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot,
            QDirIterator::Subdirectories);
        QStringList dirs;
        while (dit.hasNext()) { dit.next(); dirs << dit.filePath(); }
        // Remove deepest first
        std::sort(dirs.begin(), dirs.end(), [](const QString &a, const QString &b){
            return a.count('/') > b.count('/');
        });
        for (const auto &d : dirs) { QDir(d).rmdir("."); }
    } else {
        const auto entries = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot);
        for (const auto &e : entries) {
            freed += e.size();
            QFile::remove(e.absoluteFilePath());
        }
    }
    if (removeRoot) dir.rmdir(".");
    return freed;
}

qint64 deleteFilesInDir(const QString &path, const QStringList &patterns) {
    if (!pathExists(path)) return 0;
    if (isSystemPath(path)) return 0;

    qint64 freed = 0;
    QDir dir(path);
    QStringList nameFilters = patterns.isEmpty() ? QStringList{"*"} : patterns;

    QDirIterator it(path, nameFilters,
        QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot,
        QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        freed += it.fileInfo().size();
        QFile::remove(it.filePath());
    }
    return freed;
}

qint64 deleteOlderThan(const QString &path, int days) {
    if (!pathExists(path)) return 0;
    if (isSystemPath(path)) return 0;

    qint64 freed = 0;
    QDateTime cutoff = QDateTime::currentDateTime().addDays(-days);

    QDirIterator it(path,
        QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot,
        QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        QFileInfo fi = it.fileInfo();
        if (fi.lastModified() < cutoff) {
            freed += fi.size();
            QFile::remove(it.filePath());
        }
    }
    return freed;
}

bool pathExists(const QString &path) {
    return QFileInfo::exists(path);
}

bool isAccessible(const QString &path) {
    QFileInfo fi(path);
    return fi.exists() && fi.isReadable();
}

bool isSystemPath(const QString &path) {
    // Critical system directories that should NEVER be touched
    static const QStringList forbidden = {
        "/", "/bin", "/sbin", "/usr/bin", "/usr/sbin",
        "/lib", "/lib64", "/usr/lib", "/boot",
        "/sys", "/proc", "/dev", "/run",
        "/etc", "/root"
    };
    for (const auto &f : forbidden) {
        if (path == f || path.startsWith(f + "/")) {
            // Only allow sub-paths like /var/log, /var/cache/apt
            if (path.startsWith("/var/log") ||
                path.startsWith("/var/cache/apt") ||
                path.startsWith("/var/tmp")) {
                return false;
            }
            if (f == "/" || f == "/bin" || f == "/sbin" ||
                f == "/usr/bin" || f == "/usr/sbin" ||
                f == "/lib" || f == "/lib64" || f == "/usr/lib" ||
                f == "/boot" || f == "/sys" || f == "/proc" ||
                f == "/dev" || f == "/run" || f == "/etc" || f == "/root") {
                return true;
            }
        }
    }
    return false;
}

QStringList findFiles(const QString &path, const QStringList &patterns, bool recursive) {
    QStringList result;
    if (!pathExists(path)) return result;

    QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags;
    if (recursive) flags = QDirIterator::Subdirectories;

    QDirIterator it(path,
        patterns.isEmpty() ? QStringList{"*"} : patterns,
        QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot,
        flags);

    while (it.hasNext()) {
        it.next();
        result << it.filePath();
    }
    return result;
}

QStringList findDirs(const QString &path, bool recursive) {
    QStringList result;
    if (!pathExists(path)) return result;

    QDirIterator::IteratorFlags flags = QDirIterator::NoIteratorFlags;
    if (recursive) flags = QDirIterator::Subdirectories;

    QDirIterator it(path, QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot, flags);
    while (it.hasNext()) { it.next(); result << it.filePath(); }
    return result;
}

QStringList getBrowserProfiles(const QString &basePath) {
    QStringList profiles;
    if (!pathExists(basePath)) return profiles;

    QDir dir(basePath);
    const auto entries = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const auto &e : entries) {
        // Firefox profile pattern: xxxxxxxx.default or xxxxxxxx.default-release
        if (e.fileName().contains(".default") || e.fileName().contains("Profile")) {
            profiles << e.absoluteFilePath();
        }
    }
    if (profiles.isEmpty()) profiles << basePath; // Chrome/Brave flat structure
    return profiles;
}

int daysSinceModified(const QString &path) {
    QFileInfo fi(path);
    if (!fi.exists()) return -1;
    return static_cast<int>(fi.lastModified().daysTo(QDateTime::currentDateTime()));
}

} // namespace FileUtils
