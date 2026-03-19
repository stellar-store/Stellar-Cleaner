#include "browsercleaner.h"
#include "../utils/fileutils.h"
#include <QDir>
#include <QFile>
#include <QDirIterator>

BrowserCleaner::BrowserCleaner(QObject *parent) : BaseCleaner(parent) {
    detectBrowsers();
}

void BrowserCleaner::detectBrowsers() {
    QString home = QDir::homePath();

    // Browser definitions: id, name, base path, type
    struct BrowserDef {
        QString id;
        QString name;
        QString path;
        QString type; // "firefox" or "chromium"
    };

    QList<BrowserDef> defs = {
        // Firefox family
        {"firefox",     "Firefox",       home + "/.mozilla/firefox",              "firefox"},
        {"firefox_snap","Firefox (Snap)", home + "/snap/firefox/common/.mozilla/firefox", "firefox"},

        // Chromium family
        {"chrome",   "Google Chrome", home + "/.config/google-chrome",    "chromium"},
        {"chromium", "Chromium",      home + "/.config/chromium",          "chromium"},
        {"brave",    "Brave",         home + "/.config/BraveSoftware/Brave-Browser", "chromium"},
        {"opera",    "Opera",         home + "/.config/opera",             "chromium"},
        {"vivaldi",  "Vivaldi",       home + "/.config/vivaldi",           "chromium"},
        {"edge",     "Microsoft Edge",home + "/.config/microsoft-edge",    "chromium"},
    };

    for (const auto &def : defs) {
        if (!QDir(def.path).exists()) continue;
        CleanEntry e;
        e.id          = def.id;
        e.name        = def.name;
        e.description = (def.type == "firefox") ?
            "Caché, sesiones, historial de navegación" :
            "Caché web, datos temporales de sesión";
        e.path        = def.path;
        e.category    = "Navegadores";
        e.selected    = true;
        m_entries << e;

        // Default options
        m_options[def.id] = {true, false, false, false, false};
    }
}

void BrowserCleaner::analyze() {
    m_analyzing = true;
    emit analyzeStarted();

    int total = m_entries.size();
    for (int i = 0; i < total; ++i) {
        auto &e = m_entries[i];
        emit analyzeProgress((i * 100) / total, e.name);

        // Calculate only cache dirs for speed
        qint64 size = 0;
        QDirIterator it(e.path, QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            QString name = it.fileName();
            // Count cache-related directories
            if (name.contains("Cache", Qt::CaseInsensitive) ||
                name.contains("cache", Qt::CaseInsensitive) ||
                name == "Code Cache" ||
                name == "GPU Cache" ||
                name == "ShaderCache" ||
                name == "DawnCache" ||
                name == "Sessions" ||
                name == "storage" ||
                name == "logs") {
                size += FileUtils::dirSize(it.filePath());
            }
        }
        e.sizeBytes = size;
        e.analyzed  = true;
    }

    m_analyzing = false;
    emit analyzeFinished(totalSize());
}

void BrowserCleaner::setOptions(const QString &browserId, const BrowserOptions &opts) {
    m_options[browserId] = opts;
}

qint64 BrowserCleaner::clean(const QList<CleanEntry> &selected) {
    m_cleaning = true;
    emit cleanStarted();
    qint64 freed = 0;

    // Detect browser type
    QMap<QString,QString> types = {
        {"firefox","firefox"}, {"firefox_snap","firefox"},
        {"chrome","chromium"}, {"chromium","chromium"}, {"brave","chromium"},
        {"opera","chromium"}, {"vivaldi","chromium"}, {"edge","chromium"}
    };

    int total = selected.size();
    for (int i = 0; i < total; ++i) {
        const auto &e = selected[i];
        emit cleanProgress((i * 100) / total, QString("Limpiando %1...").arg(e.name));
        emitLog(QString("[Browser] Cleaning %1: %2").arg(e.name, e.path));

        auto opts = m_options.value(e.id, {true, false, false, false, false});
        QString type = types.value(e.id, "chromium");

        if (type == "firefox")
            freed += cleanFirefox(e, opts);
        else
            freed += cleanChromium(e, opts);
    }

    m_cleaning = false;
    emit cleanFinished(freed);
    return freed;
}

qint64 BrowserCleaner::cleanFirefox(const CleanEntry &e, const BrowserOptions &opts) {
    qint64 freed = 0;
    // Firefox stores profiles in base dir
    QDir baseDir(e.path);
    auto profiles = baseDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const auto &profile : profiles) {
        if (!profile.fileName().contains("default") &&
            !profile.fileName().contains("release")) continue;

        QString profilePath = profile.absoluteFilePath();

        if (opts.cleanCache) {
            freed += FileUtils::deleteDir(profilePath + "/cache2",  true, false);
            freed += FileUtils::deleteDir(profilePath + "/startupCache", true, false);
            freed += FileUtils::deleteDir(profilePath + "/thumbnails", true, false);
            freed += FileUtils::deleteDir(profilePath + "/storage/default", true, false);
            freed += FileUtils::deleteFilesInDir(profilePath + "/crashes/events");
        }
        if (opts.cleanHistory) {
            freed += FileUtils::deleteFile(profilePath + "/places.sqlite");
            freed += FileUtils::deleteFile(profilePath + "/formhistory.sqlite");
        }
        if (opts.cleanCookies) {
            freed += FileUtils::deleteFile(profilePath + "/cookies.sqlite");
            freed += FileUtils::deleteFile(profilePath + "/cookies.sqlite-shm");
            freed += FileUtils::deleteFile(profilePath + "/cookies.sqlite-wal");
        }
        if (opts.cleanSessions) {
            freed += FileUtils::deleteDir(profilePath + "/sessionstore-backups", true, false);
            freed += FileUtils::deleteFile(profilePath + "/sessionCheckpoints.json");
        }
    }
    return freed;
}

qint64 BrowserCleaner::cleanChromium(const CleanEntry &e, const BrowserOptions &opts) {
    qint64 freed = 0;

    // Find profile dirs (Default, Profile 1, Profile 2, ...)
    QDir base(e.path);
    QStringList profileDirs;
    profileDirs << "Default";
    for (const auto &d : base.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (d.fileName().startsWith("Profile ")) profileDirs << d.fileName();
    }

    for (const auto &pd : profileDirs) {
        QString profilePath = e.path + "/" + pd;
        if (!QDir(profilePath).exists()) continue;

        if (opts.cleanCache) {
            freed += FileUtils::deleteDir(profilePath + "/Cache",      true, false);
            freed += FileUtils::deleteDir(profilePath + "/Code Cache", true, false);
            freed += FileUtils::deleteDir(profilePath + "/GPU Cache",  true, false);
            freed += FileUtils::deleteDir(profilePath + "/ShaderCache",true, false);
            freed += FileUtils::deleteDir(profilePath + "/DawnCache",  true, false);
            freed += FileUtils::deleteFilesInDir(profilePath + "/logs");
        }
        if (opts.cleanHistory) {
            freed += FileUtils::deleteFile(profilePath + "/History");
            freed += FileUtils::deleteFile(profilePath + "/History-journal");
            freed += FileUtils::deleteFile(profilePath + "/Visited Links");
        }
        if (opts.cleanCookies) {
            freed += FileUtils::deleteFile(profilePath + "/Cookies");
            freed += FileUtils::deleteFile(profilePath + "/Cookies-journal");
        }
        if (opts.cleanSessions) {
            freed += FileUtils::deleteFile(profilePath + "/Current Session");
            freed += FileUtils::deleteFile(profilePath + "/Current Tabs");
            freed += FileUtils::deleteDir(profilePath + "/Sessions", true, false);
        }
    }

    // Global cache dir
    freed += FileUtils::deleteDir(e.path + "/ShaderCache", true, false);

    return freed;
}
