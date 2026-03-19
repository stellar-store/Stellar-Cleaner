#include "scheduler.h"

Scheduler &Scheduler::instance() {
    static Scheduler inst;
    return inst;
}

Scheduler::Scheduler()
    : m_settings("Stellar Cleaner", "Scheduler") {
    m_timer = new QTimer(this);
    m_timer->setInterval(60 * 60 * 1000); // Check every hour
    connect(m_timer, &QTimer::timeout, this, &Scheduler::onTimer);
}

void Scheduler::setEnabled(bool enabled) {
    m_settings.setValue("enabled", enabled);
    if (enabled) start(); else stop();
}

void Scheduler::setFrequency(Frequency f) {
    m_settings.setValue("frequency", (int)f);
}

void Scheduler::setLastRun(const QDateTime &dt) {
    m_settings.setValue("lastRun", dt.toString(Qt::ISODate));
}

void Scheduler::setSelectedCategories(const QStringList &cats) {
    m_settings.setValue("categories", cats);
}

bool Scheduler::isEnabled() const {
    return m_settings.value("enabled", false).toBool();
}

Scheduler::Frequency Scheduler::frequency() const {
    return (Frequency)m_settings.value("frequency", (int)Weekly).toInt();
}

QDateTime Scheduler::lastRun() const {
    return QDateTime::fromString(
        m_settings.value("lastRun", "").toString(), Qt::ISODate);
}

QDateTime Scheduler::nextRun() const {
    QDateTime last = lastRun();
    if (!last.isValid()) return QDateTime::currentDateTime();
    switch (frequency()) {
        case Daily:   return last.addDays(1);
        case Weekly:  return last.addDays(7);
        case Monthly: return last.addMonths(1);
        case OnBoot:  return QDateTime::currentDateTime();
    }
    return last.addDays(7);
}

QStringList Scheduler::selectedCategories() const {
    return m_settings.value("categories", QStringList{}).toStringList();
}

bool Scheduler::isDue() const {
    if (!isEnabled()) return false;
    if (frequency() == OnBoot) return true;
    QDateTime last = lastRun();
    if (!last.isValid()) return true;
    return QDateTime::currentDateTime() >= nextRun();
}

void Scheduler::start() {
    m_timer->start();
    onTimer(); // Check immediately
}

void Scheduler::stop() {
    m_timer->stop();
}

void Scheduler::onTimer() {
    if (isDue()) {
        emit scheduledCleanDue();
        setLastRun(QDateTime::currentDateTime());
    }
}
