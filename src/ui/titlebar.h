#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>

class TitleBar : public QWidget {
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = nullptr);
    void setTitle(const QString &title);
    void setFreedBytes(qint64 bytes);
    void setMaximized(bool maximized);

signals:
    void minimizeRequested();
    void maximizeRequested();
    void closeRequested();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    QLabel *m_titleLbl;
    QLabel *m_freedLbl;
    QPushButton *m_minimizeBtn;
    QPushButton *m_maximizeBtn;
    QPushButton *m_closeBtn;
    bool m_isMaximized = false;
    bool m_isDragging = false;
    QPoint m_dragStart;
};
