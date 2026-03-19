#include "sudohelper.h"
#include "fileutils.h"
#include <QProcess>
#include <QDir>
#include <QDirIterator>
#include <unistd.h>

SudoHelper &SudoHelper::instance() {
    static SudoHelper inst;
    return inst;
}

bool SudoHelper::isRoot() {
    return geteuid() == 0;
}

bool SudoHelper::isAvailable() {
    if (isRoot()) return true;
    QProcess p;
    p.start("sudo", {"-n", "true"});
    p.waitForFinished(3000);
    return p.exitCode() == 0;
}

int SudoHelper::runWithSudo(const QString &command, const QStringList &args, QString *output) {
    QProcess p;
    QStringList fullArgs;

    if (!isRoot()) {
        fullArgs << "-n" << command << args;
        p.start("sudo", fullArgs);
    } else {
        p.start(command, args);
    }

    p.waitForFinished(30000);
    if (output) *output = p.readAllStandardOutput().trimmed();
    return p.exitCode();
}

bool SudoHelper::cleanLogDir(const QString &path, qint64 *bytesFreed) {
    if (isRoot()) {
        qint64 freed = FileUtils::deleteFilesInDir(path, {"*.log", "*.log.*", "*.gz", "*.bz2"});
        if (bytesFreed) *bytesFreed = freed;
        return true;
    }

    // Use sudo find+rm
    QString output;
    int rc = runWithSudo("find",
        {path, "-name", "*.log*", "-o", "-name", "*.gz", "-o", "-name", "*.bz2",
         "-exec", "rm", "-f", "{}", ";"}, &output);
    if (bytesFreed) *bytesFreed = 0; // Can't easily measure via sudo
    return rc == 0;
}

bool SudoHelper::cleanAptCache(qint64 *bytesFreed) {
    if (bytesFreed) {
        qint64 sz = FileUtils::dirSize("/var/cache/apt/archives");
        *bytesFreed = sz;
    }
    QString out;
    int rc = runWithSudo("apt-get", {"clean", "-y"}, &out);
    return rc == 0;
}

bool SudoHelper::cleanDnfCache(qint64 *bytesFreed) {
    if (bytesFreed) {
        qint64 sz = FileUtils::dirSize("/var/cache/dnf");
        *bytesFreed = sz;
    }
    QString out;
    int rc = runWithSudo("dnf", {"clean", "all"}, &out);
    return rc == 0;
}

bool SudoHelper::cleanPacmanCache(qint64 *bytesFreed) {
    if (bytesFreed) {
        qint64 sz = FileUtils::dirSize("/var/cache/pacman/pkg");
        *bytesFreed = sz;
    }
    QString out;
    int rc = runWithSudo("pacman", {"-Sc", "--noconfirm"}, &out);
    return rc == 0;
}

bool SudoHelper::runJournalClean(int days, qint64 *bytesFreed) {
    QString vacuumArg = QString("--vacuum-time=%1d").arg(days);
    QString out;
    int rc = runWithSudo("journalctl", {"--rotate"}, &out);
    rc += runWithSudo("journalctl", {vacuumArg}, &out);
    if (bytesFreed) *bytesFreed = 0;
    return rc == 0;
}

bool SudoHelper::rotateAndVacuumLogs(qint64 *bytesFreed) {
    // Calculate size before
    qint64 before = FileUtils::dirSize("/var/log");
    QString out;
    runWithSudo("logrotate", {"-f", "/etc/logrotate.conf"}, &out);
    qint64 after = FileUtils::dirSize("/var/log");
    if (bytesFreed) *bytesFreed = qMax(0LL, before - after);
    return true;
}
