#!/bin/bash
# StellarCleaner Build Script with KernelCleaner support

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"

echo "🔧 StellarCleaner - Script de Compilación"
echo "=========================================="
echo ""

# Verify CMakeLists.txt has KernelCleaner
if ! grep -q "kernelcleaner" "$PROJECT_DIR/CMakeLists.txt"; then
    echo "⚠️  WARNING: CMakeLists.txt might not include KernelCleaner"
fi

# Create build directory
if [ ! -d "$BUILD_DIR" ]; then
    echo "📁 Creando directorio build..."
    mkdir -p "$BUILD_DIR"
fi

# Configure
echo "⚙️  Configurando CMake..."
cd "$BUILD_DIR"
cmake .. || exit 1

# Build
echo ""
echo "🔨 Compilando... (esto puede tomar varios minutos)"
echo ""

# Get number of available processors
JOBS=$(nproc)
if [ $JOBS -gt 4 ]; then
    JOBS=4  # Limit to 4 to avoid excessive memory usage
fi

echo "Usando $JOBS procesos de compilación..."
echo ""

make -j$JOBS || {
    echo ""
    echo "❌ Error durante la compilación"
    echo ""
    echo "Intentando compilar con diagnósticos..."
    make VERBOSE=1 2>&1 | tail -50
    exit 1
}

# Check if binary was created
if [ -f "$BUILD_DIR/StellarCleaner" ]; then
    echo ""
    echo "✅ ¡Compilación exitosa!"
    echo ""
    echo "Para ejecutar la aplicación:"
    echo "  $BUILD_DIR/StellarCleaner"
    echo ""
    echo "Puedes instalar la aplicación con:"
    echo "  cd packaging && sudo ./install.sh"
    echo ""
else
    echo "❌ Error: El binario StellarCleaner no fue creado"
    exit 1
fi
