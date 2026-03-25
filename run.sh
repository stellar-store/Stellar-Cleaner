#!/bin/bash

##############################################################################
# StellarCleaner - Setup and Run Script
# Detecta distro, instala dependencias y ejecuta la aplicación
##############################################################################

set -e

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Funciones
print_header() {
    echo -e "\n${GREEN}=== $1 ===${NC}\n"
}

print_error() {
    echo -e "${RED}[ERROR] $1${NC}"
}

print_info() {
    echo -e "${YELLOW}[INFO] $1${NC}"
}

print_success() {
    echo -e "${GREEN}[OK] $1${NC}"
}

# Detectar distro
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$ID
    else
        print_error "No se puede detectar la distribución Linux"
        exit 1
    fi
}

# Instalar dependencias según distro
install_dependencies() {
    print_header "Instalando Dependencias"
    
    detect_distro
    
    case $OS in
        ubuntu|debian)
            print_info "Detectada distribución Debian/Ubuntu"
            echo "Actualizando repositorios..."
            sudo apt-get update -qq
            
            echo "Instalando dependencias..."
            sudo apt install -y \
                build-essential \
                cmake \
                git \
                sudo \
                util-linux \
                apt-utils \
                hwinfo \
                smartmontools \
                qt6-base-dev \
                libqt6charts6-dev \
                qt6-qpa-plugins \
                2>/dev/null || {
                print_info "Qt6 no disponible, intentando Qt5..."
                sudo apt install -y \
                    build-essential \
                    cmake \
                    git \
                    sudo \
                    util-linux \
                    apt-utils \
                    hwinfo \
                    smartmontools \
                    qtbase5-dev \
                    libqt5charts5-dev
            }
            ;;
        fedora)
            print_info "Detectada distribución Fedora"
            echo "Instalando dependencias..."
            sudo dnf install -y \
                cmake \
                gcc-c++ \
                make \
                git \
                sudo \
                util-linux \
                dnf-plugins-core \
                hwinfo \
                smartmontools \
                qt6-qtbase-devel \
                qt6-qtcharts-devel
            ;;
        arch|manjaro)
            print_info "Detectada distribución Arch/Manjaro"
            echo "Instalando dependencias..."
            sudo pacman -S --noconfirm \
                cmake \
                base-devel \
                git \
                sudo \
                util-linux \
                hwinfo \
                smartmontools \
                qt6-base \
                qt6-charts
            ;;
        opensuse*)
            print_info "Detectada distribución openSUSE"
            echo "Instalando dependencias..."
            sudo zypper install -y \
                cmake \
                gcc-c++ \
                make \
                git \
                sudo \
                util-linux \
                hwinfo \
                smartmontools \
                libqt6-qtbase-devel \
                libqt6-qtcharts-devel
            ;;
        *)
            print_error "Distribución Linux no soportada: $OS"
            print_info "Dependencias requeridas:"
            echo "  - build-essential/gcc-c++"
            echo "  - cmake 3.16+"
            echo "  - git"
            echo "  - sudo"
            echo "  - util-linux"
            echo "  - hwinfo"
            echo "  - smartmontools"
            echo "  - Qt6 (o Qt5.15+)"
            echo "  - libqt6charts6-dev (o libqt5charts5-dev)"
            exit 1
            ;;
    esac
    
    print_success "Dependencias instaladas correctamente"
}

# Compilar proyecto
compile_project() {
    print_header "Compilando Proyecto"
    
    if [ ! -d "build" ]; then
        print_info "Directorio build no encontrado, creando..."
        mkdir -p build
    fi
    
    cd build
    
    if [ ! -f "Makefile" ]; then
        print_info "Configurando CMake..."
        cmake .. -DCMAKE_BUILD_TYPE=Release
    fi
    
    print_info "Compilando... (esto puede tomar algunos minutos)"
    make -j$(nproc)
    
    cd ..
    
    if [ -f "build/StellarCleaner" ]; then
        print_success "Compilación exitosa"
    else
        print_error "La compilación falló"
        exit 1
    fi
}

# Ejecutar aplicación
run_application() {
    print_header "Iniciando StellarCleaner"
    
    if [ ! -f "build/StellarCleaner" ]; then
        print_error "Ejecutable no encontrado en build/StellarCleaner"
        exit 1
    fi
    
    print_info "Iniciando aplicación..."
    ./build/StellarCleaner
}

# Script principal
main() {
    echo "╔════════════════════════════════════════════════╗"
    echo "║     StellarCleaner - Limpiador del Sistema     ║"
    echo "║                   v1.1.0                       ║"
    echo "╚════════════════════════════════════════════════╝"
    
    # Verificar si estamos en el directorio correcto
    if [ ! -f "CMakeLists.txt" ]; then
        print_error "El script debe ejecutarse desde el directorio raíz del proyecto"
        exit 1
    fi
    
    # Check if dependencies are already installed
    if ! command -v cmake &> /dev/null; then
        install_dependencies
    else
        print_info "CMake ya está instalado"
    fi
    
    # Compilar si es necesario
    if [ ! -f "build/StellarCleaner" ]; then
        compile_project
    else
        print_info "Ejecutable compilado encontrado"
    fi
    
    # Ejecutar
    run_application
}

# Ejecutar main
main "$@"
