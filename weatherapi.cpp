#include "WeatherApi.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>

WeatherApi::WeatherApi(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &WeatherApi::replyFinished);
}

void WeatherApi::makeRequest(const QString& cityName)
{
    QUrl url(QString("http://api.openweathermap.org/data/2.5/weather?q=%1&appid=e43a65503edf85fd87e11e151341ad08").arg(cityName));
    QNetworkRequest request(url);

    manager->get(request);
}

void WeatherApi::replyFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << "ERROR!";
        qDebug() << reply->errorString();
    } else {
        qDebug() << reply->readAll();
    }
    reply->deleteLater();
}
