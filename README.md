## MovieReviewApp

Desktop app to track and review movies with a modern Qt 6 UI and a FastAPI backend. Data is stored in SQLite (separate dev/prod DB files). The app supports adding, editing, deleting, searching, and marking favorites. You can optionally seed from a CSV.

### Architecture
- Frontend: C++/Qt 6 (Core, Widgets, Network)
- Backend: Python FastAPI + Uvicorn, SQLite via SQLAlchemy
- Default API URL: `http://127.0.0.1:8000`

### Prerequisites
- Python 3.10+, pip, venv
- CMake 3.16+, C++17 compiler, Qt 6
- macOS: `brew install python cmake qt`
- Windows: Install Qt 6 (MSVC) and CMake; ensure MSVC toolchain available

### Quick start (macOS/Linux)
1) Backend (terminal 1):
```bash
python -m venv .venv
source .venv/bin/activate
pip install -r backend/requirements.txt
export APP_ENV=production
uvicorn backend.app:app --host 0.0.0.0 --port 8000 --workers 1
```
2) Desktop app (terminal 2):
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
cmake --build build --config Release -j
open build/MovieReviewApp.app
```

### Quick start (Windows PowerShell)
1) Backend (terminal 1):
```powershell
py -3 -m venv .venv
\.venv\Scripts\Activate.ps1
pip install -r backend\requirements.txt
$env:APP_ENV='production'
uvicorn backend.app:app --host 0.0.0.0 --port 8000 --workers 1
```
2) Desktop app (terminal 2):
```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH "C:\Qt\6.x.x\msvcXXXX_64"
cmake --build build --config Release -j
.\build\Release\MovieReviewApp.exe
```

### Seed data from CSV (optional)
Place a file at `data/movies.csv` with headers like `Movie Name,Year,Director,Date Added,Notes,Is Favorite`, then run:
```bash
export APP_ENV=production   # or development
python -m backend.import_from_csv
```

### More details
See `RUNNING.md` for dev vs prod setup, troubleshooting, and customization (icons, build tips).
