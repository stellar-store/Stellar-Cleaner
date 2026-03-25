# Ejemplos de Uso - Kernel Cleaner

## Ejemplo 1: Análisis de Kernels

### Entrada del Usuario
1. Abre StellarCleaner
2. Ve a "Limpiador del Sistema"
3. Selecciona "Kernels Antiguos"
4. Haz clic en "🔍 Analizar"

### Salida Esperada

```
[Kernel] Kernel actual: 5.15.0-86-generic
[Kernel] Se encontraron 4 kernel(s) antiguo(s)
```

### List de Kernels Mostrada en UI

```
┌─────────────────────────────────────────────────────────────────┐
│ Categoría: Kernels                                              │
├─────────────────────────────────────────────────────────────────┤
│ ☐ Kernel 5.15.0-84-generic                                      │
│   Paquete: linux-image-5.15.0-84-generic     Tamaño: 145.2 MB  │
│   Ruta: /boot                                                   │
│   Requiere: ✓ sudo                                              │
├─────────────────────────────────────────────────────────────────┤
│ ☑ Kernel 5.15.0-82-generic                                      │
│   Paquete: linux-image-5.15.0-82-generic     Tamaño: 142.8 MB  │
│   Ruta: /boot                                                   │
│   Requiere: ✓ sudo                                              │
├─────────────────────────────────────────────────────────────────┤
│ ☑ Kernel 5.15.0-80-generic                                      │
│   Paquete: linux-image-5.15.0-80-generic     Tamaño: 140.5 MB  │
│   Ruta: /boot                                                   │
│   Requiere: ✓ sudo                                              │
├─────────────────────────────────────────────────────────────────┤
│ ☑ Kernel 5.15.0-78-generic                                      │
│   Paquete: linux-image-5.15.0-78-generic     Tamaño: 139.2 MB  │
│   Ruta: /boot                                                   │
│   Requiere: ✓ sudo                                              │
└─────────────────────────────────────────────────────────────────┘

Espacio Total a Liberar: 422.5 MB
```

## Ejemplo 2: Limpieza de Kernels

### Entrada del Usuario
(Continuando del ejemplo 1)
1. Haz clic en "🗑️ Limpiar Ahora"
2. Confirma en el diálogo

### Progreso de Limpieza

```
Eliminando: Kernel 5.15.0-82-generic
[████░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░] 25%

[Kernel] Ejecutando: apt-get purge -y linux-image-5.15.0-82-generic 
    linux-headers-5.15.0-82-generic linux-modules-5.15.0-82-generic 
    linux-modules-extra-5.15.0-82-generic
[Kernel] ✓ Eliminado: Kernel 5.15.0-82-generic (139 KB)
```

```
Eliminando: Kernel 5.15.0-80-generic
[████████░░░░░░░░░░░░░░░░░░░░░░░░░░] 50%

[Kernel] Ejecutando: apt-get purge -y linux-image-5.15.0-80-generic 
    linux-headers-5.15.0-80-generic linux-modules-5.15.0-80-generic 
    linux-modules-extra-5.15.0-80-generic
[Kernel] ✓ Eliminado: Kernel 5.15.0-80-generic (137 KB)
```

```
Eliminando: Kernel 5.15.0-78-generic
[████████████████░░░░░░░░░░░░░░░░░░] 75%

[Kernel] Ejecutando: apt-get purge -y linux-image-5.15.0-78-generic 
    linux-headers-5.15.0-78-generic linux-modules-5.15.0-78-generic 
    linux-modules-extra-5.15.0-78-generic
[Kernel] ✓ Eliminado: Kernel 5.15.0-78-generic (136 KB)
```

### Resultado Final

```
[████████████████████░░░░░░░░░░░░░░] 100%

✅ Limpieza completada exitosamente

Espacio liberado: 412 MB

Logs:
[Kernel] Kernel actual: 5.15.0-86-generic
[Kernel] Se encontraron 4 kernel(s) antiguo(s)
[Kernel] Ejecutando: apt-get purge -y linux-image-5.15.0-82-generic ...
[Kernel] ✓ Eliminado: Kernel 5.15.0-82-generic (139 KB)
[Kernel] Ejecutando: apt-get purge -y linux-image-5.15.0-80-generic ...
[Kernel] ✓ Eliminado: Kernel 5.15.0-80-generic (137 KB)
[Kernel] Ejecutando: apt-get purge -y linux-image-5.15.0-78-generic ...
[Kernel] ✓ Eliminado: Kernel 5.15.0-78-generic (136 KB)
```

## Ejemplo 3: Situación - Sin Kernels Antiguos

### Entrada del Usuario
1. Haz clic en "🔍 Analizar"

### Salida

```
[Kernel] Kernel actual: 5.15.0-86-generic
[Kernel] Se encontraron 0 kernel(s) antiguo(s)
```

### UI Result

```
Espacio Total a Liberar: 0 MB

ℹ️ No hay kernels antiguos para eliminar
```

## Ejemplo 4: Error - Permisos Insuficientes

### Entrada del Usuario
1. Intenta ejecutar limpieza sin sudo

### Salida en Log

```
[Kernel] Ejecutando: apt-get purge -y linux-image-5.15.0-84-generic ...
[Kernel] ✗ Error al eliminar Kernel 5.15.0-84-generic: sudo: 1 incorrect 
  password attempt, this incident will be reported
```

### Solución

La aplicación automáticamente solicitará permisos elevados mediante:
- GTK password dialog (GNOME)
- KDE password dialog (KDE Plasma)
- Sudo con parámetro -S para entrada interactiva

## Ejemplo 5: Selección Manual

### Escenario
Tienes 5 kernels antiguos pero solo quieres eliminar 2 específicos

### Pasos
1. Haz clic en "🔍 Analizar"
2. Deselecciona los kernels que NO deseas eliminar:
   - ☑️ Kernel 5.15.0-84 → ☐ Kernel 5.15.0-84 (no desselect)
   - ☑️ Kernel 5.15.0-82 → ☐ Kernel 5.15.0-82 (deselect)
   - ☑️ Kernel 5.15.0-80 → ☑️ Kernel 5.15.0-80 (mantener)
   - ☑️ Kernel 5.15.0-78 → ☑️ Kernel 5.15.0-78 (mantener)

3. Haz clic en "🗑️ Limpiar Ahora"

### Salida

```
[Kernel] Ejecutando: apt-get purge -y linux-image-5.15.0-84-generic ...
[Kernel] ✓ Eliminado: Kernel 5.15.0-84-generic (145 KB)
[Kernel] Ejecutando: apt-get purge -y linux-image-5.15.0-80-generic ...
[Kernel] ✓ Eliminado: Kernel 5.15.0-80-generic (140 KB)

Espacio liberado: 285 MB
```

## Ejemplo 6: Estadísticas post-limpieza

### Verificación Manual

```bash
# Listar kernels instalados
$ dpkg --get-selections | grep linux-image | grep install

linux-image-5.15.0-86-generic    install
linux-image-5.15.0-78-generic    install

# Antes: 5 kernels (1 actual + 4 antiguos)
# Después: 2 kernels (1 actual + 1 antiguo)
```

### En StellarCleaner

```
Resumen de Limpieza:
├─ Kernel 5.15.0-84-generic    ✓ Eliminado
├─ Kernel 5.15.0-82-generic    ✓ Eliminado
├─ Kernel 5.15.0-80-generic    ✓ Eliminado
└─ Kernel 5.15.0-78-generic    Conservado (protegido)

Total liberado: 412 MB
Espacio disponible en /boot: ~ 300 MB (antes: 50 MB libres)
```

## Mensajes en Diferentes Idiomas

### Español
```
Analizando: Kernels Antiguos
Eliminando: Kernel 5.15.0-86-generic
```

### Ejemplo de logs internacionales (si se expande)
```
[Kernel] Kernel actual: 5.15.0-86-generic      [ES]
[Kernel] Current kernel: 5.15.0-86-generic     [EN]
[Kernel] Noyau actuel : 5.15.0-86-generic     [FR]
```

---

Estos ejemplos muestran cómo se vería la interfaz y los logs durante diferentes escenarios de uso del Kernel Cleaner.
