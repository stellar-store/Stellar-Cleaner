#include "logviewer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QLabel>
#include <QComboBox>
#include <QScrollBar>

LogViewer::LogViewer(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(4);

    auto *toolbar = new QHBoxLayout();
    auto *titleLbl = new QLabel("Log de Operaciones", this);
    titleLbl->setObjectName("sectionTitle");
    toolbar->addWidget(titleLbl);
    toolbar->addStretch();

    m_levelFilter = new QComboBox(this);
    m_levelFilter->addItems({"Todos","INFO","WARN","ERROR"});
    toolbar->addWidget(m_levelFilter);

    auto *btnClear = new QPushButton("Limpiar", this);
    btnClear->setObjectName("btnSecondary");
    btnClear->setFixedWidth(80);
    toolbar->addWidget(btnClear);
    root->addLayout(toolbar);

    m_edit = new QPlainTextEdit(this);
    m_edit->setReadOnly(true);
    root->addWidget(m_edit, 1);

    connect(btnClear, &QPushButton::clicked, this, &LogViewer::clear);
}

void LogViewer::append(const QString &msg, const QString &level) {
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString line = QString("[%1] [%2] %3").arg(ts, level, msg);
    m_entries << qMakePair(level, line);

    QString current = m_levelFilter->currentText();
    if (current == "Todos" || current == level)
        m_edit->appendPlainText(line);

    auto *sb = m_edit->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void LogViewer::clear() {
    m_entries.clear();
    m_edit->clear();
}
