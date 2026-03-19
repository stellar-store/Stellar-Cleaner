#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include <QColor>
#include <QDir>

struct DiskNode {
    QString name;
    QString path;
    qint64  size    = 0;
    bool    isDir   = false;
    QColor  color;
    QList<DiskNode> children;

    float percentage(qint64 parentSize) const {
        return parentSize > 0 ? (float)size / parentSize * 100.0f : 0;
    }
};

class DiskAnalyzer : public QObject {
    Q_OBJECT
public:
    explicit DiskAnalyzer(QObject *parent = nullptr);

    void setRootPath(const QString &path);
    void setMaxDepth(int depth);
    void setMinSize(qint64 minBytes);

    void analyze();
    void cancel();

    const DiskNode &result() const { return m_root; }
    bool isRunning() const { return m_running; }

signals:
    void progress(int percent, const QString &currentPath);
    void finished(const DiskNode &root);
    void cancelled();

private:
    DiskNode scanDir(const QString &path, int depth = 0);
    void sortChildren(DiskNode &node);

    QString m_rootPath = QDir::homePath();
    int     m_maxDepth = 5;
    qint64  m_minSize  = 1024 * 1024; // 1 MB
    bool    m_running  = false;
    bool    m_cancelled = false;
    DiskNode m_root;

    // Color palette for treemap
    static const QList<QColor> COLORS;
};
