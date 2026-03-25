#pragma once
#include "basecleaner.h"
#include <QMap>
#include <QStringList>

struct PerformanceProfile {
    QString name;
    bool enabled = false;
    int cpuGovernor = -1; // 0=powersave, 1=performance
    int cpuFreqScale = 100; // percentage
    bool turboBoost = false;
    bool swapOptimization = false;
};

class PerformanceCleaner : public BaseCleaner {
    Q_OBJECT
public:
    explicit PerformanceCleaner(QObject *parent = nullptr);

    QString name()        const override { return "Optimizador de Latencia"; }
    QString description() const override { 
        return "Optimiza el rendimiento del sistema activando modo máximo rendimiento (Game Mode)"; 
    }

    QList<CleanEntry> entries() override { return m_entries; }
    void analyze()  override;
    qint64 clean(const QList<CleanEntry> &selected) override;

    // Performance-specific methods
    bool activatePerformanceMode();
    bool deactivatePerformanceMode();
    bool isPerformanceModeActive() const { return m_performanceActive; }
    
    QString getCPUGovernor();
    bool setCPUGovernor(const QString &governor); // "performance" or "powersave"
    
    bool setProcPriority(int pid, int priority); // -20 to 19
    bool getProcPriority(int pid, int &priority);
    
    QStringList getAvailableGovernors();
    int getCPUCount();

private:
    struct CPUInfo {
        int cpuId = 0;
        QString governor;
        qint64 frequency = 0;
    };

    void setupEntries();
    void detectCPUGovernors();
    bool writeCPUSysfs(const QString &path, const QString &value);
    QString readCPUSysfs(const QString &path);
    QString getGovernorDescription(const QString &governor);
    
    bool m_performanceActive = false;
    QString m_previousGovernor;
    QMap<int, CPUInfo> m_cpuInfo;
    QStringList m_availableGovernors;
    int m_cpuCount = 0;
};
