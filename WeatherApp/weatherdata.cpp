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
void WeatherDataALL::setCity(QString city){
    this->city=city;
};
void WeatherDataALL::setSunrise(QString sunrise){
    this->sunrise=sunrise;
};
void WeatherDataALL::setSunset(QString sunset){
    this->sunset=sunset;
};
void WeatherDataALL::setVisibility(int visibility){
    this->visibility=visibility;
};

QString WeatherDataALL::getCurrentWeather() const{
    return this->currentWeather;
};
QString WeatherDataALL::getDescription() const{
    return this->description;
};
QString WeatherDataALL::getIcon() const{
    return this->icon;
};
int WeatherDataALL::getTemperature() const{
    return this->temperature;
};
double WeatherDataALL::getHumidity() const{
    return this->humidity;
};
double WeatherDataALL::getwindSpeed() const{
    return this->windSpeed;
};
QString WeatherDataALL::getCity() const{
    return this->city;
};
QString WeatherDataALL::getSunrise() const{
    return this->sunrise;
};
QString WeatherDataALL::getSunset() const{
    return this->sunset;
};
int WeatherDataALL::getVisibility() const{
    return this->visibility;
};