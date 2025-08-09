#!/usr/bin/env bash
set -euo pipefail

# Usage: scripts/make_icns.sh [path/to/icon.png]
# Generates resources/AppIcon.icns from a square PNG using macOS sips+iconutil.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
SRC_PNG="${1:-$PROJECT_ROOT/icon_bg.png}"
ICONSET_DIR="$PROJECT_ROOT/macos/AppIcon.iconset"
OUT_DIR="$PROJECT_ROOT/resources"
OUT_ICNS="$OUT_DIR/AppIcon.icns"

if [[ ! -f "$SRC_PNG" ]]; then
  echo "Source icon not found: $SRC_PNG" >&2
  exit 1
fi

mkdir -p "$ICONSET_DIR" "$OUT_DIR"

sips -z 16 16     "$SRC_PNG" --out "$ICONSET_DIR/icon_16x16.png"
sips -z 32 32     "$SRC_PNG" --out "$ICONSET_DIR/icon_16x16@2x.png"
sips -z 32 32     "$SRC_PNG" --out "$ICONSET_DIR/icon_32x32.png"
sips -z 64 64     "$SRC_PNG" --out "$ICONSET_DIR/icon_32x32@2x.png"
sips -z 128 128   "$SRC_PNG" --out "$ICONSET_DIR/icon_128x128.png"
sips -z 256 256   "$SRC_PNG" --out "$ICONSET_DIR/icon_128x128@2x.png"
sips -z 256 256   "$SRC_PNG" --out "$ICONSET_DIR/icon_256x256.png"
sips -z 512 512   "$SRC_PNG" --out "$ICONSET_DIR/icon_256x256@2x.png"
sips -z 512 512   "$SRC_PNG" --out "$ICONSET_DIR/icon_512x512.png"
sips -z 1024 1024 "$SRC_PNG" --out "$ICONSET_DIR/icon_512x512@2x.png"

iconutil -c icns "$ICONSET_DIR" -o "$OUT_ICNS"
echo "Wrote $OUT_ICNS"
