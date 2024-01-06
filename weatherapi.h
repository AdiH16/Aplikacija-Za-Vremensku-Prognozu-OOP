#ifndef WEATHERAPI_H
#define WEATHERAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include "weatherdata.h"
class WeatherApi : public QObject
{
    Q_OBJECT

public:
    explicit WeatherApi(QObject *parent = nullptr);
    void makeRequestforLatandLong(const QString& cityName);
    void parseWeatherData(const QByteArray &jsonData);
    void makeRequestForData();

private slots:
    void replyFinished(QNetworkReply *reply);
    void parseAndStoreCoordinates(const QByteArray &jsonData);

private:
    QNetworkAccessManager *manager;
    WeatherDataALL weather;
    bool isRequestForCoordinates = true;
};

#endif // WEATHERAPI_H
