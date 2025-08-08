
// ============== MovieDatabase.h ==============
#ifndef MOVIEDATABASE_H
#define MOVIEDATABASE_H

#include "movie.h"
#include <QVector>
#include <QString>

class MovieDatabase {
public:
    MovieDatabase(const QString& csvFilePath = "movies.csv");
    
    // Core operations
    bool loadFromCsv();
    bool saveToCsv();
    void addMovie(const Movie& movie);
    bool updateMovie(int index, const Movie& movie);
    bool deleteMovie(const Movie& movie);
    
    // Search functions
    QVector<Movie> getAllMovies() const { return m_movies; }
    QVector<Movie> searchByName(const QString& name) const;
    QVector<Movie> searchByDirector(const QString& director) const;
    QVector<Movie> searchByDateRange(const QDate& startDate, const QDate& endDate) const;
    QVector<Movie> getFavorites() const;
    
    // Utility
    int getMovieCount() const { return m_movies.size(); }
    QString getLastError() const { return m_lastError; }
    
private:
    QVector<Movie> m_movies;
    QString m_csvFilePath;
    QString m_lastError;
    
    void clearError() { m_lastError.clear(); }
    void setError(const QString& error) { m_lastError = error; }
};

#endif // MOVIEDATABASE_H



