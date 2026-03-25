cmake_minimum_required(VERSION 3.21)

project(liphis VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Concurrent Gui)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGIT2 libgit2)
pkg_check_modules(LIBARCHIVE libarchive)

qt_standard_project_setup()

qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

# =========================
# Core Static Library
# =========================
add_library(liphis_core STATIC
    core/FileSystemEngine.cpp
    core/AnalysisEngine.cpp
)

target_include_directories(liphis_core PUBLIC
    ${CMAKE_SOURCE_DIR}/core
)

# =========================
# GUI Executable
# =========================
qt_add_executable(liphis_app
    src/main.cpp
    src/AppController.cpp
    src/AppController.hpp
    src/AnalysisController.cpp
    src/AnalysisController.hpp
    src/SystemIconProvider.cpp
    src/SystemIconProvider.hpp
    src/models/FileListModel.cpp
    src/models/FileListModel.hpp
    src/models/FileFilterProxyModel.cpp
    src/models/FileFilterProxyModel.hpp
    src/models/PlacesModel.cpp
    src/models/PlacesModel.hpp
    src/ThumbnailManager.cpp
    src/ThumbnailManager.hpp
    src/ThumbnailImageProvider.cpp
    src/ThumbnailImageProvider.hpp
)

# Expose include directories to GUI target
target_include_directories(liphis_app PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/models
)

qt_add_qml_module(liphis_app
    URI liphis
    VERSION 1.0
    QML_FILES
        src/qml/AppTheme.qml
        src/qml/AppShell.qml
        src/qml/Main.qml
        src/qml/Icon.qml
        src/qml/ThemedIconButton.qml
        src/qml/FilterChip.qml
        src/qml/SidebarItem.qml
        src/qml/PanelSection.qml
        src/qml/HeaderBar.qml
        src/qml/LeftSidebar.qml
        src/qml/TabStrip.qml
        src/qml/CenterWorkspace.qml
        src/qml/InspectorPanel.qml
        src/qml/StatusBar.qml
        src/qml/FileIcon.qml
        src/qml/Breadcrumbs.qml
        src/qml/FileView.qml
        src/qml/PropertiesDialog.qml
        src/qml/AnalyseDialog.qml
        src/qml/ToastManager.qml
        src/qml/BulkRenameDialog.qml
        src/qml/ConnectRemoteDialog.qml
        src/qml/FolderContextMenu.qml
        src/qml/FileContextMenu.qml
        src/qml/MediaContextMenu.qml
    RESOURCES
        LiphisApp-Logo.png
        src/qml/assets/logo-light.png
        src/qml/assets/logo-dark.png
        src/qml/assets/icons/outline/arrow-left.svg
        src/qml/assets/icons/outline/arrow-right.svg
        src/qml/assets/icons/outline/arrow-up.svg
        src/qml/assets/icons/outline/refresh.svg
        src/qml/assets/icons/outline/search.svg
        src/qml/assets/icons/outline/home.svg
        src/qml/assets/icons/outline/bookmark.svg
        src/qml/assets/icons/outline/star.svg
        src/qml/assets/icons/outline/x.svg
        src/qml/assets/icons/outline/folder.svg
        src/qml/assets/icons/outline/folder-open.svg
        src/qml/assets/icons/outline/info-circle.svg
        src/qml/assets/icons/outline/terminal.svg
        src/qml/assets/icons/outline/copy.svg
        src/qml/assets/icons/outline/scissors.svg
        src/qml/assets/icons/outline/pencil.svg
        src/qml/assets/icons/outline/trash.svg
        src/qml/assets/icons/outline/list-details.svg
        src/qml/assets/icons/outline/square-plus.svg
        src/qml/assets/icons/outline/app-window.svg
        src/qml/assets/icons/outline/layout-board-split.svg
        src/qml/assets/icons/outline/password.svg
        src/qml/assets/icons/outline/brand-vscode.svg
        src/qml/assets/icons/outline/player-play.svg
        src/qml/assets/icons/outline/background.svg
        src/qml/assets/icons/outline/tag.svg
        src/qml/assets/icons/outline/window.svg
        src/qml/assets/icons/filled/star.svg
)

target_link_libraries(liphis_app
    PRIVATE Qt6::Core
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Concurrent
    PRIVATE Qt6::Gui
    PRIVATE liphis_core
)

# =========================
# CLI Test Binary
# =========================
add_executable(fs_test
    src/fs_test.cpp
)

target_link_libraries(fs_test
    PRIVATE liphis_core
)

enable_testing()
add_test(NAME fs_test_smoke COMMAND fs_test ${CMAKE_SOURCE_DIR})

install(TARGETS liphis_app
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES
    packaging/liphis.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
)

install(FILES
    LiphisApp-Logo.png
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/512x512/apps
    RENAME liphis.png
)

set(CPACK_PACKAGE_NAME "liphis")
set(CPACK_PACKAGE_VENDOR "Liphis")
set(CPACK_PACKAGE_CONTACT "maintainers@liphis.local")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Liphis file manager")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

set(CPACK_GENERATOR "TGZ;ZIP;DEB;RPM")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Liphis Maintainers")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.31), libstdc++6, libqt6core6, libqt6gui6, libqt6qml6, libqt6quick6")

set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Applications/System")
set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase, qt6-qtdeclarative")

include(CPack)
