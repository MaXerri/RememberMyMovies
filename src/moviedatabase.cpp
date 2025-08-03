
// ============== MovieDatabase.cpp ==============
#include "moviedatabase.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>

MovieDatabase::MovieDatabase(const QString& csvFilePath) : m_csvFilePath(csvFilePath) {
    // Ensure the CSV file exists
    if (!QFile::exists(m_csvFilePath)) {
        QFile file(m_csvFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "Movie Name,Year,Date Added,Notes,Is Favorite\n";
            file.close();
        }
    }
}

bool MovieDatabase::loadFromCsv() {
    clearError();
    QFile file(m_csvFilePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setError("Cannot open CSV file for reading");
        return false;
    }
    
    QTextStream in(&file);
    m_movies.clear();
    
    // Skip header line
    if (!in.atEnd()) {
        in.readLine();
    }
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            Movie movie = Movie::fromCsvString(line);
            if (!movie.getName().isEmpty()) {
                m_movies.append(movie);
            }
        }
    }
    
    file.close();
    qDebug() << "Loaded" << m_movies.size() << "movies from CSV";
    return true;
}

bool MovieDatabase::saveToCsv() {
    clearError();
    QFile file(m_csvFilePath);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError("Cannot open CSV file for writing");
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header
    out << "Movie Name,Year,Date Added,Notes,Is Favorite\n";
    
    // Write movie data
    for (const Movie& movie : m_movies) {
        out << movie.toCsvString() << "\n";
    }
    
    file.close();
    qDebug() << "Saved" << m_movies.size() << "movies to CSV";
    return true;
}

void MovieDatabase::addMovie(const Movie& movie) {
    m_movies.append(movie);
    saveToCsv(); // Auto-save after adding
}

bool MovieDatabase::updateMovie(int index, const Movie& movie) {
    if (index < 0 || index >= m_movies.size()) {
        setError("Invalid movie index for update");
        return false;
    }
    
    m_movies[index] = movie;
    return saveToCsv();
}

bool MovieDatabase::deleteMovie(const Movie& movie) {
    for (int i = 0; i < m_movies.size(); ++i) {
        if (m_movies[i].getName() == movie.getName() && 
            m_movies[i].getYear() == movie.getYear() &&
            m_movies[i].getDateAdded() == movie.getDateAdded()) {
            m_movies.removeAt(i);
            return saveToCsv();
        }
    }
    
    setError("Movie not found for deletion");
    return false;
}

QVector<Movie> MovieDatabase::searchByName(const QString& name) const {
    QVector<Movie> results;
    for (const Movie& movie : m_movies) {
        if (movie.getName().compare(name, Qt::CaseInsensitive) == 0) {
            results.append(movie);
        }
    }
    return results;
}

QVector<Movie> MovieDatabase::searchByDateRange(const QDate& startDate, const QDate& endDate) const {
    QVector<Movie> results;
    for (const Movie& movie : m_movies) {
        QDate movieDate = movie.getDateAdded();
        if (movieDate >= startDate && movieDate <= endDate) {
            results.append(movie);
        }
    }
    return results;
}

QVector<Movie> MovieDatabase::getFavorites() const {
    QVector<Movie> results;
    for (const Movie& movie : m_movies) {
        if (movie.isFavorite()) {
            results.append(movie);
        }
    }
    return results;
}




