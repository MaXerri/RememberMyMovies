from __future__ import annotations
from datetime import date
from typing import List, Optional
from fastapi import FastAPI, HTTPException, Depends
from pydantic import BaseModel, Field
from sqlalchemy.orm import Session
from sqlalchemy import func
from .database import Base, engine, SessionLocal
from .models import Movie as MovieORM

Base.metadata.create_all(bind=engine)

app = FastAPI(title="MovieReviewApp API")


class Movie(BaseModel):
    name: str
    year: int
    director: str = ""
    date_added: date
    notes: str = ""
    is_favorite: bool = False


class MovieCreate(BaseModel):
    name: str
    year: int
    director: str = ""
    notes: str = ""
    is_favorite: bool = False
    date_added: Optional[date] = None


class MovieKey(BaseModel):
    name: str
    year: int
    date_added: date


class MovieUpdate(BaseModel):
    original: MovieKey
    updated: Movie


def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()


@app.get("/movies", response_model=List[Movie])
def list_movies(db: Session = Depends(get_db)):
    rows = db.query(MovieORM).order_by(MovieORM.date_added.desc(), MovieORM.id.desc()).all()
    return [
        Movie(
            name=row.name,
            year=row.year,
            director=row.director or "",
            date_added=row.date_added,
            notes=row.notes or "",
            is_favorite=row.is_favorite,
        )
        for row in rows
    ]


@app.post("/movies", response_model=Movie)
def create_movie(payload: MovieCreate, db: Session = Depends(get_db)):
    effective_date = payload.date_added or date.today()
    # Prevent duplicates by name (case-insensitive) + year regardless of date
    duplicate = (
        db.query(MovieORM)
        .filter(
            func.lower(MovieORM.name) == (payload.name or "").strip().lower(),
            MovieORM.year == payload.year,
        )
        .first()
    )
    if duplicate is not None:
        raise HTTPException(status_code=409, detail="Movie with the same name and year already exists")
    entity = MovieORM(
        name=payload.name.strip(),
        year=payload.year,
        director=(payload.director or "").strip(),
        date_added=effective_date,
        notes=(payload.notes or "").strip(),
        is_favorite=bool(payload.is_favorite),
    )
    db.add(entity)
    try:
        db.commit()
    except Exception as exc:
        db.rollback()
        raise HTTPException(status_code=400, detail=f"Could not create movie: {exc}")
    db.refresh(entity)
    return Movie(
        name=entity.name,
        year=entity.year,
        director=entity.director or "",
        date_added=entity.date_added,
        notes=entity.notes or "",
        is_favorite=entity.is_favorite,
    )


@app.put("/movies", response_model=Movie)
def update_movie(payload: MovieUpdate, db: Session = Depends(get_db)):
    row = (
        db.query(MovieORM)
        .filter(
            MovieORM.name == payload.original.name,
            MovieORM.year == payload.original.year,
            MovieORM.date_added == payload.original.date_added,
        )
        .one_or_none()
    )
    if row is None:
        raise HTTPException(status_code=404, detail="Movie not found")

    row.name = payload.updated.name.strip()
    row.year = payload.updated.year
    row.director = (payload.updated.director or "").strip()
    row.date_added = payload.updated.date_added
    row.notes = (payload.updated.notes or "").strip()
    row.is_favorite = bool(payload.updated.is_favorite)

    try:
        db.commit()
    except Exception as exc:
        db.rollback()
        raise HTTPException(status_code=400, detail=f"Could not update movie: {exc}")
    db.refresh(row)
    return Movie(
        name=row.name,
        year=row.year,
        director=row.director or "",
        date_added=row.date_added,
        notes=row.notes or "",
        is_favorite=row.is_favorite,
    )


@app.post("/movies/delete")
def delete_movie(payload: MovieKey, db: Session = Depends(get_db)):
    row = (
        db.query(MovieORM)
        .filter(
            MovieORM.name == payload.name,
            MovieORM.year == payload.year,
            MovieORM.date_added == payload.date_added,
        )
        .one_or_none()
    )
    if row is None:
        raise HTTPException(status_code=404, detail="Movie not found")
    db.delete(row)
    db.commit()
    return {"ok": True}
