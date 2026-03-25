cmake_minimum_required(VERSION 3.16)
project(StellarCleaner VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 (fallback to Qt5)
find_package(Qt6 QUIET COMPONENTS Widgets Core Concurrent Charts)
if(NOT Qt6_FOUND)
    find_package(Qt5 REQUIRED COMPONENTS Widgets Core Concurrent Charts)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/ui/sidebar.cpp
    src/ui/titlebar.cpp
    src/ui/dashboard.cpp
    src/ui/cleanerpage.cpp
    src/ui/analyzerpage.cpp
    src/ui/kernelpage.cpp
    src/ui/optimizerpage.cpp
    src/ui/toolspage.cpp
    src/ui/settingspage.cpp
    src/ui/logviewer.cpp
    src/cleaner/basecleaner.cpp
    src/cleaner/tempcleaner.cpp
    src/cleaner/browsercleaner.cpp
    src/cleaner/logcleaner.cpp
    src/cleaner/packagecleaner.cpp
    src/cleaner/privacycleaner.cpp
    src/cleaner/trashcleaner.cpp
    src/cleaner/kernelcleaner.cpp
    src/cleaner/performancecleaner.cpp
    src/analyzer/diskanalyzer.cpp
    src/tools/startupmanager.cpp
    src/tools/processmanager.cpp
    src/tools/uninstallmanager.cpp
    src/utils/fileutils.cpp
    src/utils/systeminfo.cpp
    src/utils/sudohelper.cpp
    src/utils/scheduler.cpp
    resources/resources.qrc
)

set(HEADERS
    src/mainwindow.h
    src/ui/sidebar.h
    src/ui/titlebar.h
    src/ui/dashboard.h
    src/ui/cleanerpage.h
    src/ui/analyzerpage.h
    src/ui/kernelpage.h
    src/ui/optimizerpage.h
    src/ui/toolspage.h
    src/ui/settingspage.h
    src/ui/logviewer.h
    src/cleaner/basecleaner.h
    src/cleaner/tempcleaner.h
    src/cleaner/browsercleaner.h
    src/cleaner/logcleaner.h
    src/cleaner/packagecleaner.h
    src/cleaner/privacycleaner.h
    src/cleaner/trashcleaner.h
    src/cleaner/kernelcleaner.h
    src/cleaner/performancecleaner.h
    src/analyzer/diskanalyzer.h
    src/tools/startupmanager.h
    src/tools/processmanager.h
    src/tools/uninstallmanager.h
    src/utils/fileutils.h
    src/utils/systeminfo.h
    src/utils/sudohelper.h
    src/utils/scheduler.h
)

add_executable(StellarCleaner ${SOURCES} ${HEADERS})

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(StellarCleaner
        Qt6::Widgets
        Qt6::Core
        Qt6::Concurrent
        Qt6::Charts
    )
else()
    target_link_libraries(StellarCleaner
        Qt5::Widgets
        Qt5::Core
        Qt5::Concurrent
        Qt5::Charts
    )
endif()

# Install targets
install(TARGETS StellarCleaner DESTINATION /usr/local/bin)
install(FILES packaging/stellarcleaner.desktop DESTINATION /usr/share/applications)
install(FILES resources/icons/stellarcleaner.png DESTINATION /usr/share/pixmaps)

# Package with CPack
set(CPACK_PACKAGE_NAME "StellarCleaner")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Professional Linux system cleaner")
set(CPACK_GENERATOR "DEB;RPM;TGZ")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt6widgets6, libqt6charts6")
include(CPack)
