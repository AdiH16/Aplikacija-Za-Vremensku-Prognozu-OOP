#include "WeatherApi.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "weatherdata.h"

WeatherApi::WeatherApi(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &WeatherApi::replyFinished);
}

void WeatherApi::makeRequestforLatandLong(const QString& cityName)
{
    QUrl url(QString("http://api.openweathermap.org/data/2.5/weather?q=%1&appid=e43a65503edf85fd87e11e151341ad08").arg(cityName));
    QNetworkRequest request(url);

    manager->get(request);
}


void WeatherApi::replyFinished(QNetworkReply *reply) {
    if (reply->error()) {
        qDebug() << "Network error:" << reply->errorString();
    } else {
        QByteArray responseData = reply->readAll();
        if (isRequestForCoordinates) {
            parseAndStoreCoordinates(responseData);
            makeRequestForData(); // Make the second API request
        } else {
            parseWeatherData(responseData); // Parse weather data
        }
    }
    reply->deleteLater();
}


void WeatherApi::makeRequestForData(){
    QUrl url(QString("https://api.openweathermap.org/data/2.5/weather?lat=%1&lon=%2&appid=e43a65503edf85fd87e11e151341ad08&units=metric").arg(weather.getLatitude()).arg(weather.getLongitude()));
    QNetworkRequest request(url);
    isRequestForCoordinates = false;
    manager->get(request);
};


void WeatherApi::parseAndStoreCoordinates(const QByteArray &jsonData) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject jsonObj = doc.object();

        QJsonObject coordObj = jsonObj["coord"].toObject();
        weather.setLongitude( coordObj["lon"].toDouble());
        weather.setLatitude(coordObj["lat"].toDouble());

        qDebug() << "Longitude:" << weather.getLongitude();
        qDebug() << "Latitude:" << weather.getLatitude();


    } else {
        qDebug() << "Invalid JSON document or format not as expected";
    }
}


void WeatherApi::parseWeatherData(const QByteArray &jsonData) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject jsonObj = doc.object();

        // Parsing weather array
        QJsonArray weatherArray = jsonObj["weather"].toArray();
        if (!weatherArray.isEmpty()) {
            QJsonObject weatherObj = weatherArray.first().toObject();
            weather.setCurrentWeather(weatherObj["main"].toString());
            weather.setDescription(weatherObj["description"].toString());
            weather.setIcon(weatherObj["icon"].toString());
        }

        // Parsing main object for temperature and humidity
        QJsonObject mainObj = jsonObj["main"].toObject();
        weather.setTemperature(mainObj["temp"].toDouble());
        weather.setHumidity(mainObj["humidity"].toDouble());

        // Parsing wind object for wind speed
        QJsonObject windObj = jsonObj["wind"].toObject();
        weather.setwindSpeed(windObj["speed"].toDouble());

        qDebug()<<weather.getwindSpeed() << " " << weather.getCurrentWeather() << " " << weather.getDescription() << " " << weather.getIcon()<< " " << weather.getTemperature();
        // Add more parsing as necessary based on the WeatherDataALL class
    } else {
        qDebug() << "Invalid JSON document or format not as expected";
    }
}
