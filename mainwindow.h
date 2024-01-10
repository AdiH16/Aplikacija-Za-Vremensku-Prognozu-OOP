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



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void scrollLeft();
    void scrollRight();
    void updateHourlyForecast(const QVector<WeatherDataALL>& forecastData);
    void updateSevenDayForecast(const QVector<WeatherDataALL>& forecastData);

    void updateCurrentWeather(const WeatherDataALL& data);
    void toggleFavorite();
    void onSearchBarPressed();
    void addCityToFavorites(const QString &city);
    void removeCityFromFavorites(const QString &city);
    QStringList getFavoriteCities();
    void writeFavoriteCities(const QStringList &cities);
    void loadFavoritesIntoDropdown();
    void updateWeatherUI();

    void onFavoriteCityChanged(const QString &cityName);
    //void onFavoriteCityChanged(int index);

    bool eventFilter(QObject *watched, QEvent *event) override;

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
    // void updateThreeHourForecast(const QVector<WeatherDataALL>& forecastData);

    void setupSevenDayForecastUi();
    void setupHourlyForecast();
    void setupWeatherDetailsUi();
    QWidget* createDetailWidget(const QString &titleText, const QString &valueText,
                                const QIcon &icon, QLabel **valueLabelPtr);
    void setupUiComponents();
    QString currentCity;
    std::unique_ptr<WeatherApi> api;
};
#endif // MAINWINDOW_H
