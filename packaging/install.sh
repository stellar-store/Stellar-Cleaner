#!/bin/bash
# StellarCleaner - Installation script
set -e
echo "=== StellarCleaner v1.0.0 - Instalador ==="
if command -v apt-get &>/dev/null; then
    echo "[+] Instalando dependencias Qt6 (APT)..."
    sudo apt-get update -qq
    sudo apt-get install -y build-essential cmake \
        qt6-base-dev libqt6charts6-dev \
        || sudo apt-get install -y build-essential cmake \
        qtbase5-dev libqt5charts5-dev
elif command -v dnf &>/dev/null; then
    sudo dnf install -y cmake gcc-c++ qt6-qtbase-devel qt6-qtcharts-devel
elif command -v pacman &>/dev/null; then
    sudo pacman -S --noconfirm cmake base-devel qt6-base qt6-charts
fi
echo "[+] Compilando..."
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
echo "[+] Instalando binario y archivos..."
sudo make install
sudo cp ../resources/icons/stellarcleaner.png /usr/share/pixmaps/ 2>/dev/null || true
sudo cp ../packaging/stellarcleaner.desktop /usr/share/applications/ 2>/dev/null || true
sudo update-desktop-database /usr/share/applications/ 2>/dev/null || true
echo ""
echo "=== Instalacion completa ==="
echo "Ejecuta: stellarcleaner"
