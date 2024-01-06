#ifndef WEATHERAPI_H
#define WEATHERAPI_H

#include <QObject>
#include <QNetworkAccessManager>

class WeatherApi : public QObject
{
    Q_OBJECT

public:
    explicit WeatherApi(QObject *parent = nullptr);
    void makeRequest(const QString& cityName);

private slots:
    void replyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
};

#endif // WEATHERAPI_H
