#pragma once
#include "basecleaner.h"

class TempCleaner : public BaseCleaner {
    Q_OBJECT
public:
    explicit TempCleaner(QObject *parent = nullptr);

    QString name()        const override { return "Archivos Temporales"; }
    QString description() const override { return "Limpia archivos temporales del sistema y usuario"; }

    QList<CleanEntry> entries() override { return m_entries; }
    void analyze()  override;
    qint64 clean(const QList<CleanEntry> &selected) override;

private:
    void setupEntries();
};
