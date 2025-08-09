from __future__ import annotations
from datetime import date
from sqlalchemy import Integer, String, Boolean, Date, UniqueConstraint
from sqlalchemy.orm import Mapped, mapped_column
from .database import Base


class Movie(Base):
    __tablename__ = "movies"
    id: Mapped[int] = mapped_column(Integer, primary_key=True, autoincrement=True)
    name: Mapped[str] = mapped_column(String(255), nullable=False)
    year: Mapped[int] = mapped_column(Integer, nullable=False)
    director: Mapped[str] = mapped_column(String(255), default="")
    date_added: Mapped[date] = mapped_column(Date, nullable=False)
    notes: Mapped[str] = mapped_column(String, default="")
    is_favorite: Mapped[bool] = mapped_column(Boolean, default=False, nullable=False)

    __table_args__ = (
        UniqueConstraint("name", "year", "date_added", name="uq_movie_identity"),
    )
