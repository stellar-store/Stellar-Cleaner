#pragma once
#include <QString>
#include <QMap>
#include <QObject>

enum class Language {
    Spanish,
    Portuguese,
    English
};

class LanguageManager : public QObject {
    Q_OBJECT
public:
    static LanguageManager& instance();
    
    void setLanguage(Language lang);
    Language currentLanguage() const { return m_currentLanguage; }
    
    QString tr(const QString& key) const;
    QString langName(Language lang) const;
    
signals:
    void languageChanged();
    
private:
    LanguageManager();
    void loadTranslations();
    
    Language m_currentLanguage = Language::Spanish;
    QMap<QString, QMap<int, QString>> m_translations;
};

#define TR(key) LanguageManager::instance().tr(key)
