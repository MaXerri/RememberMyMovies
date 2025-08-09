
// ============== MovieDatabase.h ==============
#ifndef MOVIEDATABASE_H
#define MOVIEDATABASE_H

#include "movie.h"
#include <QVector>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class MovieDatabase {
public:
    MovieDatabase(const QString& apiBaseUrl = "http://127.0.0.1:8000");
    
    // Core operations
    bool loadFromApi();
    bool addMovie(const Movie& movie);
    bool updateMovie(const Movie& original, const Movie& updatedMovie);
    bool deleteMovie(const Movie& movie);
    bool waitUntilReady(int timeoutMs = 10000);
    
    // Search functions
    QVector<Movie> getAllMovies() const { return m_movies; }
    QVector<Movie> searchByName(const QString& name) const;
    QVector<Movie> searchByDirector(const QString& director) const;
    QVector<Movie> searchByDateRange(const QDate& startDate, const QDate& endDate) const;
    QVector<Movie> getFavorites() const;
    
    // Utility
    int getMovieCount() const { return m_movies.size(); }
    QString getLastError() const { return m_lastError; }
    QString getApiBaseUrl() const { return m_apiBaseUrl; }
    
private:
    QVector<Movie> m_movies;
    QString m_apiBaseUrl;
    QNetworkAccessManager m_network;
    QString m_lastError;
    
    void clearError() { m_lastError.clear(); }
    void setError(const QString& error) { m_lastError = error; }
};

#endif // MOVIEDATABASE_H



