#include "performancecleaner.h"
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <sys/resource.h>
#include <unistd.h>
#include <cerrno>
#include <QStandardPaths>

PerformanceCleaner::PerformanceCleaner(QObject *parent) 
    : BaseCleaner(parent), m_performanceActive(false) {
    m_cpuCount = sysconf(_SC_NPROCESSORS_ONLN);
    detectCPUGovernors();
    setupEntries();
}

void PerformanceCleaner::setupEntries() {
    m_entries.clear();
    
    // Entry 1: CPU Governor Optimization
    CleanEntry cpuEntry;
    cpuEntry.id = "cpu_governor";
    cpuEntry.name = "Optimizer CPU Governor";
    cpuEntry.description = "Cambiar governor de CPU a 'performance' para máximo rendimiento";
    cpuEntry.category = "Performance";
    cpuEntry.requiresRoot = true;
    cpuEntry.selected = true;
    m_entries.append(cpuEntry);
    
    // Entry 2: Turbo Boost
    CleanEntry turboEntry;
    turboEntry.id = "turbo_boost";
    turboEntry.name = "Intel Turbo Boost / AMD Turbo Core";
    turboEntry.description = "Activar Turbo Boost/Core para aceleración de frecuencia";
    turboEntry.category = "Performance";
    turboEntry.requiresRoot = true;
    turboEntry.selected = true;
    m_entries.append(turboEntry);
    
    // Entry 3: Process Priority Boost
    CleanEntry procEntry;
    procEntry.id = "process_priority";
    procEntry.name = "Prioridad de Procesos en Primer Plano";
    procEntry.description = "Ajustar niceness de procesos en primer plano para mejor respuesta";
    procEntry.category = "Performance";
    procEntry.requiresRoot = false;
    procEntry.selected = true;
    m_entries.append(procEntry);
    
    // Entry 4: Swap Memory Optimization
    CleanEntry swapEntry;
    swapEntry.id = "swap_optimization";
    swapEntry.name = "Optimización de Memoria Swap";
    swapEntry.description = "Reducir uso de swap para menor latencia";
    swapEntry.category = "Performance";
    swapEntry.requiresRoot = true;
    swapEntry.selected = true;
    m_entries.append(swapEntry);
}

void PerformanceCleaner::analyze() {
    emit analyzeStarted();
    
    detectCPUGovernors();
    
    // Get current CPU governor
    QString currentGov = getCPUGovernor();
    emitLog(QString("[Performance] Información actual del sistema detectada"));
    emitLog(QString("[Performance] Governor actual: %1").arg(currentGov));
    emitLog(QString("[Performance] CPUs disponibles: %1").arg(m_cpuCount));
    emitLog(QString("[Performance] Governors disponibles: %1").arg(m_availableGovernors.join(", ")));
    
    emit analyzeProgress(100, "Análisis completado");
}

qint64 PerformanceCleaner::clean(const QList<CleanEntry> &selected) {
    emitLog("[Performance] Iniciando optimización...");
    
    bool success = true;
    
    for (const auto &entry : selected) {
        if (entry.id == "cpu_governor") {
            emitLog("[Performance] Aplicando CPU Governor 'performance'...");
            if (!setCPUGovernor("performance")) {
                success = false;
                emitLog("[Performance] Error al cambiar CPU Governor");
            } else {
                emitLog("[Performance] CPU Governor cambiado a 'performance' ✓");
            }
        }
        else if (entry.id == "turbo_boost") {
            emitLog("[Performance] Intentando activar Turbo Boost...");
            // Intel Turbo Boost
            bool turboSet = writeCPUSysfs(
                "/sys/devices/system/cpu/intel_pstate/no_turbo", "0"
            );
            if (turboSet) {
                emitLog("[Performance] Intel Turbo Boost activado ✓");
            } else {
                // Try AMD
                turboSet = writeCPUSysfs(
                    "/sys/devices/system/cpu/cpufreq/boost", "1"
                );
                if (turboSet) {
                    emitLog("[Performance] AMD Turbo Core activado ✓");
                } else {
                    emitLog("[Performance] No se pudo activar Turbo (CPU no compatible)");
                }
            }
        }
        else if (entry.id == "process_priority") {
            emitLog("[Performance] Ajustando prioridades de procesos...");
            
            // Get foreground processes (optional - can focus on specific apps)
            QProcess proc;
            proc.start("bash", QStringList() << "-c" 
                << "ps aux --sort=-%cpu | head -20 | awk 'NR>1 {print $2}'");
            proc.waitForFinished(3000);
            
            QString output = QString(proc.readAllStandardOutput());
            QStringList pids = output.split('\n', Qt::SkipEmptyParts);
            
            int adjustedCount = 0;
            for (const QString &pidStr : pids) {
                bool ok;
                int pid = pidStr.toInt(&ok);
                if (ok && pid > 0) {
                    // Set priority to -5 (higher priority, more CPU time)
                    if (setProcPriority(pid, -5)) {
                        adjustedCount++;
                    }
                }
            }
            
            emitLog(QString("[Performance] %1 procesos ajustados a prioridad alta ✓")
                .arg(adjustedCount));
        }
        else if (entry.id == "swap_optimization") {
            emitLog("[Performance] Optimizando swap...");
            
            // Reduce swappiness (lower value = less swap usage)
            bool swapSet = writeCPUSysfs("/proc/sys/vm/swappiness", "10");
            if (swapSet) {
                emitLog("[Performance] Swappiness reducido a 10 ✓");
            } else {
                emitLog("[Performance] No se pudo ajustar swappiness");
            }
        }
    }
    
    if (success) {
        m_performanceActive = true;
        emitLog("[Performance] ¡Modo Máximo Rendimiento activado! 🚀");
    }
    
    return 0; // No bytes freed, this is optimization
}

QString PerformanceCleaner::getCPUGovernor() {
    return readCPUSysfs("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
}

bool PerformanceCleaner::setCPUGovernor(const QString &governor) {
    bool allSuccess = true;
    
    for (int i = 0; i < m_cpuCount; ++i) {
        QString path = QString("/sys/devices/system/cpu/cpu%1/cpufreq/scaling_governor").arg(i);
        
        // Try to write directly first
        QProcess process;
        process.start("bash", QStringList() << "-c" 
            << QString("echo %1 | sudo tee %2 > /dev/null").arg(governor, path));
        process.waitForFinished(2000);
        
        if (process.exitCode() != 0) {
            allSuccess = false;
            emitLog(QString("[Performance] Error al cambiar governor de cpu%1").arg(i));
        }
    }
    
    return allSuccess;
}

bool PerformanceCleaner::setProcPriority(int pid, int priority) {
    // priority: -20 (highest) to 19 (lowest)
    // clamp to valid range
    priority = qMax(-20, qMin(19, priority));
    
    if (setpriority(PRIO_PROCESS, pid, priority) == 0) {
        return true;
    }
    return false;
}

bool PerformanceCleaner::getProcPriority(int pid, int &priority) {
    errno = 0;
    int result = getpriority(PRIO_PROCESS, pid);
    if (errno == 0) {
        priority = result;
        return true;
    }
    return false;
}

QString PerformanceCleaner::readCPUSysfs(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "";
    }
    
    QString content = file.readAll();
    file.close();
    return content.trimmed();
}

bool PerformanceCleaner::writeCPUSysfs(const QString &path, const QString &value) {
    QProcess process;
    process.start("bash", QStringList() << "-c" 
        << QString("echo %1 | sudo tee %2 > /dev/null 2>&1").arg(value, path));
    process.waitForFinished(2000);
    
    return process.exitCode() == 0;
}

void PerformanceCleaner::detectCPUGovernors() {
    QString govPath = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors";
    QString content = readCPUSysfs(govPath);
    
    if (!content.isEmpty()) {
        m_availableGovernors = content.split(' ', Qt::SkipEmptyParts);
    } else {
        // Fallback defaults
        m_availableGovernors = QStringList() << "performance" << "powersave" << "ondemand";
    }
}

QStringList PerformanceCleaner::getAvailableGovernors() {
    return m_availableGovernors;
}

int PerformanceCleaner::getCPUCount() {
    return m_cpuCount;
}

bool PerformanceCleaner::activatePerformanceMode() {
    QList<CleanEntry> allSelected = m_entries;
    return clean(allSelected) == 0; // Success if no error (0 bytes)
}

bool PerformanceCleaner::deactivatePerformanceMode() {
    emitLog("[Performance] Restaurando configuración normal...");
    
    // Restore powersave governor
    if (!setCPUGovernor("powersave")) {
        emitLog("[Performance] Error al restaurar governor");
    } else {
        emitLog("[Performance] Governor restaurado a 'powersave' ✓");
    }
    
    // Reset swappiness to default (60)
    writeCPUSysfs("/proc/sys/vm/swappiness", "60");
    
    m_performanceActive = false;
    emitLog("[Performance] Modo Máximo Rendimiento desactivado");
    
    return true;
}
