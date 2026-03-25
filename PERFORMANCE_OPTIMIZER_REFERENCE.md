# ⚡ Quick Reference: Performance Optimizer

## 📦 Archivos Agregados

```
src/cleaner/
├── performancecleaner.h      (61 líneas)    ← Nueva clase
└── performancecleaner.cpp    (269 líneas)   ← Implementación

Documentación/
├── PERFORMANCE_OPTIMIZER_SETUP.md    (8KB)  ← Técnica
├── PERFORMANCE_OPTIMIZER_USAGE.md    (8KB)  ← Usuario
└── CHANGELOG_PERFORMANCE_OPTIMIZER.md (8KB)  ← Cambios
```

## 🎮 4 Optimizaciones

| # | Nombre | Sistema | Efecto | Root |
|---|--------|---------|--------|------|
| 1 | CPU Governor | scaling_governor → performance | ↑↑ +30-50% CPU | ✅ |
| 2 | Turbo Boost | Intel/AMD turbo [on/off] | ↑ Freq máx | ✅ |
| 3 | Process Priority | setpriority(pid, -5) | ↑ Respuesta | ~ |
| 4 | Swap Opt | swappiness 60→10 | ↓ Latencia | ✅ |

## 🚀 Inicio Rápido

```bash
# Compilar
cd /home/gus/Downloads/Cleaner/build_perf
make

# Ejecutar (con permisos root)
sudo ./StellarCleaner

# En la UI:
# 1. Panel izq → "Limpiador del Sistema"
# 2. Expandir → "Optimizador de Latencia"
# 3. Click → "Analizar"
# 4. Select → Todas [✓]
# 5. Click → "Limpiar Ahora"
```

## 💻 Uso Programático

```cpp
#include "cleaner/performancecleaner.h"

PerformanceCleaner perf;

// Analizar
perf.analyze();

// Activar TODO
perf.activatePerformanceMode();

// O seleccionar específicamente
QList<CleanEntry> selected = perf.entries();
selected.removeIf([](const CleanEntry& e) { 
    return e.id != "cpu_governor"; // solo CPU governor
});
perf.clean(selected);

// Leer información
int cpus = perf.getCPUCount();
QString gov = perf.getCPUGovernor();
QStringList available = perf.getAvailableGovernors();

// Ajustar proceso específico
perf.setProcPriority(12345, -5));  // PID 12345, prioridad -5

// Desactivar
perf.deactivatePerformanceMode();
```

## 🔍 Verificación Manual

```bash
# Ver CPU governor actual
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

# Ver governors disponibles
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors

# Ver turbo boost (Intel)
cat /sys/devices/system/cpu/intel_pstate/no_turbo

# Ver turbo boost (AMD)
cat /sys/devices/system/cpu/cpufreq/boost

# Ver swappiness
cat /proc/sys/vm/swappiness

# Ver prioridad de proceso
ps -eo pid,ni,comm | grep <app>
```

## 📊 Métodos Disponibles

### Análisis
```cpp
void analyze();              // Detecta CPUs y governors
```

### Limpieza
```cpp
qint64 clean(const QList<CleanEntry>&);  // Aplica optimizaciones
```

### CPU Control
```cpp
QString getCPUGovernor();
bool setCPUGovernor(const QString &gov);
QStringList getAvailableGovernors();
int getCPUCount();
```

### Proceso Control
```cpp
bool setProcPriority(int pid, int priority);     // -20 a 19
bool getProcPriority(int pid, int &priority);
```

### Modo Rápido
```cpp
bool activatePerformanceMode();     // TODO ON
bool deactivatePerformanceMode();   // TODO OFF
bool isPerformanceModeActive();      // Estado
```

## 🔧 Integración en Código Existente

```cpp
// cleanerpage.h
#include "../cleaner/performancecleaner.h"
PerformanceCleaner *m_perfCleaner;

// cleanerpage.cpp
m_perfCleaner = new PerformanceCleaner(this);
m_cleaners << m_perfCleaner;
connectCleaner(m_perfCleaner);
```

## 📝 Estructura de Datos

```cpp
struct CleanEntry {
    QString id;           // "cpu_governor", "turbo_boost", etc
    QString name;         // Display name
    QString description;  // Explicación
    QString path;         // sysfs path
    QString category;     // "Performance"
    qint64  sizeBytes;    // N/A para performance
    bool    selected;     // Seleccionado?
    bool    requiresRoot; // Necesita sudo?
};
```

## ⚙️ CMake Integration

```cmake
# CMakeLists.txt ya actualizado con:

set(SOURCES
    ...
    src/cleaner/performancecleaner.cpp      # ← Agregado
)

set(HEADERS
    ...
    src/cleaner/performancecleaner.h        # ← Agregado
)
```

## 🐛 Troubleshooting

| Problema | Causa | Solución |
|----------|-------|----------|
| Permission Denied | No root | `sudo ./StellarCleaner` |
| Turbo no cambia | No Intel/AMD | Normal, intenta fallback |
| Swappiness sin efecto | Sin swap | Irrelevante con mucha RAM |
| App se congela | Sysfs lento | Normal, espera 5s |
| CPUs no detectados | Kernel antiguo | Ver sysfs manualmente |

## 📚 Documentación

- **PERFORMANCE_OPTIMIZER_SETUP.md** → Arquitectura y código
- **PERFORMANCE_OPTIMIZER_USAGE.md** → Casos de uso y guías
- **CHANGELOG_PERFORMANCE_OPTIMIZER.md** → Cambios efectuados

## 🎯 Casos de Uso Típicos

### Gaming (máximo FPS)
```
[✓] CPU Governor
[✓] Turbo Boost
[✓] Process Priority  
[✓] Swap Opt
```

### Streaming (sin lag)
```
[✓] Turbo Boost
[✓] Process Priority
[✓] Swap Opt
[ ] CPU Governor
```

### Video Editing (velocidad)
```
[✓] CPU Governor
[✓] Turbo Boost
[✓] Swap Opt
[ ] Process Priority
```

## 🔑 Valores Importantes

```
Prioridad (nice value):
  -20 → máxima
    0 → normal (default)
  +19 → mínima

Swappiness:
   0 → nunca usar swap
  60 → default
 100 → maximizar swap

CPU Governor:
  performance  → máxima freq
  powersave    → mínima freq
  ondemand     → dinámico
  conservative → dinámico lento
```

## 🚨 Advertencias

- ⚠️ CPU Governor aumenta consumo de energía 30-50%
- ⚠️ Turbo Boost genera más calor
- ⚠️ Algunos kernels pueden bloquear sysfs
- ⚠️ Cambios se pierden al reiniciar (no persistentes)

## ✅ Testing

```bash
# 1. Compilar
make

# 2. Ejecutar
sudo ./StellarCleaner

# 3. Ver:
#    - "Optimizador de Latencia" en lista
#    - 4 opciones expandibles
#    - Análisis funciona
#    - Logs muestran progreso

# 4. Verificar cambios:
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
# → "performance" (si seleccionó CPU Governor)

# 5. Restaurar (si cambió):
echo powersave | tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor
echo 60 | tee /proc/sys/vm/swappiness
```

## 📞 Quick API Reference

```cpp
// Crear instancia
PerformanceCleaner* perf = new PerformanceCleaner(parent);

// Usar en lista de cleaners
QList<BaseCleaner*> cleaners;
cleaners << perf;  // Polimórfico

// Signals disponibles (heredado de BaseCleaner)
connect(perf, &BaseCleaner::logMessage, [](const QString& msg) {
    qDebug() << msg;  // "[Performance] CPU Governor: performance"
});
```

---

**Versión**: 1.0.0  
**Compilado**: 24-MAR-2024  
**Status**: ✅ Listo para usar
