#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QStringList>
#include <QString>
#include "weatherapi.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct CurrentWeatherData {
    QString city;
    double temperature;
    QString condition;
    QPixmap icon;
};

struct WeatherData {
    QString uvIndex;
    int humidity;
    int windSpeed;
    QTime sunrise;
    QTime sunset;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void scrollLeft();
    void scrollRight();
    //void updateHourlyForecast(const QVector<WeatherData>& forecastData);
    //void updateSevenDayForecast(const QVector<WeatherData>& forecastData);

    void updateWeatherDetails(const WeatherData& data);
    void updateCurrentWeather(const CurrentWeatherData& data);
    void toggleFavorite();
    void onSearchBarPressed();
    void addCityToFavorites(const QString &city);
    void removeCityFromFavorites(const QString &city);
    QStringList getFavoriteCities();
    void writeFavoriteCities(const QStringList &cities);
    void loadFavoritesIntoDropdown();
private:
    Ui::MainWindow *ui;
    int scrollStep;
    QLabel *uvValueLabel;
    QLabel *humidityValueLabel;
    QLabel *windValueLabel;
    QLabel *sunriseValueLabel;
    QLabel *sunsetValueLabel;
    QPushButton *favoriteButton;
    QLineEdit *searchBar;
    QComboBox *favoritesDropdown;
    QGridLayout *gridLayout;

    void setupSevenDayForecastUi();
    void setupHourlyForecast();
    void setupWeatherDetailsUi();
    QWidget* createDetailWidget(const QString &titleText, const QString &valueText,
                                const QIcon &icon, QLabel **valueLabelPtr);
    void setupUiComponents();
    QString currentCity;
    WeatherApi *api;
};
#endif // MAINWINDOW_H
