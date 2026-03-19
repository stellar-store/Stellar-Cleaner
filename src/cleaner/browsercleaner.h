#pragma once
#include "basecleaner.h"
#include <QMap>

class BrowserCleaner : public BaseCleaner {
    Q_OBJECT
public:
    explicit BrowserCleaner(QObject *parent = nullptr);

    QString name()        const override { return "Navegadores"; }
    QString description() const override { return "Limpia caché, historial y cookies de navegadores"; }

    QList<CleanEntry> entries() override { return m_entries; }
    void analyze()  override;
    qint64 clean(const QList<CleanEntry> &selected) override;

    // Per-browser options
    struct BrowserOptions {
        bool cleanCache    = true;
        bool cleanHistory  = false;
        bool cleanCookies  = false;
        bool cleanSessions = false;
        bool cleanPasswords = false;
    };
    void setOptions(const QString &browserId, const BrowserOptions &opts);

private:
    void detectBrowsers();
    void analyzeFirefox(CleanEntry &entry);
    void analyzeChromium(CleanEntry &entry);

    qint64 cleanFirefox(const CleanEntry &e, const BrowserOptions &opts);
    qint64 cleanChromium(const CleanEntry &e, const BrowserOptions &opts);

    QMap<QString, BrowserOptions> m_options;
};
