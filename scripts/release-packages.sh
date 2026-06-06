#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
# Changed BUILD_DIR name to 'build_linux' to avoid any 'liphis' folder collision
BUILD_DIR="${ROOT_DIR}/build_linux"
DIST_DIR="${ROOT_DIR}/dist"
APPDIR="${DIST_DIR}/AppDir"
SKIP_APPIMAGE=0

for arg in "$@"; do
  case "${arg}" in
    --clean)
      rm -rf "${BUILD_DIR}" "${DIST_DIR}"
      ;;
    --skip-appimage)
      SKIP_APPIMAGE=1
      ;;
    *)
      ;;
  esac
done

mkdir -p "${BUILD_DIR}" "${DIST_DIR}"

# Clean up any existing artifacts in dist
rm -f "${DIST_DIR}"/*.deb "${DIST_DIR}"/*.rpm "${DIST_DIR}"/*.tar.gz "${DIST_DIR}"/*.zip "${DIST_DIR}"/*.AppImage 2>/dev/null || true

# Run CMake with a specific output directory for QML to prevent name collision with binary
cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" \
      -DCMAKE_BUILD_TYPE=Release \
      -G Ninja

cmake --build "${BUILD_DIR}" -j"$(nproc)"
ctest --test-dir "${BUILD_DIR}" --output-on-failure

# Prepare AppDir for AppImage and generic packaging
rm -rf "${APPDIR}"
DESTDIR="${APPDIR}" cmake --install "${BUILD_DIR}"

pushd "${BUILD_DIR}" >/dev/null
# Generate standard packages
if command -v dpkg >/dev/null 2>&1; then
  cpack -G DEB
else
  echo "Skipping DEB: dpkg not found."
fi

if command -v rpmbuild >/dev/null 2>&1; then
  cpack -G RPM
else
  echo "Skipping RPM: rpmbuild not found."
fi

cpack -G TGZ
cpack -G ZIP
popd >/dev/null

# Copy artifacts to dist
cp -f "${BUILD_DIR}"/*.deb "${DIST_DIR}/" 2>/dev/null || true
cp -f "${BUILD_DIR}"/*.rpm "${DIST_DIR}/" 2>/dev/null || true
cp -f "${BUILD_DIR}"/*.tar.gz "${DIST_DIR}/" 2>/dev/null || true
cp -f "${BUILD_DIR}"/*.zip "${DIST_DIR}/" 2>/dev/null || true

# AppImage generation (x86_64 only)
ARCH="$(uname -m)"
if [[ "${SKIP_APPIMAGE}" -eq 1 ]]; then
  echo "Skipping AppImage by request (--skip-appimage)."
elif [[ "${ARCH}" == "x86_64" ]]; then
  TOOLS_DIR="${DIST_DIR}/tools"
  mkdir -p "${TOOLS_DIR}"

  LINUXDEPLOY="${TOOLS_DIR}/linuxdeploy-x86_64.AppImage"
  QT_PLUGIN="${TOOLS_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage"
  LINUXDEPLOY_BIN="${TOOLS_DIR}/linuxdeploy"
  QT_PLUGIN_BIN="${TOOLS_DIR}/linuxdeploy-plugin-qt"

  if [[ ! -x "${LINUXDEPLOY}" ]]; then
    curl -fsSL -o "${LINUXDEPLOY}" "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
    chmod +x "${LINUXDEPLOY}"
  fi

  if [[ ! -x "${QT_PLUGIN}" ]]; then
    curl -fsSL -o "${QT_PLUGIN}" "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
    chmod +x "${QT_PLUGIN}"
  fi

  ln -sf "${LINUXDEPLOY}" "${LINUXDEPLOY_BIN}"
  ln -sf "${QT_PLUGIN}" "${QT_PLUGIN_BIN}"

  if command -v qmake6 >/dev/null 2>&1; then
    ln -sf "$(command -v qmake6)" "${TOOLS_DIR}/qmake"
    export QMAKE="${TOOLS_DIR}/qmake"
    export QT_QMAKE="${QMAKE}"
    export QT_SELECT=qt6
  elif command -v qmake >/dev/null 2>&1; then
    export QMAKE="$(command -v qmake)"
    export QT_QMAKE="${QMAKE}"
  else
    echo "AppImage generation requires qmake6 or qmake for linuxdeploy-plugin-qt." >&2
    exit 1
  fi

  export QML_SOURCES_PATHS="${ROOT_DIR}/src/qml"
  export APPIMAGE_EXTRACT_AND_RUN=1
  export PATH="${TOOLS_DIR}:${PATH}"

  (cd "${DIST_DIR}" && \
    "${LINUXDEPLOY_BIN}" \
      --appdir "${APPDIR}" \
      -d "${ROOT_DIR}/packaging/liphis.desktop" \
      -i "${ROOT_DIR}/src/qml/assets/logo-dark.png" \
      --plugin qt \
      --output appimage)
else
  echo "Skipping AppImage: unsupported architecture '${ARCH}' (requires x86_64)."
fi

echo "Artifacts generated in: ${DIST_DIR}"
ls -1 "${DIST_DIR}"
