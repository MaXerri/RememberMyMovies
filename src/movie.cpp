
// ============== Movie.cpp ==============
#include "movie.h"
#include <QStringList>
#include <QJsonObject>
#include <QJsonValue>

Movie::Movie() : m_year(0), m_dateAdded(QDate::currentDate()), m_isFavorite(false) {}

Movie::Movie(const QString& name, int year, const QString& notes, bool isFavorite)
    : m_name(name), m_year(year), m_dateAdded(QDate::currentDate()), 
      m_notes(notes), m_isFavorite(isFavorite) {}

Movie::Movie(const QString& name, int year, const QString& director, const QString& notes, bool isFavorite)
    : m_name(name), m_year(year), m_dateAdded(QDate::currentDate()),
      m_director(director), m_notes(notes), m_isFavorite(isFavorite) {}

QString Movie::toCsvString() const {
    QString escapedNotes = m_notes;
    escapedNotes.replace("\"", "\"\""); // Escape quotes in notes
    
    QString escapedDirector = m_director;
    escapedDirector.replace("\"", "\"\"");
    
    return QString("%1,%2,\"%3\",%4,\"%5\",%6")
           .arg(m_name)
           .arg(m_year)
           .arg(escapedDirector)
           .arg(m_dateAdded.toString("yyyy-MM-dd"))
           .arg(escapedNotes)
           .arg(m_isFavorite ? "1" : "0");
}

Movie Movie::fromCsvString(const QString& csvLine) {
    QStringList parts = csvLine.split(',');
    if (parts.size() < 5) return Movie(); // Invalid format
    
    Movie movie;
    if (parts.size() >= 6) {
        // New format: Name, Year, Director, Date, Notes, Favorite
        movie.setName(parts[0].trimmed());
        movie.setYear(parts[1].toInt());
        // Handle quoted director
        QString director = parts[2].trimmed();
        if (director.startsWith("\"") && director.endsWith("\"")) {
            director = director.mid(1, director.length() - 2);
            director.replace("\"\"", "\"");
        }
        movie.setDirector(director);
        movie.m_dateAdded = QDate::fromString(parts[3].trimmed(), "yyyy-MM-dd");
        // Handle quoted notes (remove quotes and unescape)
        QString notes = parts[4].trimmed();
        if (notes.startsWith("\"") && notes.endsWith("\"")) {
            notes = notes.mid(1, notes.length() - 2);
            notes.replace("\"\"", "\"");
        }
        movie.setNotes(notes);
        movie.setFavorite(parts[5].trimmed() == "1");
    } else {
        // Legacy format: Name, Year, Date, Notes, Favorite
        movie.setName(parts[0].trimmed());
        movie.setYear(parts[1].toInt());
        movie.m_dateAdded = QDate::fromString(parts[2].trimmed(), "yyyy-MM-dd");
        // Handle quoted notes (remove quotes and unescape)
        QString notes = parts[3].trimmed();
        if (notes.startsWith("\"") && notes.endsWith("\"")) {
            notes = notes.mid(1, notes.length() - 2);
            notes.replace("\"\"", "\"");
        }
        movie.setNotes(notes);
        movie.setFavorite(parts[4].trimmed() == "1");
    }
    
    return movie;
}

QJsonObject Movie::toJson() const {
    QJsonObject obj;
    obj["name"] = m_name;
    obj["year"] = m_year;
    obj["director"] = m_director;
    obj["date_added"] = m_dateAdded.toString("yyyy-MM-dd");
    obj["notes"] = m_notes;
    obj["is_favorite"] = m_isFavorite;
    return obj;
}

Movie Movie::fromJson(const QJsonObject& obj) {
    Movie movie;
    movie.setName(obj.value("name").toString());
    movie.setYear(obj.value("year").toInt());
    movie.setDirector(obj.value("director").toString());
    movie.m_dateAdded = QDate::fromString(obj.value("date_added").toString(), "yyyy-MM-dd");
    movie.setNotes(obj.value("notes").toString());
    movie.setFavorite(obj.value("is_favorite").toBool());
    return movie;
}

