#include "titlebar.h"
#include "../utils/fileutils.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QStyle>
#include <QMouseEvent>
#include <QApplication>
#include <QMainWindow>

TitleBar::TitleBar(QWidget *parent) : QWidget(parent) {
    setObjectName("titleBar");
    setFixedHeight(32);
    setStyleSheet("QWidget#titleBar { background:#0a0d14; border-bottom:1px solid #1e2535; }");

    auto *lay = new QHBoxLayout(this);
    lay->setContentsMargins(12,0,12,0);
    lay->setSpacing(8);

    lay->addSpacing(8);
    m_titleLbl = new QLabel("Stellar", this);
    QFont f = m_titleLbl->font();
    f.setPointSize(11);
    f.setWeight(QFont::Medium);
    m_titleLbl->setFont(f);
    m_titleLbl->setStyleSheet("color:#037df8;");
    lay->addWidget(m_titleLbl);

    lay->addStretch();

    m_freedLbl = new QLabel("", this);
    m_freedLbl->setStyleSheet("color:#00d4aa; font-size:11px; font-family:monospace;");
    lay->addWidget(m_freedLbl);

    lay->addSpacing(12);

    // Window control buttons on the right side
    m_minimizeBtn = new QPushButton(this);
    m_minimizeBtn->setFixedSize(28, 28);
    m_minimizeBtn->setText("−");
    m_minimizeBtn->setStyleSheet(
        "QPushButton { background:#ff5f57; border:none; border-radius:14px; color:white; font-weight:bold; }"
        "QPushButton:hover { background:#ff6b63; }"
        "QPushButton:pressed { background:#e84e43; }"
    );
    lay->addWidget(m_minimizeBtn);
    connect(m_minimizeBtn, &QPushButton::clicked, this, &TitleBar::minimizeRequested);

    m_maximizeBtn = new QPushButton(this);
    m_maximizeBtn->setFixedSize(28, 28);
    m_maximizeBtn->setText("□");
    m_maximizeBtn->setStyleSheet(
        "QPushButton { background:#febc2e; border:none; border-radius:14px; color:white; font-weight:bold; }"
        "QPushButton:hover { background:#ffc544; }"
        "QPushButton:pressed { background:#e5a91f; }"
    );
    lay->addWidget(m_maximizeBtn);
    connect(m_maximizeBtn, &QPushButton::clicked, this, &TitleBar::maximizeRequested);

    m_closeBtn = new QPushButton(this);
    m_closeBtn->setFixedSize(28, 28);
    m_closeBtn->setText("✕");
    m_closeBtn->setStyleSheet(
        "QPushButton { background:#28c840; border:none; border-radius:14px; color:white; font-weight:bold; }"
        "QPushButton:hover { background:#32d649; }"
        "QPushButton:pressed { background:#1f9831; }"
    );
    lay->addWidget(m_closeBtn);
    connect(m_closeBtn, &QPushButton::clicked, this, &TitleBar::closeRequested);

    lay->addSpacing(8);
}

void TitleBar::setTitle(const QString &title) { 
    m_titleLbl->setText(title); 
}

void TitleBar::setFreedBytes(qint64 bytes) {
    if (bytes > 0)
        m_freedLbl->setText(QString("✅ Liberados %1").arg(FileUtils::formatSize(bytes)));
}

void TitleBar::setMaximized(bool maximized) {
    m_isMaximized = maximized;
    if (maximized) {
        m_maximizeBtn->setText("❑");  // Restore icon
        m_maximizeBtn->setToolTip("Restaurar");
    } else {
        m_maximizeBtn->setText("□");  // Maximize icon
        m_maximizeBtn->setToolTip("Maximizar");
    }
}

void TitleBar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        m_dragStart = event->globalPosition().toPoint() - window()->frameGeometry().topLeft();
        event->accept();
    }
}

void TitleBar::mouseMoveEvent(QMouseEvent *event) {
    if (m_isDragging) {
        window()->move(event->globalPosition().toPoint() - m_dragStart);
        event->accept();
    }
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit maximizeRequested();
        event->accept();
    }
}
