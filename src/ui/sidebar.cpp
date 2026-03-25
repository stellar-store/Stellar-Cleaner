#include "sidebar.h"
#include <QVBoxLayout>
#include <QFont>
#include <QApplication>
#include <QStyle>
#include <QIcon>
#include <QSize>

Sidebar::Sidebar(QWidget *parent) : QWidget(parent) {
    setObjectName("sidebar");
    setFixedWidth(210);
    setupUI();
}

void Sidebar::setupUI() {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // Logo
    auto *logoW = new QWidget(this);
    logoW->setObjectName("sidebarLogo");
    logoW->setFixedHeight(80);
    auto *logoLay = new QVBoxLayout(logoW);
    logoLay->setContentsMargins(16,12,16,12);
    logoLay->setSpacing(2);
    auto *iconLbl = new QLabel("Stellar Cleaner", logoW);
    QFont f = iconLbl->font();
    f.setPointSize(13);
    f.setBold(true);
    iconLbl->setFont(f);
    iconLbl->setStyleSheet("color:#037df8;");
    auto *verLbl = new QLabel("v1.0.0 — C++ Qt6", logoW);
    verLbl->setObjectName("navSection");
    verLbl->setWordWrap(true);
    logoLay->addWidget(iconLbl);
    logoLay->addWidget(verLbl);
    root->addWidget(logoW);

    m_btnGroup = new QButtonGroup(this);
    m_btnGroup->setExclusive(true);

    struct NavItem { QString icon; QString label; };
    QList<NavItem> sections = {
        {"", "PRINCIPAL"},
        {":/icons/home.png", "Panel de Control"},
        {":/icons/clean.png", "Limpiador"},
        {":/icons/analyze.png", "Analizador de Disco"},
        {"", "OPTIMIZACIÓN"},
        {":/icons/kernel.png", "Limpiador del Kernel"},
        {":/icons/optimizacion.png", "Optimizador de Rendimiento"},
        {"", "HERRAMIENTAS"},
        {":/icons/tools.png", "Herramientas"},
        {"", "SISTEMA"},
        {":/icons/settings.png", "Configuración"},
    };

    int pageIdx = 0;
    auto *navW = new QWidget(this);
    auto *navLay = new QVBoxLayout(navW);
    navLay->setContentsMargins(0,8,0,8);
    navLay->setSpacing(1);

    for (const auto &item : sections) {
        if (item.icon.isEmpty()) {
            auto *secLbl = new QLabel(item.label, navW);
            secLbl->setObjectName("navSection");
            navLay->addWidget(secLbl);
        } else {
            auto *btn = new QPushButton(navW);
            btn->setObjectName("navButton");
            btn->setCheckable(true);
            btn->setFixedHeight(38);
            btn->setCursor(Qt::PointingHandCursor);
            btn->setProperty("pageIdx", pageIdx);
            
            // Set icon with text
            QIcon icon(item.icon);
            btn->setIcon(icon);
            btn->setIconSize(QSize(20, 20));
            btn->setText("  " + item.label);
            
            m_btnGroup->addButton(btn, pageIdx);
            m_buttons << btn;
            navLay->addWidget(btn);
            connect(btn, &QPushButton::clicked, this, [this, pageIdx]() {
                emit pageRequested(pageIdx);
            });
            pageIdx++;
        }
    }
    navLay->addStretch();
    root->addWidget(navW, 1);

    // Bottom info
    auto *botW = new QWidget(this);
    botW->setFixedHeight(44);
    auto *botLay = new QVBoxLayout(botW);
    botLay->setContentsMargins(16,8,16,8);
    auto *botLbl = new QLabel("Open source · Sin telemetría", botW);
    botLbl->setObjectName("navSection");
    botLbl->setWordWrap(true);
    botLay->addWidget(botLbl);
    root->addWidget(botW);

    if (!m_buttons.isEmpty()) {
        m_buttons[0]->setChecked(true);
        m_buttons[0]->setProperty("active","true");
    }
}

void Sidebar::setActiveIndex(int idx) {
    for (auto *btn : m_buttons) {
        bool active = (btn->property("pageIdx").toInt() == idx);
        btn->setChecked(active);
        btn->setProperty("active", active ? "true" : "false");
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }
}
