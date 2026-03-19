#pragma once
#include <QString>
#include <QStringList>
#include <QObject>

class SudoHelper : public QObject {
    Q_OBJECT
public:
    static SudoHelper &instance();

    bool isAvailable();
    bool isRoot();

    // Run a command with sudo, returns exit code
    int runWithSudo(const QString &command, const QStringList &args, QString *output = nullptr);

    // Specific privileged operations
    bool cleanLogDir(const QString &path, qint64 *bytesFreed = nullptr);
    bool cleanAptCache(qint64 *bytesFreed = nullptr);
    bool cleanDnfCache(qint64 *bytesFreed = nullptr);
    bool cleanPacmanCache(qint64 *bytesFreed = nullptr);
    bool runJournalClean(int days, qint64 *bytesFreed = nullptr);
    bool rotateAndVacuumLogs(qint64 *bytesFreed = nullptr);

signals:
    void sudoRequired();
    void operationCompleted(const QString &op, qint64 bytesFreed);

private:
    SudoHelper() = default;
    QString m_password; // Only stored briefly, cleared after use
};
