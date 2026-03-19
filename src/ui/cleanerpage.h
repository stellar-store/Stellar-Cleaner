#pragma once
#include <QWidget>
#include <QTreeWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QSplitter>
#include <QFutureWatcher>
#include "../cleaner/basecleaner.h"
#include "../cleaner/tempcleaner.h"
#include "../cleaner/browsercleaner.h"
#include "../cleaner/logcleaner.h"
#include "../cleaner/packagecleaner.h"
#include "../cleaner/privacycleaner.h"
#include "../cleaner/trashcleaner.h"

class CleanerPage : public QWidget {
    Q_OBJECT
public:
    explicit CleanerPage(QWidget *parent = nullptr);

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
    void connectCleaner(BaseCleaner *c);

    QTreeWidget  *m_tree;
    QTextEdit    *m_logView;
    QLabel       *m_lblTotal;
    QLabel       *m_lblStatus;
    QProgressBar *m_progress;
    QPushButton  *m_btnAnalyze;
    QPushButton  *m_btnClean;
    QLabel       *m_lblSelectedSize;
    QSplitter    *m_splitter;

    TempCleaner    *m_tempCleaner;
    BrowserCleaner *m_browserCleaner;
    LogCleaner     *m_logCleaner;
    PackageCleaner *m_pkgCleaner;
    PrivacyCleaner *m_privCleaner;
    TrashCleaner   *m_trashCleaner;

    QList<BaseCleaner*> m_cleaners;
    int m_currentCleanerIdx = 0;
    bool m_analyzing = false;
    bool m_cleaning  = false;
    qint64 m_totalFreed = 0;

    QFutureWatcher<void> *m_watcher;
};
