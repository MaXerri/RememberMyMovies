MovieReviewApp Backend

This is a FastAPI app that serves the MovieReview API consumed by the Qt frontend.

### Prerequisites
- Python 3.10+
- pip and venv

### Development
1) Create and activate a virtual environment, then install deps
```bash
python -m venv .venv
# macOS/Linux
source .venv/bin/activate
# Windows PowerShell
# .venv\Scripts\Activate.ps1

pip install -r backend/requirements.txt
```

2) Run the API with auto-reload
```bash
export APP_ENV=development    # Windows: $env:APP_ENV='development'
uvicorn backend.app:app --reload --port 8000
```

3) Smoke test
```bash
curl http://127.0.0.1:8000/movies
```

Dev DB path: `backend/db/dev.db`

### Production
1) Use the same virtual environment and dependencies as above

2) Run without reload and with workers
```bash
export APP_ENV=production     # Windows: $env:APP_ENV='production'
uvicorn backend.app:app --host 0.0.0.0 --port 8000 --workers 2
```

3) Smoke test
```bash
curl http://127.0.0.1:8000/movies
```

Prod DB path: `backend/db/prod.db`

### Import existing CSV data
Put your CSV at `data/movies.csv` in the repo root, then run:
```bash
python -m backend.import_from_csv
```
This writes to the DB indicated by `APP_ENV`.
