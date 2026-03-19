#pragma once
#include "basecleaner.h"
#include "../utils/systeminfo.h"

class PackageCleaner : public BaseCleaner {
    Q_OBJECT
public:
    explicit PackageCleaner(QObject *parent = nullptr);

    QString name()        const override { return "Paquetes del Sistema"; }
    QString description() const override { return "Limpia caché de paquetes y dependencias huérfanas"; }

    QList<CleanEntry> entries() override { return m_entries; }
    void analyze()  override;
    qint64 clean(const QList<CleanEntry> &selected) override;

    // Returns list of orphaned packages
    QStringList orphanedPackages();
    // Remove specific packages
    bool removePackages(const QStringList &pkgs);

private:
    SystemInfo::PackageManager m_pm;
    QStringList getAptOrphans();
    QStringList getPacmanOrphans();
    QStringList getFlatpakUnused();
    qint64 getSnapRevisionWaste();
};
