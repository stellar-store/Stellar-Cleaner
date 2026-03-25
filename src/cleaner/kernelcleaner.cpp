#include "kernelcleaner.h"
#include <QProcess>
#include <QDebug>
#include <QStandardPaths>
#include <algorithm>

KernelCleaner::KernelCleaner(QObject *parent) : BaseCleaner(parent) {
}

QString KernelCleaner::getCurrentKernel() {
    QProcess process;
    process.start("uname", QStringList() << "-r");
    process.waitForFinished(3000);

    if (process.exitCode() != 0) {
        emitLog("[Kernel] Error al detectar kernel actual");
        return "";
    }

    return QString(process.readAllStandardOutput()).trimmed();
}

QList<KernelCleaner::KernelInfo> KernelCleaner::getInstalledKernels() {
    QList<KernelInfo> result;
    
    // Get all linux-image packages
    QProcess process;
    process.start("bash", QStringList() << "-c" << "dpkg --get-selections | grep linux-image | grep install | awk '{print $1}'");
    process.waitForFinished(5000);

    if (process.exitCode() != 0) {
        emitLog("[Kernel] Error al listar kernels instalados");
        return result;
    }

    QString output = QString(process.readAllStandardOutput());
    QStringList packages = output.split('\n', Qt::SkipEmptyParts);

    for (const auto &pkg : packages) {
        KernelInfo info;
        info.packageName = pkg;
        
        // Extract version from package name (e.g., linux-image-5.15.0-86-generic -> 5.15.0-86-generic)
        if (pkg.startsWith("linux-image-")) {
            info.version = pkg.mid(12); // Remove "linux-image-" prefix
        } else {
            info.version = pkg;
        }

        info.size = getPackageSize(pkg);
        result.append(info);
    }

    return result;
}

qint64 KernelCleaner::getPackageSize(const QString &packageName) {
    QProcess process;
    process.start("bash", QStringList() << "-c" 
        << QString("dpkg-query -W -f='${Installed-Size}' %1 2>/dev/null").arg(packageName));
    process.waitForFinished(3000);

    if (process.exitCode() == 0) {
        bool ok;
        qint64 sizeKb = QString(process.readAllStandardOutput()).trimmed().toLongLong(&ok);
        if (ok) {
            return sizeKb * 1024; // Convert from KB to bytes
        }
    }
    return 0;
}

void KernelCleaner::setupEntries() {
    m_entries.clear();
    m_kernelInfoMap.clear();

    m_currentKernel = getCurrentKernel();
    if (m_currentKernel.isEmpty()) {
        emitLog("[Kernel] No se pudo detectar el kernel actual");
        return;
    }

    emitLog(QString("[Kernel] Kernel actual: %1").arg(m_currentKernel));

    QList<KernelInfo> installedKernels = getInstalledKernels();
    
    // Sort by version (newest first)
    std::sort(installedKernels.begin(), installedKernels.end(),
        [](const KernelInfo &a, const KernelInfo &b) {
            return a.version > b.version;
        });

    int kernelCount = 0;
    for (const auto &kernel : installedKernels) {
        // Skip the current kernel
        if (kernel.version == m_currentKernel) {
            emitLog(QString("[Kernel] Kernel actual en uso, omitiendo: %1").arg(kernel.packageName));
            continue;
        }

        // Store kernel info for later use
        m_kernelInfoMap[kernel.packageName] = kernel;

        // Create a clean entry for each old kernel
        CleanEntry entry;
        entry.id = kernel.packageName;
        entry.name = QString("Kernel %1").arg(kernel.version);
        entry.description = QString("Paquete: %1").arg(kernel.packageName);
        entry.path = "/boot"; // Logical path, kernels affect /boot
        entry.category = "Kernels";
        entry.sizeBytes = kernel.size;
        entry.selected = (kernelCount < 3) ? false : true; // Select only if we have many
        entry.requiresRoot = true;
        entry.analyzed = true;

        m_entries.append(entry);
        kernelCount++;
    }

    if (m_entries.isEmpty()) {
        emitLog("[Kernel] No se encontraron kernels antiguos para eliminar");
    } else {
        emitLog(QString("[Kernel] Se encontraron %1 kernel(s) antiguo(s)").arg(m_entries.size()));
    }
}

void KernelCleaner::analyze() {
    m_analyzing = true;
    emit analyzeStarted();

    setupEntries();

    m_analyzing = false;
    emit analyzeFinished(totalSize());
}

qint64 KernelCleaner::clean(const QList<CleanEntry> &selected) {
    if (selected.isEmpty()) {
        emitLog("[Kernel] No se seleccionaron kernels para eliminar");
        return 0;
    }

    m_cleaning = true;
    emit cleanStarted();

    qint64 totalFreed = 0;
    int total = selected.size();

    for (int i = 0; i < total; ++i) {
        const auto &entry = selected[i];
        emit cleanProgress((i * 100) / total, QString("Eliminando: %1").arg(entry.name));

        // Extract package names related to the kernel
        QString version = entry.id.mid(12); // Remove "linux-image-" prefix
        QStringList packagesToRemove;
        packagesToRemove << entry.id; // linux-image-X.X.X

        // Also try to remove related packages
        packagesToRemove << QString("linux-headers-%1").arg(version);
        packagesToRemove << QString("linux-modules-%1").arg(version);
        packagesToRemove << QString("linux-modules-extra-%1").arg(version);

        // Use apt-get purge to remove kernel and its associated files
        QProcess removeProcess;
        QString command = QString("apt-get purge -y %1").arg(packagesToRemove.join(" "));
        
        emitLog(QString("[Kernel] Ejecutando: %1").arg(command));

        // Note: This requires root privileges
        // The application should handle privilege escalation separately
        removeProcess.start("bash", QStringList() << "-c" << command);
        removeProcess.waitForFinished(30000); // 30 seconds timeout

        if (removeProcess.exitCode() == 0) {
            totalFreed += entry.sizeBytes;
            emitLog(QString("[Kernel] ✓ Eliminado: %1 (%2 KB)")
                    .arg(entry.name)
                    .arg(entry.sizeBytes / 1024));
        } else {
            QString error = QString(removeProcess.readAllStandardError());
            emitLog(QString("[Kernel] ✗ Error al eliminar %1: %2").arg(entry.name).arg(error));
        }
    }

    m_cleaning = false;
    emit cleanFinished(totalFreed);
    return totalFreed;
}
