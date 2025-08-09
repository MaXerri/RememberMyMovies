from __future__ import annotations
from pathlib import Path
import os
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker, declarative_base

Base = declarative_base()


def get_project_root() -> Path:
    # backend/ -> project root
    return Path(__file__).resolve().parents[1]


def get_db_path() -> Path:
    env = os.getenv("APP_ENV", "development").lower()
    db_dir = get_project_root() / "backend" / "db"
    db_dir.mkdir(parents=True, exist_ok=True)
    if env in ("prod", "production"):
        return db_dir / "prod.db"
    return db_dir / "dev.db"


def get_database_url() -> str:
    db_path = get_db_path()
    return f"sqlite:///{db_path}"


engine = create_engine(
    get_database_url(), connect_args={"check_same_thread": False}
)
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
