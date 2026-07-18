cmake_minimum_required(VERSION 3.16)

project(markdownviewer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ECM REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets DBus)
find_package(KF6SyntaxHighlighting REQUIRED)
find_package(KF6DBusAddons REQUIRED)
find_package(KF6WindowSystem REQUIRED)

add_executable(markdownviewer
    src/main.cpp
    src/MainWindow.cpp
    src/MarkdownRenderer.cpp
    src/MarkdownTab.cpp
    src/CodeHighlighter.cpp
    src/SingleInstanceGuard.cpp
    src/DebugTiming.cpp
)

target_link_libraries(markdownviewer PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::DBus
    KF6::SyntaxHighlighting
    KF6::DBusAddons
    KF6::WindowSystem
)

install(TARGETS markdownviewer DESTINATION ${KDE_INSTALL_BINDIR})
install(FILES resources/io.markupt.markdownviewer.desktop DESTINATION ${KDE_INSTALL_APPDIR})
