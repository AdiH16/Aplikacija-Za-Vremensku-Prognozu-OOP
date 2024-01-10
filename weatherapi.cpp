#include "WeatherApi.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimeZone>
#include "weatherapi.h"
#include "weatherdata.h"

WeatherApi::WeatherApi(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &WeatherApi::replyFinished);
    this->currentRequestType=CurrentWeather;

}

void WeatherApi::makeRequestforLatandLong(const QString& cityName)
{
    currentRequestType=CurrentWeather;
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
            makeForecastRequest();
        } else {
            parseForecastData(responseData);
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
    try {
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isNull() || !doc.isObject()) {
            throw std::runtime_error("Invalid JSON document or format not as expected");
        }

        QJsonObject jsonObj = doc.object();

        QJsonObject coordObj = jsonObj["coord"].toObject();
        if (coordObj.isEmpty()) {
            throw std::runtime_error("Coordinates object is empty");
        }

        weather.setLongitude(coordObj["lon"].toDouble());
        weather.setLatitude(coordObj["lat"].toDouble());

        qDebug() << "Longitude:" << weather.getLongitude();
        qDebug() << "Latitude:" << weather.getLatitude();
    } catch (const std::exception &e) {
        qDebug() << "Error occured:" << e.what();

    }
}


void WeatherApi::parseWeatherData(const QByteArray &jsonData) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject jsonObj = doc.object();

        if (jsonObj.isEmpty()) {
            throw std::runtime_error("Invalid JSON data received");
        }

        QJsonArray weatherArray = jsonObj["weather"].toArray();
        if (!weatherArray.isEmpty()) {
            QJsonObject weatherObj = weatherArray.first().toObject();
            weather.setCurrentWeather(weatherObj["main"].toString());
            weather.setDescription(weatherObj["description"].toString());

            weather.setConditionsFromDescription(weatherObj["description"].toString());
            weather.setIcon(weatherObj["icon"].toString());
        }

        // Parsing main object for temperature and humidity
        QJsonObject mainObj = jsonObj["main"].toObject();
        weather.setTemperature(mainObj["temp"].toDouble());
        weather.setHumidity(mainObj["humidity"].toDouble());

        // Parsing wind object for wind speed
        QJsonObject windObj = jsonObj["wind"].toObject();
        weather.setwindSpeed(windObj["speed"].toDouble());
        QString locationName = jsonObj["name"].toString();
        weather.setCity(locationName);

        qDebug()<<weather.getwindSpeed() << " " << weather.getCurrentWeather() << " " << weather.getDescription() << " " << weather.getIcon()<< " " << weather.getTemperature();
        emit weatherDataUpdated();

    } else {
        qDebug() << "Invalid JSON document or format not as expected";
    }
}

void WeatherApi::makeForecastRequest() {
    currentRequestType = WeatherForecast;
    QString url = QString("https://api.openweathermap.org/data/2.5/forecast?lat=%1&lon=%2&appid=e43a65503edf85fd87e11e151341ad08&units=metric")
                      .arg(weather.getLatitude()).arg(weather.getLongitude());
    QUrl requestUrl(url);
    QNetworkRequest request(requestUrl);
    isRequestForCoordinates = false;
    manager->get(request);
}

void WeatherApi::parseForecastData(const QByteArray &jsonData) {
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject jsonObj = doc.object();
        QJsonArray listArray = jsonObj["list"].toArray();

        if (!listArray.isEmpty()) {
            QJsonObject firstListItem = listArray.first().toObject();

            QJsonArray weatherArray = firstListItem["weather"].toArray();
            if (!weatherArray.isEmpty()) {
                QJsonObject weatherObj = weatherArray.first().toObject();
                weather.setCurrentWeather(weatherObj["main"].toString());
                weather.setDescription(weatherObj["description"].toString());
                weather.setConditionsFromDescription(weatherObj["description"].toString());
                weather.setIcon(weatherObj["icon"].toString());
                int visibility = firstListItem["visibility"].toInt();
                weather.setVisibility(visibility);
            }

            QJsonObject mainObj = firstListItem["main"].toObject();
            weather.setTemperature(mainObj["temp"].toDouble());
            weather.setHumidity(mainObj["humidity"].toDouble());

            QJsonObject windObj = firstListItem["wind"].toObject();
            weather.setwindSpeed(windObj["speed"].toDouble());

            QJsonObject cityObj = jsonObj["city"].toObject();
            qint64 sunriseTimestamp = cityObj["sunrise"].toVariant().toLongLong();
            qint64 sunsetTimestamp = cityObj["sunset"].toVariant().toLongLong();


            qDebug() << "Sunrise timestamp (UTC):" << sunriseTimestamp;
            qDebug() << "Sunset timestamp (UTC):" << sunsetTimestamp;

            QDateTime sunriseTimeUTC = QDateTime::fromSecsSinceEpoch(sunriseTimestamp, Qt::UTC);
            QDateTime sunsetTimeUTC = QDateTime::fromSecsSinceEpoch(sunsetTimestamp, Qt::UTC);

            qDebug() << "Sunrise time (UTC):" << sunriseTimeUTC.toString(Qt::ISODate);
            qDebug() << "Sunset time (UTC):" << sunsetTimeUTC.toString(Qt::ISODate);

            QTimeZone timeZone(3600); // Offset in seconds (3600s = 1 hour)
            QDateTime sunriseTimeLocal = sunriseTimeUTC.toTimeZone(timeZone);
            QDateTime sunsetTimeLocal = sunsetTimeUTC.toTimeZone(timeZone);

            QString formattedSunrise = sunriseTimeLocal.toString("hh:mm:ss");
            QString formattedSunset = sunsetTimeLocal.toString("hh:mm:ss");

            qDebug() << "Formatted Sunrise time (Local):" << formattedSunrise;
            qDebug() << "Formatted Sunset time (Local):" << formattedSunset;

            weather.setSunrise(formattedSunrise);
            weather.setSunset(formattedSunset);

            QString locationName = jsonObj["city"].toObject()["name"].toString();
            weather.setCity(locationName);
            qDebug() << weather;
        }

        QVector<WeatherDataALL> forecastData;
        for (const QJsonValue &value : listArray) {
            QJsonObject obj = value.toObject();
            WeatherDataALL data;

            //izmjena
            QDateTime dateTime = QDateTime::fromSecsSinceEpoch(obj["dt"].toInt());
            data.setDateTime(dateTime);

            QJsonObject itemMainObj = obj["main"].toObject();
            data.setTemperature(itemMainObj["temp"].toDouble());
            data.setHumidity(itemMainObj["humidity"].toDouble());
            QJsonArray weatherArray = obj["weather"].toArray();
            if (!weatherArray.isEmpty()) {
                QJsonObject weatherObj = weatherArray.first().toObject();

                QString icon = weatherObj["icon"].toString();
                data.setIcon(icon);
            }

            forecastData.append(data);
        }

        for (const WeatherDataALL &value : forecastData) {
            qDebug() << value.getTemperature() << " " << value.getIcon();
        }


        emit forecastDataReady(forecastData);
        isRequestForCoordinates = true;
    } else {
        qDebug() << "Invalid JSON document or format not as expected";
    }
}

WeatherDataALL WeatherApi::getWeather(){
    return this->weather;
}
