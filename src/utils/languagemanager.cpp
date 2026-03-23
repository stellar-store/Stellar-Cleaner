#include "languagemanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QStandardPaths>

LanguageManager& LanguageManager::instance() {
    static LanguageManager mgr;
    return mgr;
}

LanguageManager::LanguageManager() {
    loadTranslations();
}

void LanguageManager::setLanguage(Language lang) {
    if (m_currentLanguage != lang) {
        m_currentLanguage = lang;
        emit languageChanged();
    }
}

QString LanguageManager::tr(const QString& key) const {
    if (!m_translations.contains(key)) {
        return key;  // Return key if translation not found
    }
    return m_translations[key].value(static_cast<int>(m_currentLanguage), key);
}

QString LanguageManager::langName(Language lang) const {
    switch (lang) {
        case Language::Spanish: return "Español";
        case Language::Portuguese: return "Português";
        case Language::English: return "English";
    }
    return "Unknown";
}

void LanguageManager::loadTranslations() {
    m_translations = {
        // Application
        {"app.title", {{0, "Stellar Cleaner"}, {1, "Stellar Cleaner"}, {2, "Stellar Cleaner"}}},
        {"app.version", {{0, "v1.0.0"}, {1, "v1.0.0"}, {2, "v1.0.0"}}},
        {"app.description", {{0, "Un potente limpiador de sistema para Linux"}, 
                            {1, "Um limpador de sistema poderoso para Linux"}, 
                            {2, "A powerful system cleaner for Linux"}}},
        
        // Main window titles
        {"mainwindow.dashboard", {{0, "Panel de Control"}, {1, "Painel de Controle"}, {2, "Dashboard"}}},
        {"mainwindow.cleaner", {{0, "Limpiador del Sistema"}, {1, "Limpador do Sistema"}, {2, "System Cleaner"}}},
        {"mainwindow.analyzer", {{0, "Analizador de Disco"}, {1, "Analisador de Disco"}, {2, "Disk Analyzer"}}},
        {"mainwindow.tools", {{0, "Herramientas"}, {1, "Ferramentas"}, {2, "Tools"}}},
        {"mainwindow.settings", {{0, "Configuración"}, {1, "Configurações"}, {2, "Settings"}}},
        
        // Status bar
        {"status.freed", {{0, "✅ Liberados esta sesión: %1"}, 
                         {1, "✅ Liberados nesta sessão: %1"}, 
                         {2, "✅ Freed this session: %1"}}},
        {"status.about", {{0, "ℹ️ Acerca de"}, {1, "ℹ️ Sobre"}, {2, "ℹ️ About"}}},
        
        // About dialog
        {"about.title", {{0, "Acerca de Stellar Cleaner"}, {1, "Sobre Stellar Cleaner"}, {2, "About Stellar Cleaner"}}},
        {"about.features.temp", {{0, "🧹 Limpeza de arquivos temporários"}, 
                                {1, "🧹 Limpeza de arquivos temporários"}, 
                                {2, "🧹 Temporary file cleanup"}}},
        {"about.features.disk", {{0, "🔍 Análisis completo del disco"}, 
                                {1, "🔍 Análise completa do disco"}, 
                                {2, "🔍 Complete disk analysis"}}},
        {"about.features.browser", {{0, "🌐 Limpeza de navegadores y caché"}, 
                                   {1, "🌐 Limpeza de navegadores e cache"}, 
                                   {2, "🌐 Browser and cache cleanup"}}},
        {"about.features.packages", {{0, "📦 Gestor de paquetes"}, 
                                    {1, "📦 Gerenciador de pacotes"}, 
                                    {2, "📦 Package manager"}}},
        {"about.features.tools", {{0, "🛠️ Herramientas avanzadas del sistema"}, 
                                 {1, "🛠️ Ferramentas avançadas do sistema"}, 
                                 {2, "🛠️ Advanced system tools"}}},
        {"about.copyright", {{0, "© 2026 - Todos los derechos reservados"}, 
                            {1, "© 2026 - Todos os direitos reservados"}, 
                            {2, "© 2026 - All rights reserved"}}},
        
        // Warning messages
        {"warning.root.title", {{0, "Advertencia"}, {1, "Aviso"}, {2, "Warning"}}},
        {"warning.root.message", {{0, "Estás ejecutando Stellar Cleaner como root.\nTodas las operaciones se ejecutarán con privilegios máximos.\n\n¿Deseas continuar?"}, 
                                 {1, "Você está executando Stellar Cleaner como root.\nTodas as operações serão executadas com privilégios máximos.\n\n¿Deseja continuar?"}, 
                                 {2, "You are running Stellar Cleaner as root.\nAll operations will be executed with maximum privileges.\n\nDo you want to continue?"}}},
        
        // Error messages
        {"error.unsupported.title", {{0, "Sistema no compatible"}, {1, "Sistema não suportado"}, {2, "Unsupported System"}}},
        {"error.unsupported.message", {{0, "Stellar Cleaner solo funciona en sistemas Linux."}, 
                                      {1, "Stellar Cleaner funciona apenas em sistemas Linux."}, 
                                      {2, "Stellar Cleaner only works on Linux systems."}}},
        
        // Settings
        {"settings.language", {{0, "Idioma"}, {1, "Idioma"}, {2, "Language"}}},
        {"settings.theme", {{0, "Tema"}, {1, "Tema"}, {2, "Theme"}}},
        {"settings.autostart", {{0, "Iniciar automáticamente"}, {1, "Iniciar automaticamente"}, {2, "Auto-start"}}},
        
        // Buttons
        {"button.ok", {{0, "Aceptar"}, {1, "Aceitar"}, {2, "OK"}}},
        {"button.cancel", {{0, "Cancelar"}, {1, "Cancelar"}, {2, "Cancel"}}},
        {"button.clean", {{0, "Limpiar"}, {1, "Limpar"}, {2, "Clean"}}},
        {"button.analyze", {{0, "Analizar"}, {1, "Analisar"}, {2, "Analyze"}}},
        {"button.apply", {{0, "Aplicar"}, {1, "Aplicar"}, {2, "Apply"}}},
    };
}
