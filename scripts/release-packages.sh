#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build-release"
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
rm -f "${DIST_DIR}"/*.deb "${DIST_DIR}"/*.rpm "${DIST_DIR}"/*.tar.gz "${DIST_DIR}"/*.zip "${DIST_DIR}"/*.AppImage 2>/dev/null || true
rm -f "${BUILD_DIR}"/*.deb "${BUILD_DIR}"/*.rpm "${BUILD_DIR}"/*.tar.gz "${BUILD_DIR}"/*.zip "${BUILD_DIR}"/*.AppImage 2>/dev/null || true

cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release
cmake --build "${BUILD_DIR}" -j"$(nproc)"
ctest --test-dir "${BUILD_DIR}" --output-on-failure

rm -rf "${APPDIR}"
cmake --install "${BUILD_DIR}" --prefix "${APPDIR}/usr"

mkdir -p "${APPDIR}/usr/share/applications" "${APPDIR}/usr/share/icons/hicolor/512x512/apps"
cp "${ROOT_DIR}/packaging/liphis.desktop" "${APPDIR}/usr/share/applications/liphis.desktop"
cp "${ROOT_DIR}/LiphisApp-Logo.png" "${APPDIR}/usr/share/icons/hicolor/512x512/apps/liphis.png"

pushd "${BUILD_DIR}" >/dev/null
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

cp -f "${BUILD_DIR}"/*.deb "${DIST_DIR}/" 2>/dev/null || true
cp -f "${BUILD_DIR}"/*.rpm "${DIST_DIR}/" 2>/dev/null || true
cp -f "${BUILD_DIR}"/*.tar.gz "${DIST_DIR}/" 2>/dev/null || true
cp -f "${BUILD_DIR}"/*.zip "${DIST_DIR}/" 2>/dev/null || true

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

  export QML_SOURCES_PATHS="${ROOT_DIR}/src/qml"
  export LD_LIBRARY_PATH="${APPDIR}/usr/lib:${LD_LIBRARY_PATH:-}"
  export APPIMAGE_EXTRACT_AND_RUN=1
  export PATH="${TOOLS_DIR}:${PATH}"

  (cd "${DIST_DIR}" && \
    "${LINUXDEPLOY_BIN}" \
      --appdir "${APPDIR}" \
      -d "${ROOT_DIR}/packaging/liphis.desktop" \
      -i "${ROOT_DIR}/LiphisApp-Logo.png" \
      --plugin qt \
      --output appimage)
else
  echo "Skipping AppImage: unsupported architecture '${ARCH}' (requires x86_64)."
fi

echo "Artifacts generated in: ${DIST_DIR}"
ls -1 "${DIST_DIR}"
