#pragma once
#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <QSettings>

class Scheduler : public QObject {
    Q_OBJECT
public:
    enum Frequency { Daily, Weekly, Monthly, OnBoot };

    static Scheduler &instance();

    void setEnabled(bool enabled);
    void setFrequency(Frequency f);
    void setLastRun(const QDateTime &dt);
    void setSelectedCategories(const QStringList &cats);

    bool     isEnabled()    const;
    Frequency frequency()   const;
    QDateTime lastRun()     const;
    QDateTime nextRun()     const;
    QStringList selectedCategories() const;

    bool isDue() const;
    void start();
    void stop();

signals:
    void scheduledCleanDue();

private slots:
    void onTimer();

private:
    Scheduler();
    QTimer *m_timer = nullptr;
    QSettings m_settings;
};
