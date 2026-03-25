# 🧹 StellarCleaner v1.0.0

**Limpiador profesional del sistema Linux — C++17 + Qt6**

> Alternativa a CCleaner diseñada nativamente para Linux, con interfaz oscura moderna, limpieza inteligente y análisis de disco.

---

## ✨ Características

### 🧹 Limpiador del Sistema
- **Temporales**: `/tmp`, `/var/tmp`, `~/.cache`, miniaturas
- **Navegadores**: Firefox, Chrome, Chromium, Brave, Opera, Vivaldi, Edge
  - Limpia caché, historial, cookies, sesiones (configurable)
- **Logs**: journald, syslog, kern.log, logs de apps, crash reports
- **Paquetes**: Caché de APT/DNF/Pacman, revisiones viejas de Snap, Flatpak sin usar, paquetes huérfanos
- **Privacidad**: Archivos recientes, historial de bash/zsh/fish, marcadores GTK
- **Papelera**: Usuario, root, drives montados

### 📊 Analizador de Disco
- Mapa visual tipo Treemap con colores por tamaño
- Árbol expandible de directorios ordenado por tamaño
- Doble clic para profundizar en directorios
- Eliminar archivos/carpetas directamente desde el analizador

### 🔧 Herramientas
- **Inicio automático**: Ver y habilitar/deshabilitar apps de autostart y servicios systemd
- **Gestor de procesos**: Ver todos los procesos con CPU/RAM, terminar procesos
- **Desinstalador**: Listar y desinstalar apps (APT, Snap, Flatpak) con búsqueda

### ⚙️ Configuración
- Limpieza automática programada (diaria/semanal/mensual/al inicio)
- Control de días de retención del journal
- Notificaciones del sistema

---

## 📋 Requisitos

| Componente | Versión mínima |
|---|---|
| Linux | Cualquier distro moderna |
| C++ | C++17 |
| CMake | 3.16+ |
| Qt | 6.x (o 5.15+) |
| Qt Charts | Incluido |

### Distros probadas
- Ubuntu 22.04 / 24.04 LTS
- Debian 12
- Fedora 39+
- Arch Linux
- Linux Mint 21+
- openSUSE Tumbleweed

---

## 🚀 Instalación rápida

```bash
# Clonar el repositorio
git clone https://github.com/tu-usuario/stellarcleaner.git
cd stellarcleaner

# Instalar con script automático
chmod +x packaging/install.sh
./packaging/install.sh
```

## 🔨 Compilación manual

```bash
# Ubuntu/Debian
sudo apt install build-essential cmake qt6-base-dev libqt6charts6-dev

# Fedora
sudo dnf install cmake gcc-c++ qt6-qtbase-devel qt6-qtcharts-devel

# Arch Linux
sudo pacman -S cmake base-devel qt6-base qt6-charts

# Compilar
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
```

---

## 📁 Estructura del Proyecto

```
StellarCleaner/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp
│   ├── mainwindow.h/cpp
│   ├── cleaner/
│   │   ├── basecleaner.h/cpp        # Clase base abstracta
│   │   ├── tempcleaner.h/cpp        # Archivos temporales
│   │   ├── browsercleaner.h/cpp     # Navegadores web
│   │   ├── logcleaner.h/cpp         # Logs del sistema
│   │   ├── packagecleaner.h/cpp     # Caché de paquetes
│   │   ├── privacycleaner.h/cpp     # Rastros de privacidad
│   │   └── trashcleaner.h/cpp       # Papelera
│   ├── analyzer/
│   │   └── diskanalyzer.h/cpp       # Analizador + TreeMap
│   ├── tools/
│   │   ├── startupmanager.h/cpp     # Gestor de inicio
│   │   ├── processmanager.h/cpp     # Gestor de procesos
│   │   └── uninstallmanager.h/cpp   # Desinstalador
│   ├── utils/
│   │   ├── fileutils.h/cpp          # Utilidades de archivos
│   │   ├── systeminfo.h/cpp         # Info del sistema
│   │   ├── sudohelper.h/cpp         # Operaciones con sudo
│   │   └── scheduler.h/cpp          # Programador automático
│   └── ui/
│       ├── sidebar.h/cpp            # Barra lateral
│       ├── titlebar.h/cpp           # Barra de título
│       ├── dashboard.h/cpp          # Panel principal
│       ├── cleanerpage.h/cpp        # Página limpiador
│       ├── analyzerpage.h/cpp       # Página analizador
│       ├── toolspage.h/cpp          # Página herramientas
│       ├── settingspage.h/cpp       # Página ajustes
│       └── logviewer.h/cpp          # Visor de logs
├── resources/
│   ├── resources.qrc
│   ├── icons/
│   └── styles/dark.qss              # Tema oscuro completo
└── packaging/
    ├── stellarcleaner.desktop         # Entrada de menú
    ├── install.sh                   # Instalador automático
    └── uninstall.sh                 # Desinstalador
```

---

## ⚠️ Seguridad

- **Nunca** toca directorios del sistema (`/bin`, `/usr/bin`, `/etc`, etc.)
- Operaciones privilegiadas usan `sudo -n` (sin contraseña guardada)
- Confirmación antes de cualquier limpieza
- El código fuente es 100% auditable

---

## 📜 Licencia

MIT License — Libre para uso personal y comercial.
