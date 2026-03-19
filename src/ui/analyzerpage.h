#pragma once
#include <QWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QComboBox>
#include <QSplitter>
#include <QFutureWatcher>
#include "../analyzer/diskanalyzer.h"

class TreeMapWidget;

class AnalyzerPage : public QWidget {
    Q_OBJECT
public:
    explicit AnalyzerPage(QWidget *parent = nullptr);

private slots:
    void onAnalyze();
    void onAnalyzeFinished();
    void onPathChanged(const QString &path);
    void onItemDoubleClicked(QTreeWidgetItem *item, int col);
    void onDeleteSelected();

private:
    void setupUI();
    void populateTree(const DiskNode &node, QTreeWidgetItem *parent = nullptr);
    void updateTreeMapView(const DiskNode &root);

    QComboBox    *m_pathCombo;
    QPushButton  *m_btnAnalyze;
    QPushButton  *m_btnDelete;
    QProgressBar *m_progress;
    QLabel       *m_lblStatus;
    QTreeWidget  *m_tree;
    TreeMapWidget *m_treeMap;
    QSplitter    *m_splitter;

    DiskAnalyzer *m_analyzer;
    QFutureWatcher<void> *m_watcher;
    DiskNode m_result;
};

// ---- TreeMap Widget ----
class TreeMapWidget : public QWidget {
    Q_OBJECT
public:
    explicit TreeMapWidget(QWidget *parent = nullptr);
    void setRoot(const DiskNode &root);

signals:
    void nodeClicked(const DiskNode &node);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    struct Rect { float x,y,w,h; const DiskNode *node; };
    void computeLayout(const DiskNode &node, float x, float y, float w, float h, int depth);
    DiskNode m_root;
    QList<Rect> m_rects;
};
