# Liphis Project Roadmap (Start → Finish, For Beginners)

You said: “I have **no prior C++ experience** and I want a **proper roadmap**: what to read first, what to read last, and how to understand the project end‑to‑end.”

This document is that roadmap.

**Start file:** `ABOUT.md`  
**End file:** `tests/unit_tests.cpp`

If you follow this in order (and do the small exercises), you’ll be able to “glide through” the codebase and confidently answer: “Where is this feature implemented, and how does data flow from UI → C++ → filesystem → back to UI?”

---

## Roadmap rules (read this once)

1) **Always trace from the UI event first.** In Qt/QML apps, the UI is often the clearest “user story”.  
2) **Learn the architecture by running one feature through the pipeline:**

```
QML (src/qml/*)
  -> Controller (src/*Controller.*)
    -> Model (src/models/*Model.*)
      -> Core Engine (core/*Engine.*)
        -> OS / filesystem / external tools
      -> Model emits updates
  -> QML redraws automatically via bindings
```

3) **Don’t try to “understand all C++”.** You only need the subset used here.
4) Each phase below has:
- What to read (exact files)
- What to look for
- A quick “checkpoint” so you know you actually understood it

---

## Phase 0 — 30 minutes: “Minimal C++ + Qt vocabulary”

### 0.1 C++ basics you need (only)
- `*.hpp` vs `*.cpp`: declaration vs implementation.
- `#include`: pulls declarations into a file.
- `std::vector<T>`: dynamic array.
- `std::string`: string.
- `std::shared_ptr<T>`: shared ownership pointer (used for caching/interning here).
- `std::atomic<T>`: used for cancellation / counters.
- `namespace fs = std::filesystem`: alias to C++ filesystem library.

### 0.2 Qt/QML basics you need (only)
- `QObject`: base class that can emit signals & expose properties.
- `Q_PROPERTY`: makes a C++ value observable in QML.
- `Q_INVOKABLE`: makes a C++ method callable from QML.
- `signals:`: events QML can react to.
- `QAbstractListModel`: how C++ exposes lists to QML.

Checkpoint:
- You can explain what `Q_INVOKABLE void openPath(const QString &path);` means in plain English.

---

## Phase 1 — 45 minutes: “What is this project and how is it built?”

### 1.1 Read: `ABOUT.md`
What to look for:
- The big pieces: `core/` engines vs `src/` app layer vs `src/qml/` UI
- Key dependencies: Qt6, libgit2, libarchive

Checkpoint:
- You can summarize the architecture in 3 sentences.

### 1.2 Read: `CMakeLists.txt`
What to look for:
- **Targets**
  - `add_library(liphis_core STATIC ...)` (core engines)
  - `qt_add_executable(liphis_app ...)` (the app)
- QML module:
  - `qt_add_qml_module(liphis_app URI liphis ...)`
- Linking:
  - which Qt modules + external libs are linked

Checkpoint:
- You can answer: “Which files compile into the executable vs the core library?”

### 1.3 Optional but recommended: build & run once
From repo root:
- Configure: `cmake -S . -B build`
- Build: `cmake --build build -j`
- Run (typical): `./build/liphis_app`

If it fails, read the missing dependency from the error and cross-check `CMakeLists.txt` (`find_package(...)`, `pkg_check_modules(...)`).

Checkpoint:
- The app window opens (even if some features aren’t tested yet).

---

## Phase 2 — 60 minutes: “Process entry → QML entry → ‘the app exists’”

### 2.1 Read: `src/main.cpp`
What to look for (in order):
1) **Library init/shutdown**
   - `git_libgit2_init();` and `git_libgit2_shutdown();`
2) **Qt app creation**
   - `QGuiApplication app(argc, argv);`
3) **Single instance logic**
   - `QLocalSocket` + `QLocalServer`
4) **C++ types exposed to QML**
   - `qmlRegisterType<AppController>(...)` etc.
5) **Global singletons provided to QML**
   - `engine.rootContext()->setContextProperty(...)`
6) **QML boot**
   - `engine.loadFromModule("liphis", "Main");`

Checkpoint:
- You can explain: “How does `AppController` become usable from QML?”

### 2.2 Read: `src/qml/Main.qml`
What to look for:
- It’s intentionally tiny. It exists to load the real shell.

Checkpoint:
- You can identify the QML file that is the real UI root.

### 2.3 Read: `src/qml/AppShell.qml`
This is the real UI root (`ApplicationWindow`).

What to look for (don’t read every line; skim with intent):
- Window basics: `ApplicationWindow { ... }`
- Key app-level state: `activeController`, settings objects
- Command system usage:
  - Search for `registerAppCommand(` (keyboard shortcuts / command palette)
- Where controllers are created/managed:
  - Search for `AppController {` or `AnalysisController {` or `DocumentIntelligenceController {`
- Where UI triggers controller actions:
  - Search for `.openPath(`, `.refresh(`, `.deleteItem(`, `.startGlobalSearch(`

Checkpoint:
- You can find one example action in QML that calls into `activeController`.

Mini-exercise:
- Run: `rg -n "activeController\\." src/qml`
- Pick one call that looks important (like navigation/search/delete). Keep it; you’ll trace it in Phase 3.

---

## Phase 3 — 90 minutes: “Controllers: the glue between UI and backend”

Controllers are your “API surface” for the UI. If you understand controllers, the project stops feeling mysterious.

### 3.1 Read: `src/AppController.hpp`
What to look for:
- **Expose state to QML** via `Q_PROPERTY(...)`
  - Example categories:
    - navigation: `currentPath`, history (`canGoBack`, `canGoForward`)
    - selection: `selectedPath(s)`
    - UI state: `viewMode`, `iconSize`, `showHiddenFiles`
    - search: `searchInProgress`, `activeSearchTerm`, etc.
- **Expose actions to QML** via `Q_INVOKABLE`
  - navigation: `openPath`, `goUp`, `refresh`
  - file ops: `createFolder`, `renameItem`, `deleteItems`, `trashItems`, `pasteItem`
  - search: `startGlobalSearch`, `cancelSearch`, `applySearchQuery`
  - advanced: compress/extract, permissions, checksum, open-as-root
- **Signals** that cause the UI to update

Checkpoint:
- You can answer: “Which methods does QML call for file operations?”

### 3.2 Read: `src/AppController.cpp`
This file is big. Use a strategy:

Strategy:
1) Start with the action you picked in Phase 2 (from `activeController.<something>`).
2) Locate that method’s implementation in `src/AppController.cpp`.
3) For that method, identify:
   - Which model is updated (`m_fileModel`, `m_treeModel`, etc.)
   - Which engine function is called (`FileSystemEngine::...`, `AnalysisEngine::...`)
   - Which signal(s) are emitted (`currentPathChanged()`, `loadingChanged()`, etc.)
   - How cancellation is handled (`m_cancelRequested`, `m_operationGeneration`)

Suggested “first trace” (recommended):
- `openPath(...)` or `refresh()`

Checkpoint:
- You can draw a line from “QML action” → “AppController method” → “engine call” → “model update”.

### 3.3 Read: `src/AnalysisController.hpp` then `src/AnalysisController.cpp`
What to look for:
- How it calls `core/AnalysisEngine` and reports progress to QML (signals / properties)

Checkpoint:
- You can explain what “analysis” means in this app (directory size/tree insight).

### 3.4 Read: `src/DocumentIntelligenceController.hpp` then `src/DocumentIntelligenceController.cpp`
Do this last in the controller phase.

Checkpoint:
- You can identify what part is “document intelligence” vs “standard file manager”.

---

## Phase 4 — 90 minutes: “Models: how C++ data becomes QML-friendly”

Models are how QML lists (files, places, trees) get populated.

### 4.1 Read: `core/FileMeta.hpp` (yes, now)
This struct is the “payload” passed around.

What to look for:
- Fields: name/path/isDir/size/timestamps/permissions/mime/icon/thumbnail/etc.
- Any shared-pointer fields (used to reduce allocations / cache strings)

Checkpoint:
- You can list the top 8 fields the UI likely needs for a file row.

### 4.2 Read: `src/models/FileListModel.hpp` then `src/models/FileListModel.cpp`
What to look for:
- `enum Roles { NameRole, PathRole, ... }`
- `roleNames()` maps role integers to role strings that QML uses.
- `data()` returns a QVariant per role.
- Update paths:
  - `setEntries(...)`, `insertBatch(...)`, `removeItems(...)`, `updateThumbnail(...)`

Checkpoint:
- You can answer: “What role name does QML use to get `path` / `isDir` / `formattedSize`?”

Mini-exercise:
- Search QML for one role usage (for example `iconName` or `formattedSize`) and confirm it matches `roleNames()`.

### 4.3 Read: `src/models/FileTreeModel.*`
What to look for:
- How it represents hierarchy vs flat lists.

Checkpoint:
- You can explain why there are two models (list view vs tree view).

### 4.4 Read: `src/models/FileFilterProxyModel.*`
What to look for:
- What filtering/sorting is done at the model level vs controller level.

Checkpoint:
- You can identify where sorting is performed.

### 4.5 Read: `src/models/PlacesModel.*`
What to look for:
- What “places” are (home, disks, bookmarks) and how they get exposed to QML.

Checkpoint:
- You can find where the sidebar items come from.

---

## Phase 5 — 2 hours: “Core engines: where the real work happens”

Now you know what the UI asks for. Here is how the backend fulfills it.

### 5.1 Read: `core/FileSystemEngine.hpp` then `core/FileSystemEngine.cpp`
This is the “filesystem API” for the app.

What to look for:
- Directory listing:
  - `listDirectorySync(...)`
  - `listDirectoryStream(...)` (batching + cancellation)
- File metadata:
  - `getFileMeta(...)`
- Recursive search:
  - `searchRecursive(...)`
- File operations:
  - `renamePath`, `deletePath`, `createDirectory`, `copyPath`, `movePath`, `moveToTrash`, etc.

Checkpoint:
- You can point to the exact function that lists a directory and the one that searches recursively.

### 5.2 Read: `core/AnalysisEngine.hpp` then `core/AnalysisEngine.cpp`
What to look for:
- `analyseDirectoryFast(...)` and how progress is reported.
- `calculateFolderSize(...)`
- `generateTreeString(...)`

Checkpoint:
- You can explain the difference between “folder size” and “analysis tree”.

### 5.3 Read: `core/ThreadPool.hpp`
What to look for:
- How tasks are queued/executed.
- How the app avoids blocking the UI thread.

Checkpoint:
- You can explain why filesystem operations must not run on the UI thread.

### 5.4 Read: `core/FileCommand.hpp` then `core/FileCommand.cpp`
What to look for:
- Command/history design for undo/redo.
- How commands encapsulate actions and store metadata.

Checkpoint:
- You can connect `AppController::undo()` / `redo()` to the history mechanism.

---

## Phase 6 — 60 minutes: “Supporting services (thumbnails, icons, command palette)”

These help UI feel polished. Understand them after the core path.

Read in this order:
1) `src/ThumbnailManager.*`
2) `src/ThumbnailImageProvider.*` (QML `image://thumbs/...`)
3) `src/SystemIconProvider.*` (QML `image://icon/...`)
4) `src/TerminalManager.*`
5) `src/CommandManager.*`

Checkpoint:
- You can explain why `src/main.cpp` installs image providers (`thumbs`, `icon`) and what that enables in QML.

---

## Phase 7 — Finish: Tests (the official “end of the roadmap”)

### 7.1 Read: `tests/unit_tests.cpp`
What to look for:
- What behavior is considered correct.
- How core pieces are expected to behave when called from outside the UI.

Final checkpoint (you’re “done”):
- You can pick any user-facing feature (open folder, search, delete, thumbnails) and trace it:
  - QML file → controller method → model update → core engine method → returned `FileMeta` → QML roles.

---

## Debugging / “Where is this implemented?” (repeatable recipe)

When you want to locate code for a feature:
1) Find the QML trigger:
   - `rg -n "activeController\\.|onClicked|Keys\\.onPressed" src/qml`
2) Find the C++ method (usually `Q_INVOKABLE`) in:
   - `src/AppController.cpp`, `src/AnalysisController.cpp`, `src/DocumentIntelligenceController.cpp`
3) Find model changes:
   - `setEntries`, `insertBatch`, `beginResetModel`, `dataChanged`, signals
4) Find core engine calls:
   - `rg -n "FileSystemEngine::|AnalysisEngine::" src`

---

## “Shortest path” version (if you’re in a hurry)

Read in this exact order:
1) `ABOUT.md`
2) `CMakeLists.txt`
3) `src/main.cpp`
4) `src/qml/AppShell.qml`
5) `src/AppController.hpp`
6) `src/AppController.cpp`
7) `core/FileMeta.hpp`
8) `core/FileSystemEngine.hpp`
9) `core/FileSystemEngine.cpp`
10) `tests/unit_tests.cpp`
