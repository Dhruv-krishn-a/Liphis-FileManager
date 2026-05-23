cmake_minimum_required(VERSION 3.21)

project(liphis VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Concurrent Gui Core5Compat Sql)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBGIT2 REQUIRED libgit2)
pkg_check_modules(LIBARCHIVE REQUIRED libarchive)

qt_standard_project_setup()

qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

# =========================
# Core Static Library
# =========================
add_library(liphis_core STATIC
    core/FileSystemEngine.cpp
    core/AnalysisEngine.cpp
    core/FileCommand.cpp
)

target_link_libraries(liphis_core PRIVATE Qt6::Core)

target_include_directories(liphis_core PUBLIC
    ${CMAKE_SOURCE_DIR}/core
)

# =========================
# GUI Executable
# =========================
# We use 'liphis_bin' as the target name to avoid conflict with the 'liphis' QML module directory
qt_add_executable(liphis_bin
    src/main.cpp
    src/AppController.cpp
    src/AppController.hpp
    src/AnalysisController.cpp
    src/AnalysisController.hpp
    src/DocumentIntelligenceController.cpp
    src/DocumentIntelligenceController.hpp
    src/SystemIconProvider.cpp
    src/SystemIconProvider.hpp
    src/models/FileListModel.cpp
    src/models/FileListModel.hpp
    src/models/FileTreeModel.cpp
    src/models/FileTreeModel.hpp
    src/models/FileFilterProxyModel.cpp
    src/models/FileFilterProxyModel.hpp
    src/TerminalManager.cpp
    src/TerminalManager.hpp
    src/models/PlacesModel.cpp
    src/models/PlacesModel.hpp
    src/ThumbnailManager.cpp
    src/ThumbnailManager.hpp
    src/ThumbnailImageProvider.cpp
    src/ThumbnailImageProvider.hpp
    src/CommandManager.cpp
    src/CommandManager.hpp
)

# We ensure the final binary is still named 'liphis'
set_target_properties(liphis_bin PROPERTIES OUTPUT_NAME "liphis")

target_include_directories(liphis_bin PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/models
)

# Collecting all icons with RELATIVE paths to src/qml/assets/icons/
file(GLOB_RECURSE OUTLINE_ICONS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/qml/assets/icons/outline/*.svg")
file(GLOB_RECURSE FILLED_ICONS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/qml/assets/icons/filled/*.svg")

qt_add_qml_module(liphis_bin
    URI liphis_module
    VERSION 1.0
    QML_FILES
        src/qml/AppTheme.qml
        src/qml/AppShell.qml
        src/qml/Main.qml
        src/qml/Icon.qml
        src/qml/CommandPalette.qml
        src/qml/ThemedIconButton.qml
        src/qml/FilterChip.qml
        src/qml/SidebarItem.qml
        src/qml/PanelSection.qml
        src/qml/HeaderBar.qml
        src/qml/LeftSidebar.qml
        src/qml/TabStrip.qml
        src/qml/CenterWorkspace.qml
        src/qml/DocumentCenterPage.qml
        src/qml/InspectorPanel.qml
        src/qml/StatusBar.qml
        src/qml/FileIcon.qml
        src/qml/Breadcrumbs.qml
        src/qml/HeaderColumn.qml
        src/qml/SettingsDialog.qml
        src/qml/OpenWithDialog.qml
        src/qml/ProgressDialog.qml
        src/qml/FileView.qml
        src/qml/PropertiesDialog.qml
        src/qml/AnalyseDialog.qml
        src/qml/DocumentInboxDialog.qml
        src/qml/DocumentDuplicatesDialog.qml
        src/qml/DocumentHealthDialog.qml
        src/qml/CanonicalNamingDialog.qml
        src/qml/ToastManager.qml
        src/qml/BulkRenameDialog.qml
        src/qml/ConnectRemoteDialog.qml
        src/qml/FolderContextMenu.qml
        src/qml/DirectoryContextMenu.qml
        src/qml/FileContextMenu.qml
        src/qml/MediaContextMenu.qml
    RESOURCES
        LiphisApp-Logo.png
        src/qml/assets/logo-light.png
        src/qml/assets/logo-dark.png
        src/qml/assets/folder-light.png
        src/qml/assets/folder-dark.png
        ${OUTLINE_ICONS}
        ${FILLED_ICONS}
)

target_link_libraries(liphis_bin
    PRIVATE Qt6::Core
    PRIVATE Qt6::Quick
    PRIVATE Qt6::Concurrent
    PRIVATE Qt6::Gui
    PRIVATE Qt6::Core5Compat
    PRIVATE Qt6::Sql
    PRIVATE liphis_core
    PRIVATE ${LIBGIT2_LIBRARIES}
    PRIVATE ${LIBARCHIVE_LIBRARIES}
)

target_include_directories(liphis_bin PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/models
    ${LIBGIT2_INCLUDE_DIRS}
    ${LIBARCHIVE_INCLUDE_DIRS}
)

option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    add_executable(fs_test src/fs_test.cpp)
    target_link_libraries(fs_test PRIVATE liphis_core)
    add_executable(unit_tests tests/unit_tests.cpp)
    target_link_libraries(unit_tests PRIVATE liphis_core)
    enable_testing()
    add_test(NAME fs_test_smoke COMMAND fs_test ${CMAKE_SOURCE_DIR})
    add_test(NAME unit_tests COMMAND unit_tests)
endif()

# Installation rules
install(TARGETS liphis_bin
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY src/qml/assets/icons/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/liphis/icons
)

install(FILES
    packaging/liphis.desktop
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
)

install(FILES
    src/qml/assets/logo-light.png
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
