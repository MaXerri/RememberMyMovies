@echo off
pushd "%~dp0"

echo --- Starting MovieReviewApp ---

REM Activate Python environment
call backend\.venv\Scripts\activate.bat

REM Start backend server in a new window
echo --- Starting backend server in a new window ---
start "MovieReview Backend" cmd /c "uvicorn backend.app:app --host 0.0.0.0 --port 8000 --workers 1"

REM Run the frontend application
echo --- Launching frontend application ---
REM The 'start /wait' command ensures this script waits until the app is closed.
REM Check for executable in both standard build and Debug build locations.
set APP_EXE=build\MovieReviewApp.exe
if not exist %APP_EXE% set APP_EXE=build\Debug\MovieReviewApp.exe

if not exist %APP_EXE% (
    echo Application executable not found at %APP_EXE%
    goto :eof
)

start "MovieReviewApp" /wait %APP_EXE%

echo --- Application closed ---
echo Please manually close the backend server window.

popd
