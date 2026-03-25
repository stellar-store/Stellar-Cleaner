#pragma once
#include <QWidget>
#include <QTreeWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QSplitter>
#include <QFutureWatcher>
#include "../cleaner/performancecleaner.h"

class OptimizerPage : public QWidget {
    Q_OBJECT
public:
    explicit OptimizerPage(QWidget *parent = nullptr);

signals:
    void totalFreedChanged(qint64 bytes);

private slots:
    void onAnalyze();
    void onClean();
    void onAnalyzeFinished();
    void onCleanFinished();
    void onSelectAll();
    void onSelectNone();
    void onItemChanged(QTreeWidgetItem *item, int col);
    void appendLog(const QString &msg);

private:
    void setupUI();
    void populateTree();
    void updateSummary();

    QTreeWidget  *m_tree;
    QTextEdit    *m_logView;
    QLabel       *m_lblTotal;
    QLabel       *m_lblStatus;
    QProgressBar *m_progress;
    QPushButton  *m_btnAnalyze;
    QPushButton  *m_btnClean;
    QLabel       *m_lblSelectedSize;
    QSplitter    *m_splitter;
    PerformanceCleaner *m_perfCleaner;

    QList<BaseCleaner*> m_cleaners;
    bool m_analyzing = false;
    bool m_cleaning  = false;
    qint64 m_totalFreed = 0;

    QFutureWatcher<void> *m_watcher;
};
