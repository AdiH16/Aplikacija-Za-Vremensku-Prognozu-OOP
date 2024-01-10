#ifndef WEATHERAPI_H
#define WEATHERAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include "weatherdata.h"
enum RequestType{
CurrentWeather=0,
    WeatherForecast
};
class WeatherApi : public QObject
{
    Q_OBJECT

public:

    explicit WeatherApi(QObject *parent = nullptr);
    void makeRequestforLatandLong(const QString& cityName);
    void parseWeatherData(const QByteArray &jsonData);
    void makeRequestForData();
    WeatherDataALL getWeather();
private slots:
    void replyFinished(QNetworkReply *reply);
    void parseAndStoreCoordinates(const QByteArray &jsonData);
    void parseForecastData(const QByteArray &jsonData);
    void makeForecastRequest();
signals:
    void weatherDataUpdated();
    void forecastDataReady(const QVector<WeatherDataALL>& forecastData);
private:
    RequestType currentRequestType;
    QNetworkAccessManager *manager;
    WeatherDataALL weather;
    bool isRequestForCoordinates = true;
    bool isForecastRequest = true;

};

#endif // WEATHERAPI_H
