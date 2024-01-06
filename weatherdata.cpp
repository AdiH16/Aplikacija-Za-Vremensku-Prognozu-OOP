#include "weatherdata.h"

WeatherDataALL::WeatherDataALL() {}

void WeatherDataALL::setLatitude(double latitu){
    this->latitude=latitu;
};
void WeatherDataALL::setLongitude(double longi){
    this->longitude=longi;
};


double WeatherDataALL::getLatitude(){
    return this->latitude;
};

double WeatherDataALL::getLongitude(){
    return this->longitude;
};


void WeatherDataALL::setCurrentWeather(QString curweather){
    this->currentWeather=curweather;
};
void WeatherDataALL::setDescription(QString desc){
    this->description=desc;
};
void WeatherDataALL::setIcon(QString imgID){
    this->icon=imgID;
};
void WeatherDataALL:: setTemperature(int temp){
    this->temperature=temp;
};
void WeatherDataALL::setHumidity(double humid){
    this->humidity=humid;
};
void WeatherDataALL::setwindSpeed(double speed){
    this->windSpeed=speed;
};


QString WeatherDataALL::getCurrentWeather(){
    return this->currentWeather;
};
QString WeatherDataALL::getDescription(){
    return this->description;
};
QString WeatherDataALL::getIcon(){
    return this->icon;
};
int WeatherDataALL::getTemperature(){
    return this->temperature;
};
double WeatherDataALL::getHumidity(){
    return this->humidity;
};
double WeatherDataALL::getwindSpeed(){
    return this->windSpeed;
};
