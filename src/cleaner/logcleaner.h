#pragma once
#include "basecleaner.h"

class LogCleaner : public BaseCleaner {
    Q_OBJECT
public:
    explicit LogCleaner(QObject *parent = nullptr);

    QString name()        const override { return "Logs del Sistema"; }
    QString description() const override { return "Limpia logs de journald, /var/log y aplicaciones"; }

    QList<CleanEntry> entries() override { return m_entries; }
    void analyze()  override;
    qint64 clean(const QList<CleanEntry> &selected) override;

    void setJournalVacuumDays(int days) { m_journalDays = days; }
    int journalVacuumDays() const { return m_journalDays; }

private:
    int m_journalDays = 30;
    qint64 getJournalSize();
};
