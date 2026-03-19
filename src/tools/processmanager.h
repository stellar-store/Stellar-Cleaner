#pragma once
#include <QObject>
#include <QList>
#include <QString>

struct ProcessInfo {
    int     pid     = 0;
    QString name;
    QString user;
    QString status;
    float   cpuPercent = 0.0f;
    qint64  memBytes   = 0;
    QString command;
};

class ProcessManager : public QObject {
    Q_OBJECT
public:
    explicit ProcessManager(QObject *parent = nullptr);

    QList<ProcessInfo> processes();
    bool killProcess(int pid);
    bool killProcessByName(const QString &name);
    void refresh();

signals:
    void refreshed(const QList<ProcessInfo> &procs);

private:
    QList<ProcessInfo> m_procs;
    ProcessInfo parseStatFile(int pid);
};
