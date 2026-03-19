#pragma once
#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QComboBox>

class LogViewer : public QWidget {
    Q_OBJECT
public:
    explicit LogViewer(QWidget *parent = nullptr);
    void append(const QString &msg, const QString &level = "INFO");
    void clear();

private:
    QPlainTextEdit *m_edit;
    QComboBox      *m_levelFilter;
    QList<QPair<QString,QString>> m_entries;
};
