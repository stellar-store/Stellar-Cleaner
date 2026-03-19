#pragma once
#include <QWidget>
#include <QButtonGroup>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class Sidebar : public QWidget {
    Q_OBJECT
public:
    explicit Sidebar(QWidget *parent = nullptr);
    void setActiveIndex(int idx);

signals:
    void pageRequested(int index);

private:
    void setupUI();
    QButtonGroup *m_btnGroup;
    QList<QPushButton*> m_buttons;
};
