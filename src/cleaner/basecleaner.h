#pragma once
#include <QString>
#include <QStringList>
#include <QObject>
#include <QList>
#include <QColor>
#include <QIcon>

// Represents a single cleanable entry (file, dir, category)
struct CleanEntry {
    QString id;           // Unique identifier
    QString name;         // Display name
    QString description;  // Short description
    QString path;         // File system path
    QString category;     // Parent category name
    qint64  sizeBytes   = 0;
    bool    selected    = true;
    bool    requiresRoot = false;
    bool    analyzed    = false;
    QColor  color;        // For UI highlighting
    QIcon   icon;
};

class BaseCleaner : public QObject {
    Q_OBJECT
public:
    explicit BaseCleaner(QObject *parent = nullptr);
    virtual ~BaseCleaner() = default;

    virtual QString name() const = 0;
    virtual QString description() const = 0;

    // Returns list of all entries this cleaner manages
    virtual QList<CleanEntry> entries() = 0;

    // Analyze (calculate sizes) - can be slow, run in thread
    virtual void analyze() = 0;

    // Clean selected entries, returns total bytes freed
    virtual qint64 clean(const QList<CleanEntry> &selected) = 0;

    // Total size across all entries
    qint64 totalSize() const;

    bool isAnalyzing() const { return m_analyzing; }
    bool isCleaning()  const { return m_cleaning;  }

signals:
    void analyzeStarted();
    void analyzeProgress(int percent, const QString &currentItem);
    void analyzeFinished(qint64 totalBytes);
    void cleanStarted();
    void cleanProgress(int percent, const QString &currentItem);
    void cleanFinished(qint64 bytesFreed);
    void errorOccurred(const QString &message);
    void logMessage(const QString &msg);

protected:
    QList<CleanEntry> m_entries;
    bool m_analyzing = false;
    bool m_cleaning  = false;
    void emitLog(const QString &msg);
};
