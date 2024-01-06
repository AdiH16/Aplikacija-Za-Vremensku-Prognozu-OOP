#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPixmap>
#include <QScrollBar>
#include <QFile>
#include "weatherapi.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    favoriteButton = new QPushButton();
    favoriteButton->setIcon(QIcon(":/images/star_hollow.png"));
    favoriteButton->setIconSize(QSize(28, 28));
    favoriteButton->setFixedSize(QSize(34, 34));

    ui->cityLabelLayout->setAlignment(favoriteButton, Qt::AlignVCenter);
    ui->cityLabelLayout->addWidget(favoriteButton);

    favoriteButton->setStyleSheet("QPushButton {"
                                  "border: none;"
                                  "background-color: transparent;"
                                  "}"
                                  "QPushButton:hover {"
                                  "background-color: rgba(255, 255, 255, 20);"
                                  "}"
                                  "QPushButton:pressed {"
                                  "background-color: rgba(255, 255, 255, 20);"
                                  "}");
    scrollStep = 150;
    connect(favoriteButton, &QPushButton::clicked, this, &MainWindow::toggleFavorite);
    connect(ui->leftScrollButton, &QToolButton::clicked, this, &MainWindow::scrollLeft);
    connect(ui->rightScrollButton, &QToolButton::clicked, this, &MainWindow::scrollRight);
    connect(ui->searchBar, &QLineEdit::returnPressed, this, &MainWindow::onSearchBarPressed);

    setupSevenDayForecastUi();
    setupHourlyForecast();
    setupWeatherDetailsUi();
    loadFavoritesIntoDropdown();

    currentCity = "Zenica";

    QStringList favorites = getFavoriteCities();
    if (favorites.contains(currentCity)) {
        favoriteButton->setIcon(QIcon(":/images/star_filled.png"));
    }

    api = new WeatherApi(this);


    CurrentWeatherData cwd;
    cwd.city = "Zenica";
    cwd.icon = QPixmap("C:/Users/ajdeJ/Downloads/Edin-Tabak-2.png");
    cwd.condition = "Sunny";
    cwd.temperature = 25.0;
    updateCurrentWeather(cwd);

}

void MainWindow::toggleFavorite() {

    bool isFavorite = favoriteButton->property("isFavorite").toBool();

    isFavorite = !isFavorite;
    favoriteButton->setProperty("isFavorite", isFavorite);

    favoriteButton->setIcon(QIcon(isFavorite ? ":/images/star_filled.png" : ":/images/star_hollow.png"));

    if (isFavorite) {
        addCityToFavorites(currentCity);
    } else {
        removeCityFromFavorites(currentCity);
    }

    loadFavoritesIntoDropdown();
}

void MainWindow::addCityToFavorites(const QString &city) {
    QStringList favorites = getFavoriteCities();
    if (!favorites.contains(city)) {
        favorites.append(city);
        writeFavoriteCities(favorites);
    }
}

void MainWindow::removeCityFromFavorites(const QString &city) {
    QStringList favorites = getFavoriteCities();
    favorites.removeAll(city);
    writeFavoriteCities(favorites);
}

void MainWindow::loadFavoritesIntoDropdown() {
    QStringList favoriteCities = getFavoriteCities();
    ui->favouriteCities->clear();
    ui->favouriteCities->addItems(favoriteCities);
}

QStringList MainWindow::getFavoriteCities() {
    QFile file("favorites.txt");
    QStringList cities;
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString line;
        while (in.readLineInto(&line)) {
            if (!line.trimmed().isEmpty()) {
                cities << line.trimmed();
            }
        }
        file.close();
    }
    return cities;
}


void MainWindow::writeFavoriteCities(const QStringList &cities) {
    QFile file("favorites.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        for (const QString &city : cities) {
            out << city << '\n';
        }
        file.close();
    }
}


void MainWindow::onSearchBarPressed() {
    QString searchText = ui->searchBar->text().trimmed();
    if (!searchText.isEmpty()) {
        api->makeRequestforLatandLong(searchText);
    }
}


void MainWindow::setupSevenDayForecastUi() {
    QGridLayout *gridLayout = new QGridLayout(ui->forecastGrid);

    for (int i = 0; i < 7; ++i) {
        QLabel *dayLabel = new QLabel("Day");
        QLabel *iconLabel = new QLabel("Icon");
        QLabel *chanceOfRainLabel = new QLabel("Chance of Rain");
        QLabel *temperatureLabel = new QLabel("Temp");

        gridLayout->addWidget(dayLabel, i, 0);
        gridLayout->addWidget(iconLabel, i, 1);
        gridLayout->addWidget(chanceOfRainLabel, i, 2);
        gridLayout->addWidget(temperatureLabel, i, 3);
    }

}

void MainWindow::setupHourlyForecast() {
    QWidget *container = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(container);

    const QSize widgetSize(150, 170);

    for (int i = 0; i < 19; ++i) {
        QWidget *hourWidget = new QWidget(container);
        hourWidget->setFixedSize(widgetSize);
        QVBoxLayout *hourLayout = new QVBoxLayout(hourWidget);

        QLabel *timeLabel = new QLabel("h", hourWidget);
        QLabel *tempLabel = new QLabel(QString::number(i), hourWidget);
        QLabel *iconLabel = new QLabel(hourWidget);

        hourLayout->addWidget(timeLabel);
        hourLayout->addWidget(iconLabel);
        hourLayout->addWidget(tempLabel);

        layout->addWidget(hourWidget);
    }

    int visibleWidgets = 6;
    ui->scrollArea->setFixedWidth(widgetSize.width() * visibleWidgets);

    ui->scrollArea->setWidget(container);
}

void MainWindow::updateCurrentWeather(const CurrentWeatherData& data) {
    ui->city->setText(data.city);
    ui->temperature->setText(QString::number(data.temperature) + "°C");

    int w = ui->weatherIcon->width();
    int h = ui->weatherIcon->height();
    ui->weatherIcon->setPixmap(data.icon.scaled(w, h, Qt::KeepAspectRatio));

    ui->condition->setText(data.condition);
}

void MainWindow::setupWeatherDetailsUi() {
    QGridLayout *gridLayout = new QGridLayout(ui->details);

    QIcon uvIcon;
    QIcon humidityIcon;
    QIcon windIcon;
    QIcon sunriseIcon;
    QIcon sunsetIcon;

    QWidget *uvWidget = createDetailWidget("UV Index", "Low", uvIcon, &uvValueLabel);
    QWidget *humidityWidget = createDetailWidget("Humidity", "81%", humidityIcon, &humidityValueLabel);
    QWidget *windWidget = createDetailWidget("Wind", "3 km/h", windIcon, &windValueLabel);

    QWidget *sunWidget = new QWidget();
    QVBoxLayout *sunLayout = new QVBoxLayout(sunWidget);
    QLabel *sunriseLabel = new QLabel();
    QLabel *sunsetLabel = new QLabel();
    sunriseLabel->setPixmap(sunriseIcon.pixmap(50, 50));
    sunsetLabel->setPixmap(sunsetIcon.pixmap(50, 50));

    sunLayout->addWidget(sunriseLabel);
    sunLayout->addWidget(sunriseValueLabel = new QLabel("07:23"));
    sunLayout->addWidget(sunsetLabel);
    sunLayout->addWidget(sunsetValueLabel = new QLabel("18:24"));
    sunWidget->setStyleSheet("background-color: #333; color: white; border-radius: 10px;");

    gridLayout->addWidget(uvWidget, 0, 0);
    gridLayout->addWidget(humidityWidget, 0, 1);
    gridLayout->addWidget(windWidget, 1, 0);
    gridLayout->addWidget(sunWidget, 1, 1);

    ui->details->setLayout(gridLayout);
}

void MainWindow::updateWeatherDetails(const WeatherData& data) {
    // Update the value labels with the data from the API
    uvValueLabel->setText(data.uvIndex);
    humidityValueLabel->setText(QString::number(data.humidity) + '%');
    windValueLabel->setText(QString::number(data.windSpeed) + " km/h");
    sunriseValueLabel->setText(data.sunrise.toString("HH:mm"));
    sunsetValueLabel->setText(data.sunset.toString("HH:mm"));
}

QWidget* MainWindow::createDetailWidget(const QString &titleText, const QString &valueText,
                                        const QIcon &icon, QLabel **valueLabelPtr) {
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);

    QLabel *iconLabel = new QLabel();
    iconLabel->setPixmap(icon.pixmap(50, 50));
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel *titleLabel = new QLabel(titleText);
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *valueLabel = new QLabel(valueText);
    valueLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(iconLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);

    widget->setStyleSheet("background-color: #333; color: white; border-radius: 10px;");
    titleLabel->setStyleSheet("font-weight: bold; color: #ccc;");
    valueLabel->setStyleSheet("font-weight: bold;");

    *valueLabelPtr = valueLabel;

    return widget;
}


/*void MainWindow::updateHourlyForecast(const QVector<WeatherData>& forecastData) {
    QWidget *container = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(container);

    const QSize widgetSize(150, 190);

    for (const auto &data : forecastData) {
        QWidget *hourWidget = new QWidget(container);
        hourWidget->setFixedSize(widgetSize);
        QVBoxLayout *hourLayout = new QVBoxLayout(hourWidget);

        QLabel *timeLabel = new QLabel(data.time.toString("h:mm a"), hourWidget);
        QLabel *tempLabel = new QLabel(QString::number(data.temperature) + "°C", hourWidget);
        QLabel *iconLabel = new QLabel(hourWidget);
        iconLabel->setPixmap(getWeatherIcon(data.condition));

        hourLayout->addWidget(timeLabel);
        hourLayout->addWidget(iconLabel);
        hourLayout->addWidget(tempLabel);

        layout->addWidget(hourWidget);
    }

    int visibleWidgets = 6;
    ui->scrollArea->setFixedWidth(widgetSize.width() * visibleWidgets + 20);

    ui->scrollArea->setWidget(container);
}

void MainWindow::updateSevenDayForecast(const QVector<WeatherData>& forecastData) {
    QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->forecastPlaceholder->layout());
    if (!gridLayout) return;

    while (QLayoutItem* item = gridLayout->takeAt(0)) {
        delete item->widget(); // Delete the widget
        delete item;           // Delete the layout item
    }

    for (int i = 0; i < forecastData.size(); ++i) {
        const WeatherData &data = forecastData[i];

        QLabel *dayLabel = new QLabel();
        QLabel *iconLabel = new QLabel(); // Set icon based on data.condition
        QLabel *chanceOfRainLabel = new QLabel(QString::number(data.chanceOfRain) + "%");
        QLabel *temperatureLabel = new QLabel(QString::number(data.temperature) + "°C");

        gridLayout->addWidget(dayLabel, i, 0);
        gridLayout->addWidget(iconLabel, i, 1);
        gridLayout->addWidget(chanceOfRainLabel, i, 2);
        gridLayout->addWidget(temperatureLabel, i, 3);
    }
}


*/

void MainWindow::scrollLeft() {
    int value = ui->scrollArea->horizontalScrollBar()->value();
    ui->scrollArea->horizontalScrollBar()->setValue(value - scrollStep);
}

void MainWindow::scrollRight() {
    int value = ui->scrollArea->horizontalScrollBar()->value();
    ui->scrollArea->horizontalScrollBar()->setValue(value + scrollStep);
}

MainWindow::~MainWindow()
{
    delete ui;
}
