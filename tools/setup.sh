#!/usr/bin/env bash
set -euo pipefail

echo "--- Setting up MovieReviewApp ---"

# 1. Generate platform-specific icon
OS="$(uname -s)"
case "$OS" in
    Darwin)
        echo "--- Generating macOS icon ---"
        scripts/make_icns.sh
        ;;
    Linux)
        echo "--- Skipping icon generation for Linux ---"
        ;;
    *)
        echo "--- Skipping icon generation for unsupported OS: $OS ---"
        ;;
esac

# 2. Set up Python backend
echo "--- Setting up Python backend ---"
if [ ! -d "backend/.venv" ]; then
  python3 -m venv backend/.venv
fi
source backend/.venv/bin/activate
pip install -r backend/requirements.txt

# 3. Build C++ application
echo "--- Building C++ application ---"
cmake -S . -B build
cmake --build build

echo "--- Setup complete! ---"
if [[ "$OS" == "Darwin" ]]; then
    echo "You can now run the application from build/MovieReviewApp.app"
else
    echo "You can now run the application from build/MovieReviewApp"
fi
echo "To run the backend server, open a new terminal and run the following commands:"
echo "cd backend"
echo "source .venv/bin/activate"
echo "python app.py"