
// ============== Movie.cpp ==============
#include "movie.h"
#include <QStringList>

Movie::Movie() : m_year(0), m_dateAdded(QDate::currentDate()), m_isFavorite(false) {}

Movie::Movie(const QString& name, int year, const QString& notes, bool isFavorite)
    : m_name(name), m_year(year), m_dateAdded(QDate::currentDate()), 
      m_notes(notes), m_isFavorite(isFavorite) {}

QString Movie::toCsvString() const {
    QString escapedNotes = m_notes;
    escapedNotes.replace("\"", "\"\""); // Escape quotes in notes
    
    return QString("%1,%2,%3,\"%4\",%5")
           .arg(m_name)
           .arg(m_year)
           .arg(m_dateAdded.toString("yyyy-MM-dd"))
           .arg(escapedNotes)
           .arg(m_isFavorite ? "1" : "0");
}

Movie Movie::fromCsvString(const QString& csvLine) {
    QStringList parts = csvLine.split(',');
    if (parts.size() < 5) return Movie(); // Invalid format
    
    Movie movie;
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
    
    return movie;
}

