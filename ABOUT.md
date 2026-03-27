# About Liphis

Liphis is a high-performance, modern file manager built with **C++20** and the **Qt6** framework. It is designed to be fast, extensible, and deeply integrated with the Linux desktop environment.

## 🏗️ Core Architecture

The project follows a decoupled architecture that separates performance-critical backend logic from a reactive, modern frontend.

### 1. Performance Engines (`core/`)
- **`FileSystemEngine`**: The heart of the application. It provides a high-level API for file operations (CRUD, recursive search, path monitoring). While built on `std::filesystem`, it is optimized to use external tools like `fd` for lightning-fast recursive searches and `gio` for desktop-integrated operations like trashing or mounting.
- **`AnalysisEngine`**: A specialized engine for deep directory analysis. It uses parallel traversal to calculate folder sizes and generate tree structures, providing users with insights into their storage usage.
- **`ThreadPool`**: A custom, lightweight thread pool implementation used across all engines to ensure that heavy I/O and analysis tasks never block the main UI thread.

### 2. Application Layer & UI (`src/`)
The application follows a variation of the **MVVM (Model-View-ViewModel)** pattern, adapted for Qt/QML.

- **Controllers (`AppController`, `AnalysisController`)**: These act as the glue between the C++ engines and the QML UI. They manage application state (current path, navigation history, selection) and integrate with **`libgit2`** to provide real-time git status overlays.
- **Models (`src/models/`)**: Custom Qt models (e.g., `FileListModel`, `PlacesModel`) wrap data from the engines, allowing QML views to display and interact with file lists efficiently.
- **Frontend (`src/qml/`)**: A fully reactive UI built with **Qt Quick/QML**. It features modern navigation, breadcrumbs, and a flexible "AppShell" layout for a seamless user experience.

### 3. Asset & Icon Management
Liphis includes a massive collection of SVG icons (from the Tabler collection) located in the `filled/` and `outline/` directories.
- **`Icon.qml`**: A centralized component that maps abstract icon names to local SVG assets or falls back to system themes using `image://icon/`.

## 🛠️ Technology Stack

- **Language**: C++20, QML, JavaScript (UI logic).
- **Framework**: Qt6 (Core, Quick, Gui, Concurrent).
- **Build System**: CMake.
- **Key Dependencies**:
    - `libgit2`: For seamless git integration.
    - `libarchive`: For handling compressed files (zip, tar.gz, etc.).
    - `fd`: Optional dependency for high-speed file searching.
    - `gio`: For standard Linux desktop integration (trash, mounting, opening files).
    - `pkexec`: For elevated (root) operations.

## 🚀 Key Features

- **Parallel Directory Analysis**: Quickly visualize folder sizes and structures.
- **Git Integration**: View repository status directly in the file manager.
- **Fast Search**: Integrated support for `fd` for near-instant results.
- **Modern UI**: A clean, responsive interface that respects system themes and provides rich visual feedback.
