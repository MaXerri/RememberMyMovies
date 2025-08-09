# MovieReviewApp Internals

This document explains the architecture and key data flows of MovieReviewApp.

## Overview
- Desktop app: C++/Qt 6
- Backend API: Python FastAPI (Uvicorn)
- Storage: SQLite via SQLAlchemy ORM
- Transport: JSON over HTTP, API base `http://127.0.0.1:8000`

## Data model
Backend ORM (`backend/models.py`):
- `movies` table
  - `id` (PK, autoincrement)
  - `name` (str, required)
  - `year` (int, required)
  - `director` (str, default "")
  - `date_added` (date, required)
  - `notes` (str, default "")
  - `is_favorite` (bool, default false)
  - Unique constraint: (`name`, `year`, `date_added`) as `uq_movie_identity`

Implications:
- A logical movie identity is name+year+date_added. Updates use the original identity to find and replace the row.
- Duplicate insertions with the same identity will be rejected by the backend with 409.

## Backend API
- `GET /movies` → list of movies (JSON array)
- `POST /movies` → create a movie; expects fields in the response model. If `date_added` missing, UI sends today.
- `PUT /movies` → update; payload: `{ original: {name, year, date_added}, updated: Movie }`; returns updated Movie.
- `POST /movies/delete` → delete by identity; body: `{name, year, date_added}`.

DB selection
- Env var `APP_ENV=development|production` sets DB path:
  - Dev → `backend/db/dev.db`
  - Prod → `backend/db/prod.db`

CSV import
- `python -m backend.import_from_csv` reads `data/movies.csv` and performs upsert-like inserts (skip if identity exists).

## Frontend architecture
Classes:
- `Movie` (C++): in-memory DTO for a row; can convert to/from JSON for API payloads.
- `MovieDatabase` (C++): data access layer that talks to the API using `QNetworkAccessManager`.
- `MainWindow` (C++): Qt UI; owns a `MovieDatabase`, displays a table with sorting, search, and edit/delete.

Key behaviors:
- On startup, `MainWindow` calls `MovieDatabase::waitUntilReady` (with timeout) then `loadFromApi()`; movies are stored in memory (`m_movies`).
- All write operations (`addMovie`, `updateMovie`, `deleteMovie`) are synchronous: wait for HTTP reply, update `m_movies`, return success/failure. The UI then refreshes and reflects changes immediately.
- `searchByName`, `searchByDirector`, `searchByDateRange`, and `getFavorites` operate on the in-memory list for responsiveness.
- Sorting is applied client-side in the UI before rendering rows.

## Error handling
- Backend: raises 409 on duplicate create; 404 on missing for update/delete; 400 on validation/commit errors. Errors are surfaced as JSON and mapped to HTTPException details.
- Frontend: if a network error or API error occurs, `MovieDatabase` sets `m_lastError` and returns `false`; `MainWindow` shows a `QMessageBox` with the error.

## Configuration points
- API base URL: constructor default in `include/moviedatabase.h` → change for remote server.
- DB env: `APP_ENV` switches dev/prod DB files.
- Uvicorn workers: use 1 with SQLite to avoid write locks; if moving to Postgres, you can increase.

## Packaging and Icons
- macOS: `.icns` placed at `resources/AppIcon.icns`, referenced by `CMakeLists.txt` (CFBundleIconFile = `AppIcon`).
- Windows: `.ico` at `resources/AppIcon.ico`, embedded via a generated `.rc` and marked as a GUI app.

## Future improvements
- Paginate `GET /movies` when data grows, with server-side filtering/sorting.
- Switch to Postgres for concurrent writes.
- Add CI builds and signed installers for macOS/Windows.
