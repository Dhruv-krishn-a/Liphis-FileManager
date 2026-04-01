# Liphis Phase 1-5 Fix Report

Date: 2026-04-01

## Scope Implemented
- Phase 1: Sidebar button UX feedback and responsiveness
- Phase 2: Full filename visibility
- Phase 3: Zoom lag reduction
- Phase 4: Global search performance
- Phase 5: Large-directory responsiveness

## Phase 1 Changes
- Added hover help tooltips and pointer cursor for right-sidebar lifecycle buttons.
- Added tooltips for Git action chips and Analyse button.
- Hardened lifecycle action feedback via `operationNotice` in C++:
  - Explicit success messages for status/current operations.
  - Explicit error when file is not indexed yet.
- Timeline UI now refreshes reactively from `groupsChanged` to avoid stale/no-response feeling.

Files:
- `src/qml/InspectorPanel.qml`
- `src/DocumentIntelligenceController.cpp`

## Phase 2 Changes
- Added filename/path hover tooltips in file views:
  - List view rows
  - Grid view tiles
  - Tree view rows
- This ensures full names/paths are visible even when labels are elided.

File:
- `src/qml/FileView.qml`

## Phase 3 Changes
- Implemented zoom throttling with queued zoom target + timer (`24ms`) to reduce heavy relayout churn.
- Pinch and Ctrl+wheel now update a pending zoom value, then commit batched updates.

File:
- `src/qml/FileView.qml`

## Phase 4 Changes
- Optimized global search root scope usage:
  - `current` searches only current path.
  - `home` searches home path.
  - `mounted` searches mounted volumes.
- Added faster command-path integrations when available:
  - `fd` for filename searches.
  - `rg` for content searches.
- Added excludes for expensive folders (`.git`, `node_modules`, `.cache`).
- Reduced expensive canonical path resolution in dedupe key during search streaming.
- Limited thumbnail requests during global search to an initial budget to avoid UI overload.

File:
- `src/AppController.cpp`

## Phase 5 Changes
- Removed forced `gc()` call on every directory change.
- Changed grid thumbnails to asynchronous loading.
- Enabled view item reuse and reduced extreme cache buffers:
  - ListView: reuse enabled, lower buffer.
  - GridView: reuse enabled, lower buffer.
- Reduced debug logging in hot interaction paths.

File:
- `src/qml/FileView.qml`

## Build/Test Verification
- Build: `cmake --build build -j4` ✅
- Tests: `ctest --test-dir build --output-on-failure` ✅ (2/2 passed)

## Notes
- Phase 6 (Doc Intel modal -> centered/new tab redesign) was not included in this execution because the request explicitly asked to complete phases 1-5.
