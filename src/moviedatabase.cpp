
// ============== MovieDatabase.cpp ==============
#include "moviedatabase.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QEventLoop>
#include <QTimer>

MovieDatabase::MovieDatabase(const QString& apiBaseUrl) : m_apiBaseUrl(apiBaseUrl) {}

bool MovieDatabase::loadFromApi() {
    clearError();
    QNetworkRequest req(QUrl(m_apiBaseUrl + "/movies"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply* reply = m_network.get(req);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() != QNetworkReply::NoError) {
        setError(reply->errorString());
        reply->deleteLater();
        return false;
    }
    const QByteArray data = reply->readAll();
    reply->deleteLater();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isArray()) {
        setError("Invalid response from API");
        return false;
    }
    m_movies.clear();
    for (const QJsonValue& val : doc.array()) {
        if (val.isObject()) {
            m_movies.append(Movie::fromJson(val.toObject()));
        }
    }
    qDebug() << "Loaded" << m_movies.size() << "movies from API";
    return true;
}

// removed CSV saving in API mode

bool MovieDatabase::addMovie(const Movie& movie) {
    clearError();
    QNetworkRequest req(QUrl(m_apiBaseUrl + "/movies"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject body = movie.toJson();
    if (body.value("date_added").toString().isEmpty()) {
        body["date_added"] = QDate::currentDate().toString("yyyy-MM-dd");
    }
    QNetworkReply* reply = m_network.post(req, QJsonDocument(body).toJson());
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() != QNetworkReply::NoError) {
        setError(reply->errorString());
        reply->deleteLater();
        return false;
    }
    const QByteArray data = reply->readAll();
    reply->deleteLater();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        m_movies.append(Movie::fromJson(doc.object()));
        return true;
    }
    setError("Invalid response from API");
    return false;
}

bool MovieDatabase::updateMovie(const Movie& original, const Movie& movie) {
    QNetworkRequest req(QUrl(m_apiBaseUrl + "/movies"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject payload;
    QJsonObject originalKey;
    originalKey["name"] = original.getName();
    originalKey["year"] = original.getYear();
    originalKey["date_added"] = original.getDateAdded().toString("yyyy-MM-dd");
    payload["original"] = originalKey;
    payload["updated"] = movie.toJson();
    clearError();
    QNetworkReply* reply = m_network.put(req, QJsonDocument(payload).toJson());
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() != QNetworkReply::NoError) {
        setError(reply->errorString());
        reply->deleteLater();
        return false;
    }
    const QByteArray data = reply->readAll();
    reply->deleteLater();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        Movie updated = Movie::fromJson(doc.object());
        for (int i = 0; i < m_movies.size(); ++i) {
            if (m_movies[i].getName() == original.getName() &&
                m_movies[i].getYear() == original.getYear() &&
                m_movies[i].getDateAdded() == original.getDateAdded()) {
                m_movies[i] = updated;
                break;
            }
        }
        return true;
    }
    setError("Invalid response from API");
    return false;
}

bool MovieDatabase::deleteMovie(const Movie& movie) {
    QNetworkRequest req(QUrl(m_apiBaseUrl + "/movies/delete"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject key;
    key["name"] = movie.getName();
    key["year"] = movie.getYear();
    key["date_added"] = movie.getDateAdded().toString("yyyy-MM-dd");
    clearError();
    QNetworkReply* reply = m_network.post(req, QJsonDocument(key).toJson());
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() != QNetworkReply::NoError) {
        setError(reply->errorString());
        reply->deleteLater();
        return false;
    }
    // On success, remove locally
    for (int i = 0; i < m_movies.size(); ++i) {
        if (m_movies[i].getName() == movie.getName() &&
            m_movies[i].getYear() == movie.getYear() &&
            m_movies[i].getDateAdded() == movie.getDateAdded()) {
            m_movies.removeAt(i);
            break;
        }
    }
    reply->deleteLater();
    return true;
}

bool MovieDatabase::waitUntilReady(int timeoutMs) {
    clearError();
    QNetworkRequest req(QUrl(m_apiBaseUrl + "/movies"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply* reply = m_network.get(req);
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(timeoutMs);
    loop.exec();
    bool ok = (reply->error() == QNetworkReply::NoError);
    if (!ok && reply->error() != QNetworkReply::NoError) {
        setError(reply->errorString());
    }
    reply->deleteLater();
    return ok;
}

QVector<Movie> MovieDatabase::searchByName(const QString& name) const {
    QVector<Movie> results;
    for (const Movie& movie : m_movies) {
        if (movie.getName().contains(name, Qt::CaseInsensitive)) {
            results.append(movie);
        }
    }
    return results;
}

QVector<Movie> MovieDatabase::searchByDirector(const QString& director) const {
    QVector<Movie> results;
    for (const Movie& movie : m_movies) {
        if (movie.getDirector().contains(director, Qt::CaseInsensitive)) {
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




