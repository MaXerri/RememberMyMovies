## Running MovieReviewApp (Dev and Prod)

This guide covers running the FastAPI backend and the C++/Qt desktop app in development and production.

### Prerequisites
- Backend: Python 3.10+, pip, venv
- Frontend: CMake 3.16+, C++17 compiler, Qt 6 (Core, Widgets, Network)
- macOS: `brew install python cmake qt`

---

## Backend (FastAPI)
API base URL default: `http://127.0.0.1:8000`

### One-time setup
```bash
python -m venv .venv
# macOS/Linux
source .venv/bin/activate
# Windows PowerShell
# .venv\Scripts\Activate.ps1

pip install -r backend/requirements.txt
```

### Development
- macOS/Linux:
  ```bash
  export APP_ENV=development
  uvicorn backend.app:app --reload --port 8000
  ```
- Windows (PowerShell):
  ```powershell
  $env:APP_ENV='development'
  uvicorn backend.app:app --reload --port 8000
  ```
- Dev DB path: `backend/db/dev.db`
- Optional seed from CSV (`data/movies.csv`):
  ```bash
  python -m backend.import_from_csv
  ```

### Production
- macOS/Linux:
  ```bash
  export APP_ENV=production
  # Use 1 worker with SQLite to avoid write-locks
  uvicorn backend.app:app --host 0.0.0.0 --port 8000 --workers 1
  ```
- Windows (PowerShell):
  ```powershell
  $env:APP_ENV='production'
  uvicorn backend.app:app --host 0.0.0.0 --port 8000 --workers 1
  ```
- Prod DB path: `backend/db/prod.db`
- Optional seed (writes to prod DB when `APP_ENV=production`):
  ```bash
  python -m backend.import_from_csv
  ```

---

## Desktop App (C++/Qt)
The app targets `http://127.0.0.1:8000` by default. To change it, adjust the default API base URL in `include/moviedatabase.h`.

### Development build/run
- macOS/Linux:
  ```bash
  mkdir -p build && cd build
  # If CMake cannot find Qt, provide a prefix path (example for Homebrew):
  # cmake -DCMAKE_PREFIX_PATH="$(brew --prefix qt)" ..
  cmake ..
  cmake --build .
  ./MovieReviewApp
  ```
- Windows (PowerShell):
  ```powershell
  mkdir build; cd build
  cmake -G "Visual Studio 17 2022" -A x64 .. -DCMAKE_PREFIX_PATH="C:\\Qt\\6.x.x\\msvcXXXX_64"
  cmake --build . --config Debug
  .\Debug\MovieReviewApp.exe
  ```

### Production (Release) build/run
- macOS:
  ```bash
  mkdir -p build-release && cd build-release
  cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$(brew --prefix qt)" ..
  cmake --build . --config Release
  open MovieReviewApp.app
  ```
- Windows (PowerShell):
  ```powershell
  mkdir build-release; cd build-release
  cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\\Qt\\6.x.x\\msvcXXXX_64" ..
  cmake --build . --config Release
  .\Release\MovieReviewApp.exe
  ```

---

## Verify
```bash
curl http://127.0.0.1:8000/movies
```
Then start the desktop app and confirm the movie list loads.

## Troubleshooting
- CMake cannot find Qt: pass `-DCMAKE_PREFIX_PATH` to Qt (e.g., `$(brew --prefix qt)` on macOS).
- Port in use: run backend on a different `--port` and update the API base URL in `include/moviedatabase.h` if needed.
- SQLite concurrency: keep Uvicorn `--workers 1` to avoid write-locks.
- Virtual environment: make sure it’s activated before `uvicorn` or `python -m backend.import_from_csv`.
