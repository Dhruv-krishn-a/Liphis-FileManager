#!/bin/bash
# Helper script to update AUR package files
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

cd "$SCRIPT_DIR"

# Update PKGBUILD version if needed (optional for -git packages, but good for local check)
makepkg --printsrcinfo > .SRCINFO

echo "AUR files updated in $SCRIPT_DIR"
