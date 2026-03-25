#pragma once
#include "basecleaner.h"
#include <QProcess>
#include <QMap>

class KernelCleaner : public BaseCleaner {
    Q_OBJECT
public:
    explicit KernelCleaner(QObject *parent = nullptr);

    QString name()        const override { return "Kernels Antiguos"; }
    QString description() const override { return "Elimina kernels del sistema no utilizados para liberar espacio en /boot"; }

    QList<CleanEntry> entries() override { return m_entries; }
    void analyze()  override;
    qint64 clean(const QList<CleanEntry> &selected) override;

private:
    struct KernelInfo {
        QString version;
        QString packageName;
        qint64 size = 0;
    };

    QString getCurrentKernel();
    QList<KernelInfo> getInstalledKernels();
    qint64 getPackageSize(const QString &packageName);
    void setupEntries();

    QString m_currentKernel;
    QMap<QString, KernelInfo> m_kernelInfoMap;
};
