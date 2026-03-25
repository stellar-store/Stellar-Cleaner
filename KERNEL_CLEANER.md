# Kernel Cleaner - Documentación

## Descripción

El **Kernel Cleaner** es una nueva funcionalidad que permite eliminar kernels antiguos del sistema, liberando espacio en la partición `/boot`. Es especialmente útil en distribuciones basadas en Debian/Ubuntu donde los kernels antiguos se acumulan rápidamente.

## Características

- 🔍 **Detección automática**: Identifica el kernel actual usando `uname -r`
- 📋 **Listado inteligente**: Enumera todos los kernels instalados via `dpkg`
- 📊 **Cálculo de tamaño**: Muestra el espacio ocupado por cada kernel
- ✅ **Protección automática**: Por defecto protege los kernels más recientes
- 🔄 **Progreso visual**: Barra de progreso durante análisis y limpieza
- 🔐 **Requiere privilegios**: Solicita sudo para la eliminación
- 📝 **Logging detallado**: Registro de todas las operaciones

## Compilación

```bash
cd /home/gus/Downloads/Cleaner
mkdir -p build
cd build
cmake ..
make -j4
```

## Uso

1. **Inicia la aplicación**:
   ```bash
   ./build/StellarCleaner
   ```

2. **Navega a la pestaña "Limpiador del Sistema"**

3. **Ve a la opción "Kernels Antiguos"**

4. **Haz clic en "Analizar"**:
   - Detectará el kernel actual
   - Listará todos los kernels instalados
   - Calculará sus tamaños
   - Por defecto seleccionará los kernels antiguos (excepto los 3 más recientes)

5. **Revisa los kernels a eliminar**:
   - Desactiva cualquier kernel que desees conservar
   - Confirma la selección

6. **Haz clic en "Limpiar Ahora"**:
   - Se ejecutará `apt-get purge` para cada kernel seleccionado
   - El progreso se mostrará en tiempo real
   - Se registrarán todas las operaciones

## Archivos Modificados/Creados

### Nuevos archivos:
- `src/cleaner/kernelcleaner.h` - Definición de la clase
- `src/cleaner/kernelcleaner.cpp` - Implementación

### Archivos modificados:
- `CMakeLists.txt` - Agregados fuentes y encabezados
- `src/ui/cleanerpage.h` - Incluido kernelcleaner.h y declarada variable miembro
- `src/ui/cleanerpage.cpp` - Instanciado KernelCleaner

## API Interna

### Métodos públicos (heredados de BaseCleaner):

```cpp
QString name() const override;           // "Kernels Antiguos"
QString description() const override;    // Descripción de la funcionalidad
QList<CleanEntry> entries() override;   // Lista de kernels detectados
void analyze() override;                 // Detecta kernels y calcula tamaños
qint64 clean(const QList<CleanEntry> &selected); // Elimina kernels seleccionados
```

### Métodos privados:

```cpp
QString getCurrentKernel();              // Obtiene kernel actual con uname -r
QList<KernelInfo> getInstalledKernels(); // Enumera kernels con dpkg
qint64 getPackageSize(const QString &packageName); // Obtiene tamaño del paquete
void setupEntries();                     // Prepara lista de CleanEntry
```

## Señales

Heredadas de BaseCleaner:
- `analyzeStarted()` - Se inicia el análisis
- `analyzeProgress(int percent, const QString &item)` - Progreso del análisis
- `analyzeFinished(qint64 totalBytes)` - Análisis completo
- `cleanStarted()` - Se inicia la limpieza
- `cleanProgress(int percent, const QString &item)` - Progreso de limpieza
- `cleanFinished(qint64 bytesFreed)` - Limpieza completada
- `logMessage(const QString &msg)` - Mensajes de log

## Comandos del sistema

La implementación utiliza:

```bash
# Detectar kernel actual
uname -r

# Listar kernels instalados
dpkg --get-selections | grep linux-image | grep install

# Obtener tamaño del paquete
dpkg-query -W -f='${Installed-Size}' linux-image-X.X.X

# Eliminar kernel
apt-get purge -y linux-image-X.X.X [otras dependencias]
```

## Consideraciones de Seguridad

- **Requiere sudo**: Todas las operaciones de eliminación requieren privilegios de administrador
- **Protección del kernel actual**: El kernel en uso nunca se marca para eliminación
- **Protección de múltiples kernels**: Por defecto solo muestra kernels antiguos
- **Confirmación desactivable**: El usuario debe seleccionar explícitamente cada kernel

## Solución de Problemas

### Error: "dpkg: command not found"
- Asegúrate de estar en Ubuntu/Debian
- dpkg es parte de la base de esos sistemas

### Error: "Permission denied"
- La aplicación debe ejecutarse con permisos de administrador o con sudo
- ElCleanerPage manejaráautomáticamente la solicitud de escalado de privilegios

### Kernels no se detectan
- Ejecuta manualmente: `dpkg --get-selections | grep linux-image`
- Verifica que haya kernels adicionales instalados

## Ejemplo de Salida de Log

```
[Kernel] Kernel actual: 5.15.0-86-generic
[Kernel] Se encontraron 4 kernel(s) antiguo(s)
[Kernel] Ejecutando: apt-get purge -y linux-image-5.15.0-84-generic linux-headers-5.15.0-84-generic ...
[Kernel] ✓ Eliminado: Kernel 5.15.0-84-generic (145230 KB)
[Kernel] ✓ Eliminado: Kernel 5.15.0-82-generic (142100 KB)
```

## Notas de Desarrollo

La clase `KernelCleaner` sigue la arquitectura de otros cleaners (TempCleaner, BrowserCleaner, etc.):

- Hereda de `BaseCleaner`
- Implementa estructura de `CleanEntry` para UI
- Usa signals/slots para actualizar progreso
- Maneja errores mediante logging
- Soporta selección diferenciada de items para limpiar

## Licencia

Parte de StellarCleaner. Sigue los términos de licencia del proyecto principal.
