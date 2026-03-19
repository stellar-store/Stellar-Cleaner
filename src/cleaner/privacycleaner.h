#pragma once
#include "basecleaner.h"

class PrivacyCleaner : public BaseCleaner {
    Q_OBJECT
public:
    explicit PrivacyCleaner(QObject *parent = nullptr);

    QString name()        const override { return "Privacidad y Rastros"; }
    QString description() const override { return "Elimina rastros de actividad y archivos recientes"; }

    QList<CleanEntry> entries() override { return m_entries; }
    void analyze()  override;
    qint64 clean(const QList<CleanEntry> &selected) override;
};
