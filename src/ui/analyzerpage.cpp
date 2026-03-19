#include "analyzerpage.h"
#include "../utils/fileutils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDir>
#include <QStandardPaths>
#include <QPainter>
#include <QMouseEvent>
#include <QtConcurrent>
#include <QMessageBox>
#include <QFileDialog>

// ==================== TreeMapWidget ====================
TreeMapWidget::TreeMapWidget(QWidget *parent) : QWidget(parent) {
    setMinimumHeight(200);
}

void TreeMapWidget::setRoot(const DiskNode &root) {
    m_root = root;
    m_rects.clear();
    if (root.size > 0)
        computeLayout(root, 0, 0, (float)width(), (float)height(), 0);
    update();
}

void TreeMapWidget::computeLayout(const DiskNode &node, float x, float y,
                                   float w, float h, int depth) {
    if (w < 2 || h < 2 || node.children.isEmpty()) return;

    bool horizontal = w >= h;
    float pos = horizontal ? x : y;
    float total = (float)node.size;

    for (const auto &child : node.children) {
        if (child.size <= 0) continue;
        float ratio = (float)child.size / total;
        float size  = ratio * (horizontal ? w : h);

        Rect r;
        if (horizontal) { r.x=pos; r.y=y; r.w=size; r.h=h; }
        else             { r.x=x; r.y=pos; r.w=w; r.h=size; }
        r.node = &child;
        m_rects << r;

        if (depth < 3 && !child.children.isEmpty()) {
            float pad = 2.0f;
            computeLayout(child, r.x+pad, r.y+pad,
                          r.w-2*pad, r.h-2*pad, depth+1);
        }
        pos += size;
    }
}

void TreeMapWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor("#0f1117"));

    for (const auto &r : m_rects) {
        if (r.w < 2 || r.h < 2) continue;
        QRectF rect(r.x, r.y, r.w-1, r.h-1);
        QColor col = r.node->color.isValid() ? r.node->color : QColor("#00d4aa");
        col.setAlpha(180);
        p.fillRect(rect, col);
        p.setPen(QColor("#0f1117"));
        p.drawRect(rect);

        if (r.w > 60 && r.h > 20) {
            p.setPen(Qt::white);
            QFont f = p.font(); f.setPointSize(9); p.setFont(f);
            QString label = r.node->name;
            QString size  = FileUtils::formatSize(r.node->size, 1);
            p.drawText(rect.adjusted(3,3,-3,-3), Qt::AlignTop | Qt::AlignLeft,
                       label + "\n" + size);
        }
    }

    if (m_rects.isEmpty()) {
        p.setPen(QColor("#4a5568"));
        p.drawText(rect(), Qt::AlignCenter, "Haz clic en 'Analizar' para ver el mapa de disco");
    }
}

void TreeMapWidget::mousePressEvent(QMouseEvent *event) {
    QPointF pos = event->pos();
    for (auto it = m_rects.rbegin(); it != m_rects.rend(); ++it) {
        QRectF r(it->x, it->y, it->w, it->h);
        if (r.contains(pos)) {
            emit nodeClicked(*it->node);
            break;
        }
    }
}

void TreeMapWidget::resizeEvent(QResizeEvent *) {
    m_rects.clear();
    if (m_root.size > 0)
        computeLayout(m_root, 0, 0, (float)width(), (float)height(), 0);
}

// ==================== AnalyzerPage ====================
AnalyzerPage::AnalyzerPage(QWidget *parent) : QWidget(parent) {
    m_analyzer = new DiskAnalyzer(this);
    m_watcher  = new QFutureWatcher<void>(this);
    connect(m_watcher, &QFutureWatcher<void>::finished,
            this, &AnalyzerPage::onAnalyzeFinished);
    connect(m_analyzer, &DiskAnalyzer::progress, this,
        [this](int pct, const QString &path) {
            m_progress->setValue(pct);
            m_lblStatus->setText(path.right(60));
        });
    setupUI();
}

void AnalyzerPage::setupUI() {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // Toolbar
    auto *tb = new QWidget(this);
    tb->setObjectName("pageHeader");
    tb->setFixedHeight(56);
    auto *tbLay = new QHBoxLayout(tb);
    tbLay->setContentsMargins(20,8,20,8);
    tbLay->setSpacing(8);

    auto *ttl = new QLabel("Analizador de Disco", tb);
    ttl->setObjectName("pageTitle");
    tbLay->addWidget(ttl);

    m_pathCombo = new QComboBox(tb);
    m_pathCombo->setMinimumWidth(250);
    m_pathCombo->setEditable(true);
    m_pathCombo->addItem(QDir::homePath());
    m_pathCombo->addItem("/");
    m_pathCombo->addItem("/usr");
    m_pathCombo->addItem("/var");
    m_pathCombo->addItem("/opt");

    auto *browseBtn = new QPushButton("📁", tb);
    browseBtn->setObjectName("btnSecondary");
    browseBtn->setFixedWidth(36);
    connect(browseBtn, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Seleccionar directorio",
            m_pathCombo->currentText());
        if (!dir.isEmpty()) {
            m_pathCombo->setCurrentText(dir);
        }
    });

    tbLay->addStretch();
    tbLay->addWidget(new QLabel("Ruta:", tb));
    tbLay->addWidget(m_pathCombo);
    tbLay->addWidget(browseBtn);

    m_btnAnalyze = new QPushButton("🔍  Analizar", tb);
    m_btnAnalyze->setObjectName("btnPrimary");
    m_btnAnalyze->setFixedWidth(130);
    tbLay->addWidget(m_btnAnalyze);

    m_btnDelete = new QPushButton("🗑  Eliminar", tb);
    m_btnDelete->setObjectName("btnDanger");
    m_btnDelete->setFixedWidth(120);
    m_btnDelete->setEnabled(false);
    tbLay->addWidget(m_btnDelete);

    connect(m_btnAnalyze, &QPushButton::clicked, this, &AnalyzerPage::onAnalyze);
    connect(m_btnDelete,  &QPushButton::clicked, this, &AnalyzerPage::onDeleteSelected);
    root->addWidget(tb);

    // Progress
    auto *progBar = new QWidget(this);
    progBar->setObjectName("card");
    progBar->setFixedHeight(36);
    auto *pbLay = new QHBoxLayout(progBar);
    pbLay->setContentsMargins(16,6,16,6);
    pbLay->setSpacing(10);
    m_progress = new QProgressBar(progBar);
    m_progress->setTextVisible(false);
    m_progress->setFixedHeight(6);
    m_lblStatus = new QLabel("Listo", progBar);
    m_lblStatus->setObjectName("statLabel");
    m_lblStatus->setMinimumWidth(400);
    pbLay->addWidget(m_progress, 1);
    pbLay->addWidget(m_lblStatus);
    root->addWidget(progBar);

    // Splitter: tree + treemap
    m_splitter = new QSplitter(Qt::Vertical, this);
    m_splitter->setHandleWidth(1);

    // TreeMap
    m_treeMap = new TreeMapWidget(m_splitter);
    m_treeMap->setMinimumHeight(240);
    connect(m_treeMap, &TreeMapWidget::nodeClicked, this,
        [this](const DiskNode &node) {
            m_lblStatus->setText(QString("%1  |  %2")
                .arg(node.path, FileUtils::formatSize(node.size)));
        });
    m_splitter->addWidget(m_treeMap);

    // File tree
    m_tree = new QTreeWidget(m_splitter);
    m_tree->setColumnCount(4);
    m_tree->setHeaderLabels({"Nombre", "Ruta", "Tamaño", "%"});
    m_tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_tree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_tree->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tree->setAlternatingRowColors(true);
    m_tree->setSortingEnabled(true);
    m_tree->sortByColumn(2, Qt::DescendingOrder);
    connect(m_tree, &QTreeWidget::itemDoubleClicked,
            this, &AnalyzerPage::onItemDoubleClicked);
    connect(m_tree, &QTreeWidget::itemSelectionChanged, this,
        [this]() { m_btnDelete->setEnabled(!m_tree->selectedItems().isEmpty()); });
    m_splitter->addWidget(m_tree);

    m_splitter->setStretchFactor(0, 2);
    m_splitter->setStretchFactor(1, 3);
    root->addWidget(m_splitter, 1);
}

void AnalyzerPage::onAnalyze() {
    m_btnAnalyze->setEnabled(false);
    m_progress->setValue(0);
    m_tree->clear();
    m_treeMap->setRoot(DiskNode{});
    m_lblStatus->setText("Analizando...");

    QString path = m_pathCombo->currentText();
    m_analyzer->setRootPath(path);
    m_analyzer->setMaxDepth(4);
    m_analyzer->setMinSize(512 * 1024); // 512 KB min

    auto future = QtConcurrent::run([this]() {
        m_analyzer->analyze();
    });
    m_watcher->setFuture(future);
}

void AnalyzerPage::onAnalyzeFinished() {
    m_btnAnalyze->setEnabled(true);
    m_result = m_analyzer->result();
    m_progress->setValue(100);
    m_lblStatus->setText(QString("Análisis completo — Total: %1")
        .arg(FileUtils::formatSize(m_result.size)));
    populateTree(m_result);
    m_treeMap->setRoot(m_result);
}

void AnalyzerPage::populateTree(const DiskNode &node, QTreeWidgetItem *parent) {
    for (const auto &child : node.children) {
        QTreeWidgetItem *item = parent
            ? new QTreeWidgetItem(parent)
            : new QTreeWidgetItem(m_tree);

        QString icon = child.isDir ? "📁" : "📄";
        item->setText(0, icon + " " + child.name);
        item->setText(1, child.path);
        item->setText(2, FileUtils::formatSize(child.size));
        float pct = node.size > 0 ? (float)child.size / node.size * 100.0f : 0;
        item->setText(3, QString("%1%").arg(pct, 0, 'f', 1));
        item->setData(0, Qt::UserRole, child.path);
        item->setData(0, Qt::UserRole+1, child.isDir);
        item->setForeground(2, QBrush(child.size > 500*1024*1024
            ? QColor("#ef4444")
            : child.size > 100*1024*1024 ? QColor("#f59e0b")
            : QColor("#8892a4")));

        if (child.isDir && !child.children.isEmpty())
            populateTree(child, item);
    }
}

void AnalyzerPage::onItemDoubleClicked(QTreeWidgetItem *item, int) {
    QString path = item->data(0, Qt::UserRole).toString();
    bool isDir   = item->data(0, Qt::UserRole+1).toBool();
    if (isDir) {
        m_pathCombo->setCurrentText(path);
        onAnalyze();
    }
}

void AnalyzerPage::onDeleteSelected() {
    auto selected = m_tree->selectedItems();
    if (selected.isEmpty()) return;

    QStringList paths;
    for (auto *item : selected)
        paths << item->data(0, Qt::UserRole).toString();

    QString msg = QString("¿Eliminar %1 elemento(s) seleccionado(s)?\n\nEsta acción NO se puede deshacer.")
        .arg(paths.size());
    if (QMessageBox::question(this, "Confirmar eliminación", msg,
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;

    qint64 freed = 0;
    for (const auto &p : paths) {
        QFileInfo fi(p);
        if (fi.isDir())  freed += FileUtils::deleteDir(p, true, true);
        else             freed += FileUtils::deleteFile(p);
    }

    m_lblStatus->setText(QString("Eliminados — Liberados: %1").arg(FileUtils::formatSize(freed)));
    onAnalyze(); // refresh
}

void AnalyzerPage::onPathChanged(const QString &path) {
    Q_UNUSED(path)
}
