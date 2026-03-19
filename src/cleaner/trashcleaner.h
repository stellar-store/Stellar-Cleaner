#pragma once
#include "basecleaner.h"

class TrashCleaner : public BaseCleaner {
    Q_OBJECT
public:
    explicit TrashCleaner(QObject *parent = nullptr);
    QString name()        const override { return "Papelera"; }
    QString description() const override { return "Vacía la papelera del usuario y de root"; }
    QList<CleanEntry> entries() override { return m_entries; }
    void analyze()  override;
    qint64 clean(const QList<CleanEntry> &selected) override;
};
