from __future__ import annotations
from pathlib import Path
import csv
from datetime import datetime, date
import os
from .database import Base, engine, SessionLocal, get_project_root, get_db_path
from .models import Movie

Base.metadata.create_all(bind=engine)


def parse_bool(value: str) -> bool:
    return str(value).strip().lower() in {"1", "true", "yes", "y"}


def main() -> None:
    root = get_project_root()
    csv_path = root / "data" / "movies.csv"
    if not csv_path.exists():
        print(f"CSV not found at {csv_path}")
        return

    with SessionLocal() as db:
        with csv_path.open(newline="", encoding="utf-8") as f:
            reader = csv.DictReader(f)
            count = 0
            for row in reader:
                name = (row.get("Movie Name") or row.get("name") or "").strip()
                year = int(row.get("Year") or row.get("year") or 0)
                director = (row.get("Director") or row.get("director") or "").strip()
                date_str = (row.get("Date Added") or row.get("date_added") or "").strip()
                notes = (row.get("Notes") or row.get("notes") or "").strip()
                fav = parse_bool(row.get("Is Favorite") or row.get("is_favorite") or "0")
                if not name or year <= 0:
                    continue
                try:
                    d = datetime.strptime(date_str, "%Y-%m-%d").date() if date_str else date.today()
                except Exception:
                    d = date.today()
                entity = Movie(
                    name=name,
                    year=year,
                    director=director,
                    date_added=d,
                    notes=notes,
                    is_favorite=fav,
                )
                # Upsert-like behavior based on unique constraint
                existing = (
                    db.query(Movie)
                    .filter(Movie.name == name, Movie.year == year, Movie.date_added == d)
                    .one_or_none()
                )
                if existing is None:
                    db.add(entity)
                    count += 1
            db.commit()
            print(f"Imported {count} new movies into {get_db_path()}")


if __name__ == "__main__":
    main()
