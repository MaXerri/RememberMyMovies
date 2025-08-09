#!/usr/bin/env python3
import sys
from pathlib import Path

try:
    from PIL import Image
except ImportError:
    print("Pillow is required. Install with: python -m pip install pillow", file=sys.stderr)
    sys.exit(1)

# Usage: scripts/make_ico.py [path/to/icon.png] [out.ico]
# Creates a multi-size Windows ICO from a square PNG

def main():
    project_root = Path(__file__).resolve().parents[1]
    src_png = Path(sys.argv[1]) if len(sys.argv) > 1 else project_root / 'icon_bg.png'
    out_ico = Path(sys.argv[2]) if len(sys.argv) > 2 else project_root / 'resources' / 'AppIcon.ico'

    if not src_png.exists():
        print(f"Source icon not found: {src_png}", file=sys.stderr)
        sys.exit(1)

    out_ico.parent.mkdir(parents=True, exist_ok=True)

    img = Image.open(src_png).convert('RGBA')
    sizes = [16, 24, 32, 48, 64, 128, 256]
    images = [img.resize((s, s), Image.LANCZOS) for s in sizes]
    images[0].save(out_ico, sizes=[(s, s) for s in sizes])
    print(f"Wrote {out_ico}")

if __name__ == '__main__':
    main()
