@echo off
REM Ensures that this script is run from the project root.
pushd "%~dp0"

echo --- Setting up MovieReviewApp for Windows ---

REM 1. Check for Python
python --version >nul 2>nul
if %errorlevel% neq 0 (
    echo Python is not installed or not in your PATH.
    echo Please install Python from python.org and try again.
    goto :eof
)

REM 2. Generate Windows icon
echo --- Generating Windows icon ---
REM The make_ico.py script requires Pillow
python -m pip install --user pillow
python scripts\make_ico.py

REM 3. Set up Python backend
echo --- Setting up Python backend ---
if not exist "backend\.venv" (
    echo Creating Python virtual environment...
    python -m venv backend\.venv
)
echo Activating virtual environment and installing dependencies...
call backend\.venv\Scripts\activate.bat
pip install -r backend\requirements.txt

REM 4. Build C++ application
echo --- Building C++ application ---
echo Note: This requires CMake and a C++ compiler (like Visual Studio) to be installed.
cmake -S . -B build
cmake --build build

echo --- Setup complete! ---
echo You can now run the application from: build\Debug\MovieReviewApp.exe

echo To run the backend server, open a new terminal and run the following commands:
echo cd backend
echo .venv\Scripts\activate.bat
echo python app.py

popd

