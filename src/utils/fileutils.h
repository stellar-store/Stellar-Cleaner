#pragma once
#include <QString>
#include <QStringList>
#include <QDateTime>

namespace FileUtils {

    // Size calculations
    qint64 dirSize(const QString &path);
    qint64 fileSize(const QString &path);
    QString formatSize(qint64 bytes, int decimals = 2);

    // File operations (returns bytes freed)
    qint64 deleteFile(const QString &path);
    qint64 deleteDir(const QString &path, bool recursive = true, bool removeRoot = false);
    qint64 deleteFilesInDir(const QString &path, const QStringList &patterns = {});
    qint64 deleteOlderThan(const QString &path, int days);

    // Checks
    bool pathExists(const QString &path);
    bool isAccessible(const QString &path);
    bool isSystemPath(const QString &path);  // Safety check

    // Listing
    QStringList findFiles(const QString &path, const QStringList &patterns, bool recursive = true);
    QStringList findDirs(const QString &path, bool recursive = false);
    QStringList getBrowserProfiles(const QString &basePath);

    // Date helpers
    int daysSinceModified(const QString &path);

} // namespace FileUtils
