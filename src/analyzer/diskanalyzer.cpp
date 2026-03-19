#include "diskanalyzer.h"
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <algorithm>

const QList<QColor> DiskAnalyzer::COLORS = {
    QColor("#00d4aa"), QColor("#0099ff"), QColor("#f59e0b"),
    QColor("#ef4444"), QColor("#8b5cf6"), QColor("#22c55e"),
    QColor("#ec4899"), QColor("#06b6d4"), QColor("#84cc16"),
    QColor("#f97316"), QColor("#a78bfa"), QColor("#34d399")
};

DiskAnalyzer::DiskAnalyzer(QObject *parent) : QObject(parent) {}

void DiskAnalyzer::setRootPath(const QString &path) { m_rootPath = path; }
void DiskAnalyzer::setMaxDepth(int depth)             { m_maxDepth = depth; }
void DiskAnalyzer::setMinSize(qint64 minBytes)        { m_minSize  = minBytes; }

void DiskAnalyzer::analyze() {
    m_running   = true;
    m_cancelled = false;

    m_root = scanDir(m_rootPath, 0);
    sortChildren(m_root);

    m_running = false;
    if (m_cancelled)
        emit cancelled();
    else
        emit finished(m_root);
}

void DiskAnalyzer::cancel() {
    m_cancelled = true;
}

DiskNode DiskAnalyzer::scanDir(const QString &path, int depth) {
    DiskNode node;
    node.path  = path;
    node.isDir = true;
    QFileInfo fi(path);
    node.name  = fi.fileName().isEmpty() ? path : fi.fileName();

    if (m_cancelled) return node;

    emit progress(0, path);

    QDir dir(path);
    const auto entries = dir.entryInfoList(
        QDir::Files | QDir::Dirs | QDir::Hidden |
        QDir::NoDotAndDotDot | QDir::NoSymLinks);

    int colorIdx = 0;
    for (const auto &entry : entries) {
        if (m_cancelled) break;

        DiskNode child;
        child.name  = entry.fileName();
        child.path  = entry.absoluteFilePath();
        child.color = COLORS[colorIdx % COLORS.size()];
        colorIdx++;

        if (entry.isDir()) {
            if (depth < m_maxDepth) {
                child = scanDir(entry.absoluteFilePath(), depth + 1);
                child.color = COLORS[(colorIdx - 1) % COLORS.size()];
            } else {
                child.isDir = true;
                // Quick size via iterator
                QDirIterator it(entry.absoluteFilePath(),
                    QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
                while (it.hasNext()) {
                    it.next();
                    child.size += it.fileInfo().size();
                }
            }
        } else {
            child.isDir = false;
            child.size  = entry.size();
        }

        if (child.size >= m_minSize || child.isDir) {
            node.size += child.size;
            if (child.size >= m_minSize)
                node.children << child;
            else
                node.size += 0; // still count but don't show
        }
    }
    return node;
}

void DiskAnalyzer::sortChildren(DiskNode &node) {
    std::sort(node.children.begin(), node.children.end(),
        [](const DiskNode &a, const DiskNode &b) {
            return a.size > b.size;
        });
    for (auto &child : node.children)
        if (child.isDir) sortChildren(child);
}
