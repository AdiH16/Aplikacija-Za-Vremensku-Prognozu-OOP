#ifndef WEATHERDATAALL_H
#define WEATHERDATAALL_H
#include <QString>
#include "qtmetamacros.h"
class WeatherDataALL
{
public:
    WeatherDataALL();
    void setLatitude(double latitu);
    void setLongitude(double longi);
    double getLatitude();
    double getLongitude();

    void setCurrentWeather(QString curweather);
    void setDescription(QString desc);
    void setIcon(QString imgID);
    void setTemperature(int temp);
    void setHumidity(double humid);
    void setwindSpeed(double speed);

    QString getCurrentWeather();
    QString getDescription();
    QString getIcon();
    int getTemperature();
    double getHumidity();
    double getwindSpeed();

    private:
    double latitude;
    double longitude;
    QString currentWeather;
    QString description;
    QString icon;
    int temperature;
    double humidity;
    double windSpeed;





};

#endif // WEATHERDATA_H
