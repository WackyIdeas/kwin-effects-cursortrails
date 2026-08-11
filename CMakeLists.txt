cmake_minimum_required(VERSION 3.16.0)

project(effect_cursortrails)

option(KWIN_BUILD_WAYLAND "Build with Wayland support" ON)

set(QT_MAJOR_VERSION "6")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ECM ${QT_MAJOR_VERSION} REQUIRED NO_MODULE)

list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs6)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Qt${QT_MAJOR_VERSION} REQUIRED
    COMPONENTS
        Core Gui DBus
)

find_package(KF${QT_MAJOR_VERSION} REQUIRED
    COMPONENTS
        Config ConfigWidgets KCMUtils GuiAddons I18n
)

find_package(KDecoration3 REQUIRED)

find_package(KWin REQUIRED COMPONENTS
    kwineffects
)
find_package(KWinDBusInterface CONFIG REQUIRED)

feature_summary(WHAT ALL)

set(kwin4_effect_cursortrails_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cursortrails.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cursortrails.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/plugin.cpp
)
kconfig_add_kcfg_files(kwin4_effect_cursortrails_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/kcm/trailsconfig.kcfgc
)

add_library(kwin_effect_cursortrails SHARED ${kwin4_effect_cursortrails_SOURCES})

target_link_libraries(kwin_effect_cursortrails
    PRIVATE
        Qt${QT_MAJOR_VERSION}::Core
        KF${QT_MAJOR_VERSION}::ConfigGui
        KDecoration3::KDecoration
        KWin::kwin
)

set(kwin_cursortrails_config_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/kcm/trails_config.cpp)
ki18n_wrap_ui(kwin_cursortrails_config_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/kcm/trails_config.ui)
kconfig_add_kcfg_files(kwin_cursortrails_config_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/kcm/trailsconfig.kcfgc)
qt_add_dbus_interface(kwin_cursortrails_config_SRCS ${KWIN_EFFECTS_INTERFACE} kwineffects_interface)

add_library(kwin_cursortrails_config MODULE ${kwin_cursortrails_config_SRCS})
target_link_libraries(kwin_cursortrails_config
    KF6::KCMUtils
    KF6::CoreAddons
    KF6::I18n
    Qt6::DBus
    KF6::WindowSystem
)

install(
    TARGETS
        kwin_effect_cursortrails
    DESTINATION
        ${KDE_INSTALL_PLUGINDIR}/kwin/effects/plugins
)

install(TARGETS kwin_cursortrails_config DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/configs)
