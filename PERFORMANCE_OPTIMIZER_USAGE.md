# 📖 Guía de Uso: Optimizador de Latencia (Game Mode)

## Inicio Rápido (Quick Start)

### 1. Ejecutar la Aplicación
```bash
./build_perf/StellarCleaner
```

### 2. Navegar al Optimizador de Latencia
```
Panel Izquierdo → "Limpiador del Sistema" → Expandir "Optimizador de Latencia"
```

### 3. Analizar el Sistema
```
Botón "🔍 Analizar" → Espera a que complete
```

### 4. Activar Optimizaciones
```
☑ Selecciona las optimizaciones deseadas:
  ☑ Optimizer CPU Governor           (recomendado para gaming)
  ☑ Intel Turbo Boost / AMD Turbo Core (recomendado para gaming)
  ☑ Prioridad de Procesos             (recomendado si hay lag)
  ☑ Optimización de Memoria Swap      (siempre recomendado)

Botón "🗑 Limpiar Ahora" [requiere permisos root]
```

### 5. Observar Registro de Operaciones
```
Panel Derecho → "REGISTRO DE OPERACIONES" muestra el progreso
```

---

## Casos de Uso

### 🎮 Gaming: Máximo Rendimiento en Juegos
**Problema**: FPS bajo, lag, stuttering

**Solución**:
```
☑ Optimizer CPU Governor         ← CPU en máxima potencia
☑ Intel Turbo Boost              ← Saca frecuencia máxima
☑ Prioridad de Procesos          ← Juego obtiene más CPU
☑ Optimización de Memoria Swap   ← Reduce latencia RAM
```

**Comando manual** (si prefieres terminal):
```bash
# CPU Governor a performance
echo performance | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# Desactivar Intel turbo (0=habilitado... espera, typo en sysfs)
# Realmente: no_turbo=0 para HABILITADO
echo 0 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo

# Boost en primer plano
sudo nice -n -5 ./game

# Reducir swap usage
echo 10 | sudo tee /proc/sys/vm/swappiness
```

---

### 🎥 Streaming: Bajos Latencia en Transmisión
**Problema**: La transmisión se entrecorta, lag de audio

**Solución**:
```
☑ Intel Turbo Boost              ← Mantiene frecuencia constante
☑ Prioridad de Procesos          ← OBS/Streaming priorizado
☑ Optimización de Memoria Swap   ← Menos interrupciones
⬜ Optimizer CPU Governor        ← Opcional (consume más energía)
```

**Procesos a monitorear**:
```bash
ps aux | grep -E "(obs|ffmpeg|streamlab)" | awk '{print $2}'
# Luego: sudo renice -n -5 -p <PID>
```

---

### 💻 Workstation: Renderizado / Compilación
**Problema**: Renderizado lento, compilaciones tardan horas

**Solución**:
```
☑ Optimizer CPU Governor
☑ Intel Turbo Boost
⬜ Prioridad de Procesos         ← No es necesario
☑ Optimización de Memoria Swap
```

---

### 🔄 Restaurar Comportamiento Normal
Para deshacer los cambios:

```bash
# CPU Governor a modo automático
echo powersave | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor

# Desactivar Turbo Boost (vuelve a defaults)
echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo

# Restaurar swappiness a default
echo 60 | sudo tee /proc/sys/vm/swappiness

# Procesos vuelven a prioridad normal
sudo renice 0 -p $$
```

---

## Métodos Disponibles (Para Programadores)

Si necesitas integrar esta funcionalidad en tu código:

```cpp
#include "cleaner/performancecleaner.h"

PerformanceCleaner perf;

// Analizar sistema
perf.analyze();

// Obtener información
int cpuCount = perf.getCPUCount();
QString currentGov = perf.getCPUGovernor();
QStringList available = perf.getAvailableGovernors();

// Cambiar governor
perf.setCPUGovernor("performance");

// Ajustar prioridad de un proceso
perf.setProcPriority(1234, -5);  // PID 1234 con prioridad -5

// Obtener prioridad actual
int priority;
if (perf.getProcPriority(1234, priority)) {
    qDebug() << "Prioridad actual:" << priority;
}

// Activar/Desactivar modo performance
perf.activatePerformanceMode();
perf.deactivatePerformanceMode();
```

---

## Valores de Prioridad (Nice Values)

El sistema Linux utiliza "nice values" para la prioridad de procesos:

```
Valor    Significado              CPU Time
------   ==================       ===========
 -20     Máxima prioridad         Máximo
 -10     Alta prioridad (real)    Muy alto  ← Recomendado para gaming
  -5     Alta prioridad (normal)  Alto      ← Usado por PerformanceCleaner
   0     Prioridad normal         Normal    ← Default
  +10    Baja prioridad           Bajo
  +19    Mínima prioridad         Mínimo
```

**Notas**:
- Solo root puede establecer valores negativos (< 0)
- Tu usuario puede reducir su propia prioridad (aumentar nice value)
- Process Priority Boost intenta -5, pero podría necesitar ajustes

---

## Problemas Comunes y Soluciones

### ❌ "Permission Denied" en CPU Governor
**Causa**: No tienen permisos root

**Solución**:
```bash
# Opción 1: Ejecutar con sudo
sudo ./build_perf/StellarCleaner

# Opción 2: Agregar a sudoers (mejor para desarrollo)
echo "$USER ALL=(ALL) NOPASSWD:/bin/bash" | sudo tee -a /etc/sudoers
```

### ❌ "Intel Turbo no se activa"
**Causa**: No es un CPU Intel, o no soporta turbo

**Solución**:
- AMD CPUs: Sistema intenta automáticamente boost en cpufreq
- Dual setup (Intel + AMD): Sistema intenta ambos
- CPUs antiguos: Puede no estar disponible

### ❌ La aplicación se "congela" al analizar
**Causa**: Lectura de sysfs en CPU lenta o con muchos cores

**Solución**:
```cpp
// El análisis corre en thread separado (QtConcurrent)
// Espera ~5 segundos para CPUs con 64+ cores
```

### ❌ "Swappiness ya estaba bajo" o "sin efecto"
**Causa**: Sistema con mucha RAM (no usa swap)

**Solución**:
- Cambio principalmente afecta si tienes activo swap
- En SSD, el efecto es menor que en HDD antiguo
- Recomendado si: RAM < 50% disponible

---

## Monitoreo Posterior

Una vez activadas las optimizaciones, puedes verificar:

### Ver CPU Governor Actual
```bash
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
# Output: performance
```

### Ver Status de Turbo Boost
```bash
# Intel
cat /sys/devices/system/cpu/intel_pstate/no_turbo
# Output: 0 (habilitado), 1 (deshabilitado)

# AMD
cat /sys/devices/system/cpu/cpufreq/boost
# Output: 1 (habilitado), 0 (deshabilitado)
```

### Ver CPU Frequency Actual (MHz)
```bash
cat /proc/cpuinfo | grep MHz

# O en tiempo real:
watch -n 0.1 'cat /proc/cpuinfo | grep MHz'
```

### Ver Swappiness
```bash
cat /proc/sys/vm/swappiness
# Output: 10 (después de PerformanceCleaner)
```

### Ver Uso de Procesos
```bash
# Ver prioridad de un proceso
ps -eo pid,ni,comm | grep <app>

# Ejemplo: ver prioridad de Firefox
ps -eo pid,ni,comm | grep firefox
# Output: 1234  -5  firefox  ← prioridad boosteada
```

---

## Impacto en Consumo de Energía

| Optimización | Efecto Energía | Recomendado para |
|--------|-------|------|
| CPU Governor (Performance) | ⬆️⬆️ +30-50% | Desktop/Gaming |
| Turbo Boost | ⬆️ +10-20% | Workload Temporal |
| Process Priority | ➡️ Neutral | Siempre seguro |
| Swap Reduction | ➡️ Neutral | Siempre seguro (+ rápido) |

**Consejo**: En laptops con batería, desactiva el governor performance si no juegas.

---

## Configuración Avanzada

### Limitar CPU Frequency Máxima
```bash
# Limitar a 2.5 GHz para reducir calor
echo 2500000 | sudo tee /sys/devices/system/cpu/cpu*/cpufreq/scaling_max_freq
```

### Cambiar Scaling Driver
```bash
# Ver driver actual
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_driver
# Output: intel_pstate, amd-pstate, cpufreq-dt, etc.

# No se recomienda cambiar desde aplicación (requiere reboot)
```

### Usar CPU Set para Pinning de Cores
```bash
# Correr aplicación solo en cores 0-3
taskset -c 0-3 ./mi_aplicacion

# Verificar
ps -eo pid,cmd,psr
```

---

## Referencias

- [Linux CPU Frequency Scaling](https://www.kernel.org/doc/html/latest/admin-guide/pm/cpufreq.html)
- [setpriority(2) man page](https://man7.org/linux/man-pages/man2/setpriority.2.html)
- [/proc/sys/vm/swappiness](https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/performance_tuning_guide/s-memory-tunables)
- [Intel Turbo Boost](https://ark.intel.com/content/www/us/en/ark.html)
- [AMD Precision Boost](https://www.amd.com/en/technologies/precision-boost)

---

**Creado**: Marzo 2024
**Versión**: 1.0
**Mantenedor**: Community
