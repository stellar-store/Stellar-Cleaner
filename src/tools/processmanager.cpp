#include "processmanager.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <csignal>
#include <QRegularExpression>

ProcessManager::ProcessManager(QObject *parent) : QObject(parent) {}

QList<ProcessInfo> ProcessManager::processes() {
    QList<ProcessInfo> result;
    QProcess p;
    p.start("ps", {"-eo", "pid,user,stat,pcpu,rss,comm,args", "--sort=-rss"});
    p.waitForFinished(5000);

    bool first = true;
    for (const auto &line : p.readAllStandardOutput().split('\n')) {
        if (first) { first = false; continue; } // skip header
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) continue;

        QStringList parts = trimmed.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() < 6) continue;

        ProcessInfo pi;
        pi.pid        = parts[0].toInt();
        pi.user       = parts[1];
        pi.status     = parts[2];
        pi.cpuPercent = parts[3].toFloat();
        pi.memBytes   = parts[4].toLongLong() * 1024; // rss is in KB
        pi.name       = parts[5];
        pi.command    = parts.mid(6).join(' ');
        result << pi;
    }
    return result;
}

bool ProcessManager::killProcess(int pid) {
    return ::kill(pid, SIGTERM) == 0;
}

bool ProcessManager::killProcessByName(const QString &name) {
    QProcess p;
    p.start("pkill", {"-f", name});
    p.waitForFinished(3000);
    return p.exitCode() == 0;
}

void ProcessManager::refresh() {
    m_procs = processes();
    emit refreshed(m_procs);
}
