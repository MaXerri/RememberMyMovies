#// ============== Movie.h ==============
#ifndef MOVIE_H
#define MOVIE_H

#include <QString>
#include <QDate>

class Movie {
public:
    Movie();
    Movie(const QString& name, int year, const QString& notes, bool isFavorite);
    
    // Getters
    QString getName() const { return m_name; }
    int getYear() const { return m_year; }
    QDate getDateAdded() const { return m_dateAdded; }
    QString getNotes() const { return m_notes; }
    bool isFavorite() const { return m_isFavorite; }
    
    // Setters
    void setName(const QString& name) { m_name = name; }
    void setYear(int year) { m_year = year; }
    void setNotes(const QString& notes) { m_notes = notes; }
    void setFavorite(bool favorite) { m_isFavorite = favorite; }
    void setDateAdded(const QDate& date) { m_dateAdded = date; }
    
    // CSV conversion
    QString toCsvString() const;
    static Movie fromCsvString(const QString& csvLine);
    
private:
    QString m_name;
    int m_year;
    QDate m_dateAdded;
    QString m_notes;
    bool m_isFavorite;
};

#endif // MOVIE_Hendif // MOVIE_H


