#include "optimizerpage.h"
#include "../utils/fileutils.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QtConcurrent>
#include <QMessageBox>
#include <QDateTime>
#include <QScrollBar>

// Column indices for tree
enum Col { COL_NAME=0, COL_DESC=1, COL_PATH=2, COL_SIZE=3, COL_ROOT=4 };

OptimizerPage::OptimizerPage(QWidget *parent) : QWidget(parent) {
    m_perfCleaner = new PerformanceCleaner(this);
    m_cleaners = { m_perfCleaner };

    connect(m_perfCleaner, &BaseCleaner::logMessage, this, &OptimizerPage::appendLog);
    connect(m_perfCleaner, &BaseCleaner::analyzeProgress, this, [this](int pct, const QString &item) {
        m_progress->setValue(pct);
        m_lblStatus->setText(QString("Analizando: %1").arg(item));
    });
    connect(m_perfCleaner, &BaseCleaner::cleanProgress, this, [this](int pct, const QString &item) {
        m_progress->setValue(pct);
        m_lblStatus->setText(item);
    });

    m_watcher = new QFutureWatcher<void>(this);
    setupUI();
    populateTree();
}

void OptimizerPage::setupUI() {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // --- Toolbar ---
    auto *toolbar = new QWidget(this);
    toolbar->setObjectName("pageHeader");
    toolbar->setFixedHeight(56);
    auto *tbLay = new QHBoxLayout(toolbar);
    tbLay->setContentsMargins(20,8,20,8);
    tbLay->setSpacing(8);

    auto *titleLbl = new QLabel("⚡ Optimizador de Rendimiento", toolbar);
    titleLbl->setObjectName("pageTitle");
    tbLay->addWidget(titleLbl);

    auto *subtitleLbl = new QLabel("  Optimiza el rendimiento del sistema", toolbar);
    subtitleLbl->setObjectName("pageSubtitle");
    tbLay->addWidget(subtitleLbl);
    tbLay->addStretch();

    auto *btnSelAll  = new QPushButton("Seleccionar Todo", toolbar);
    auto *btnSelNone = new QPushButton("Ninguno", toolbar);
    btnSelAll->setObjectName("btnSecondary");
    btnSelNone->setObjectName("btnSecondary");
    tbLay->addWidget(btnSelAll);
    tbLay->addWidget(btnSelNone);

    m_btnAnalyze = new QPushButton("🔍  Analizar", toolbar);
    m_btnAnalyze->setObjectName("btnSecondary");
    m_btnAnalyze->setFixedWidth(130);
    tbLay->addWidget(m_btnAnalyze);

    m_btnClean = new QPushButton("⚙  Optimizar Ahora", toolbar);
    m_btnClean->setObjectName("btnDanger");
    m_btnClean->setFixedWidth(150);
    m_btnClean->setEnabled(false);
    tbLay->addWidget(m_btnClean);
    root->addWidget(toolbar);

    connect(btnSelAll,  &QPushButton::clicked, this, &OptimizerPage::onSelectAll);
    connect(btnSelNone, &QPushButton::clicked, this, &OptimizerPage::onSelectNone);
    connect(m_btnAnalyze, &QPushButton::clicked, this, &OptimizerPage::onAnalyze);
    connect(m_btnClean,   &QPushButton::clicked, this, &OptimizerPage::onClean);

    // --- Main area (splitter) ---
    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_splitter->setHandleWidth(1);

    // Left: tree
    auto *leftW = new QWidget(m_splitter);
    auto *leftLay = new QVBoxLayout(leftW);
    leftLay->setContentsMargins(16,12,8,12);
    leftLay->setSpacing(8);

    m_tree = new QTreeWidget(leftW);
    m_tree->setColumnCount(5);
    m_tree->setHeaderLabels({"Categoría / Elemento", "Descripción", "Ruta", "Tamaño", "Root"});
    m_tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_tree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_tree->header()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tree->header()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_tree->setAlternatingRowColors(true);
    m_tree->setRootIsDecorated(true);
    m_tree->setAnimated(true);
    m_tree->expandAll();
    connect(m_tree, &QTreeWidget::itemChanged, this, &OptimizerPage::onItemChanged);
    leftLay->addWidget(m_tree, 1);

    // Size summary bar
    auto *sumBar = new QWidget(leftW);
    auto *sumLay = new QHBoxLayout(sumBar);
    sumLay->setContentsMargins(0,4,0,0);
    m_lblSelectedSize = new QLabel("Seleccionado: 0 B", sumBar);
    m_lblSelectedSize->setObjectName("accent");
    sumLay->addWidget(m_lblSelectedSize);
    sumLay->addStretch();
    m_lblTotal = new QLabel("Total: 0 B", sumBar);
    m_lblTotal->setObjectName("statLabel");
    sumLay->addWidget(m_lblTotal);
    leftLay->addWidget(sumBar);

    m_splitter->addWidget(leftW);

    // Right: log + progress
    auto *rightW = new QWidget(m_splitter);
    rightW->setObjectName("card");
    rightW->setMinimumWidth(280);
    rightW->setMaximumWidth(360);
    auto *rightLay = new QVBoxLayout(rightW);
    rightLay->setContentsMargins(12,12,12,12);
    rightLay->setSpacing(8);

    auto *logTitle = new QLabel("REGISTRO DE OPERACIONES");
    logTitle->setObjectName("sectionTitle");
    rightLay->addWidget(logTitle);

    m_logView = new QTextEdit(rightW);
    m_logView->setReadOnly(true);
    m_logView->setPlaceholderText("El log aparecerá aquí...");
    rightLay->addWidget(m_logView, 1);

    m_lblStatus = new QLabel("Listo para analizar", rightW);
    m_lblStatus->setObjectName("statLabel");
    rightLay->addWidget(m_lblStatus);

    m_progress = new QProgressBar(rightW);
    m_progress->setTextVisible(true);
    m_progress->setValue(0);
    m_progress->setFixedHeight(14);
    rightLay->addWidget(m_progress);

    m_splitter->addWidget(rightW);
    m_splitter->setStretchFactor(0, 3);
    m_splitter->setStretchFactor(1, 1);

    root->addWidget(m_splitter, 1);
}

void OptimizerPage::populateTree() {
    m_tree->blockSignals(true);
    m_tree->clear();

    for (auto *cleaner : m_cleaners) {
        auto *catItem = new QTreeWidgetItem(m_tree);
        catItem->setText(COL_NAME, cleaner->name());
        catItem->setText(COL_DESC, cleaner->description());
        catItem->setCheckState(COL_NAME, Qt::Checked);
        QFont f = catItem->font(COL_NAME);
        f.setWeight(QFont::Medium);
        catItem->setFont(COL_NAME, f);
        catItem->setData(COL_NAME, Qt::UserRole, "category");

        for (auto entry : cleaner->entries()) {
            auto *item = new QTreeWidgetItem(catItem);
            item->setText(COL_NAME, entry.name);
            item->setText(COL_DESC, entry.description);
            item->setText(COL_PATH, entry.path);
            item->setText(COL_SIZE, entry.analyzed ?
                FileUtils::formatSize(entry.sizeBytes) : "—");
            item->setText(COL_ROOT, entry.requiresRoot ? "🔒 sudo" : "");
            item->setCheckState(COL_NAME, entry.selected ? Qt::Checked : Qt::Unchecked);
            item->setData(COL_NAME, Qt::UserRole, "entry");
            item->setData(COL_NAME, Qt::UserRole+1, entry.id);
            item->setData(COL_NAME, Qt::UserRole+2, QVariant::fromValue(cleaner));
        }
    }

    m_tree->expandAll();
    m_tree->blockSignals(false);
}

void OptimizerPage::onAnalyze() {
    if (m_analyzing || m_cleaning) return;
    m_analyzing = true;
    m_btnAnalyze->setEnabled(false);
    m_btnClean->setEnabled(false);
    m_progress->setValue(0);
    m_lblStatus->setText("Iniciando análisis...");
    appendLog(QString("[%1] === Análisis iniciado ===").arg(
        QDateTime::currentDateTime().toString("hh:mm:ss")));

    connect(m_watcher, &QFutureWatcher<void>::finished,
            this, &OptimizerPage::onAnalyzeFinished, Qt::UniqueConnection);

    auto future = QtConcurrent::run([this]() {
        m_perfCleaner->analyze();
        QMetaObject::invokeMethod(m_progress, "setValue", Qt::QueuedConnection, Q_ARG(int, 100));
    });
    m_watcher->setFuture(future);
}

void OptimizerPage::onAnalyzeFinished() {
    m_analyzing = false;
    m_btnAnalyze->setEnabled(true);
    m_btnClean->setEnabled(true);
    m_lblStatus->setText("Análisis completo. Listo para optimizar.");
    appendLog(QString("[%1] === Análisis completado ===").arg(
        QDateTime::currentDateTime().toString("hh:mm:ss")));
    populateTree();
    updateSummary();
}

void OptimizerPage::onClean() {
    if (m_analyzing || m_cleaning) return;

    // Collect selected entries
    QList<CleanEntry> toClean;
    qint64 totalSelected = 0;

    for (const auto &e : m_perfCleaner->entries()) {
        auto *catItem = m_tree->topLevelItem(0);
        for (int j = 0; j < catItem->childCount(); ++j) {
            auto *child = catItem->child(j);
            if (child->data(COL_NAME, Qt::UserRole+1).toString() == e.id &&
                child->checkState(COL_NAME) == Qt::Checked) {
                toClean << e;
                totalSelected += e.sizeBytes;
                break;
            }
        }
    }

    if (toClean.isEmpty()) {
        QMessageBox::information(this, "Nada seleccionado",
            "Selecciona al menos un elemento para optimizar.");
        return;
    }

    QString confirmMsg = QString(
        "¿Estás seguro de que deseas optimizar los elementos seleccionados?\n\n"
        "Se liberarán aproximadamente %1 de espacio.\n\n"
        "Esta acción puede afectar el rendimiento del sistema.")
        .arg(FileUtils::formatSize(totalSelected));

    if (QMessageBox::question(this, "Confirmar Optimización", confirmMsg,
        QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) return;

    m_cleaning = true;
    m_totalFreed = 0;
    m_btnAnalyze->setEnabled(false);
    m_btnClean->setEnabled(false);
    m_progress->setValue(0);
    appendLog(QString("[%1] === Optimización iniciada ===").arg(
        QDateTime::currentDateTime().toString("hh:mm:ss")));

    connect(m_watcher, &QFutureWatcher<void>::finished,
            this, &OptimizerPage::onCleanFinished, Qt::UniqueConnection);

    auto future = QtConcurrent::run([this, toClean]() {
        qint64 freed = m_perfCleaner->clean(toClean);
        m_totalFreed += freed;
        QMetaObject::invokeMethod(m_progress, "setValue", Qt::QueuedConnection, Q_ARG(int, 100));
        QMetaObject::invokeMethod(this, "appendLog", Qt::QueuedConnection,
            Q_ARG(QString, QString("[Optimizer] Performance Cleaner: liberados %1")
                .arg(FileUtils::formatSize(freed))));
    });
    m_watcher->setFuture(future);
}

void OptimizerPage::onCleanFinished() {
    m_cleaning = false;
    m_btnAnalyze->setEnabled(true);
    m_btnClean->setEnabled(false);
    m_progress->setValue(100);
    m_lblStatus->setText(QString("✅ Optimización completa — Liberados %1")
        .arg(FileUtils::formatSize(m_totalFreed)));
    appendLog(QString("[%1] === Optimización completa: %2 liberados ===")
        .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
        .arg(FileUtils::formatSize(m_totalFreed)));
    emit totalFreedChanged(m_totalFreed);

    QMessageBox::information(this, "Optimización Completada",
        QString("✅ Se liberaron %1 de espacio en disco.")
            .arg(FileUtils::formatSize(m_totalFreed)));
    populateTree();
    updateSummary();
}

void OptimizerPage::onSelectAll() {
    m_tree->blockSignals(true);
    for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
        auto *cat = m_tree->topLevelItem(i);
        cat->setCheckState(COL_NAME, Qt::Checked);
        for (int j = 0; j < cat->childCount(); ++j)
            cat->child(j)->setCheckState(COL_NAME, Qt::Checked);
    }
    m_tree->blockSignals(false);
    updateSummary();
}

void OptimizerPage::onSelectNone() {
    m_tree->blockSignals(true);
    for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
        auto *cat = m_tree->topLevelItem(i);
        cat->setCheckState(COL_NAME, Qt::Unchecked);
        for (int j = 0; j < cat->childCount(); ++j)
            cat->child(j)->setCheckState(COL_NAME, Qt::Unchecked);
    }
    m_tree->blockSignals(false);
    updateSummary();
}

void OptimizerPage::onItemChanged(QTreeWidgetItem *item, int col) {
    if (col != COL_NAME) return;
    m_tree->blockSignals(true);

    // If parent changed, propagate to children
    if (item->data(COL_NAME, Qt::UserRole).toString() == "category") {
        Qt::CheckState state = item->checkState(COL_NAME);
        for (int j = 0; j < item->childCount(); ++j)
            item->child(j)->setCheckState(COL_NAME, state);
    }
    // If child changed, update parent
    else if (item->parent()) {
        int checkedCount = 0;
        auto *parent = item->parent();
        for (int j = 0; j < parent->childCount(); ++j)
            if (parent->child(j)->checkState(COL_NAME) == Qt::Checked) checkedCount++;

        if (checkedCount == 0)
            parent->setCheckState(COL_NAME, Qt::Unchecked);
        else if (checkedCount == parent->childCount())
            parent->setCheckState(COL_NAME, Qt::Checked);
        else
            parent->setCheckState(COL_NAME, Qt::PartiallyChecked);
    }

    m_tree->blockSignals(false);
    updateSummary();
}

void OptimizerPage::updateSummary() {
    qint64 selected = 0, total = 0;
    for (const auto &e : m_perfCleaner->entries()) {
        total += e.sizeBytes;
        // Check if selected in tree
        auto *catItem = m_tree->topLevelItem(0);
        for (int j = 0; j < catItem->childCount(); ++j) {
            auto *child = catItem->child(j);
            if (child->data(COL_NAME, Qt::UserRole+1).toString() == e.id &&
                child->checkState(COL_NAME) == Qt::Checked) {
                selected += e.sizeBytes;
            }
        }
    }
    m_lblSelectedSize->setText(QString("Seleccionado: %1").arg(FileUtils::formatSize(selected)));
    m_lblTotal->setText(QString("Total analizado: %1").arg(FileUtils::formatSize(total)));
}

void OptimizerPage::appendLog(const QString &msg) {
    m_logView->append(msg);
    auto *sb = m_logView->verticalScrollBar();
    sb->setValue(sb->maximum());
}
