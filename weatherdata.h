#ifndef WEATHERDATAALL_H
#define WEATHERDATAALL_H
#include <QString>
#include "qtmetamacros.h"
#include <QDebug>
#include <QDateTime>
class WeatherDataALL
{
public:
    WeatherDataALL();
    void setLatitude(double latitu);
    void setLongitude(double longi);
    double getLatitude();
    double getLongitude();

    enum WeatherCondition{
        Sunny,
        Cloudy,
        Rainy,
        Snowy,
        Clear,
        Mist,
        Unknown
    };
    void setCurrentWeather(QString curweather);
    void setDescription(QString desc);
    void setIcon(QString imgID);
    void setTemperature(int temp);
    void setHumidity(double humid);
    void setwindSpeed(double speed);
    void setCity(QString city);
    void setSunrise(QString sunrise);
    void setSunset(QString sunset);
    void setVisibility(int visibility);

    WeatherCondition getWeatherCondition();
    void setConditionsFromDescription(const QString &description);
    virtual QString getConditionAsString() const;
    friend QDebug operator<<(QDebug dbg, const WeatherDataALL &data);

    QString getCurrentWeather() const;
    QString getDescription() const;
    QString getIcon() const;
    int getTemperature() const;
    double getHumidity() const;
    double getwindSpeed() const;
    QString getCity() const;
    QString getSunrise() const;
    QString getSunset() const;
    int getVisibility() const;

    void setDateTime(const QDateTime& newDateTime);
    QDateTime getDateTime() const;
private:
    WeatherCondition currentCondition=Unknown;

    double latitude;
    double longitude;
    QString currentWeather;
    QString description;
    QString icon;
    int temperature;
    double humidity;
    double windSpeed;
    QString city;
    QString sunrise;
    QString sunset;
    int visibility;

    QDateTime dateTime;
};

#endif // WEATHERDATA_H
