#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPixmap>
#include <QDebug>
#include <QScrollBar>
#include <QFile>
#include "weatherapi.h"
#include "weatherdata.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags disableFullscreen = Qt::WindowMaximizeButtonHint;
    setWindowFlags(flags & ~disableFullscreen);
    setWindowTitle("Vremenska Prognoza");
    setWindowIcon(QIcon(":/images/weather-app.png"));
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
    scrollStep = 156;


    setupHourlyForecast();
    setupWeatherDetailsUi();
    loadFavoritesIntoDropdown();

    ui->favouriteCities->installEventFilter(this);

    api = std::make_unique<WeatherApi>(this);
    connect(api.get(), &WeatherApi::forecastDataReady, this, &MainWindow::updateWeatherUI);
    connect(favoriteButton, &QPushButton::clicked, this, &MainWindow::toggleFavorite);
    connect(ui->leftScrollButton, &QToolButton::clicked, this, &MainWindow::scrollLeft);
    connect(ui->rightScrollButton, &QToolButton::clicked, this, &MainWindow::scrollRight);
    connect(ui->searchBar, &QLineEdit::returnPressed, this, &MainWindow::onSearchBarPressed);
    connect(api.get(), &WeatherApi::forecastDataReady, this, &MainWindow::updateHourlyForecast);
    connect(api.get(), &WeatherApi::forecastDataReady, this, &MainWindow::updateSevenDayForecast);



    connect(ui->favouriteCities, &QComboBox::currentTextChanged, this, &MainWindow::onFavoriteCityChanged);



    currentCity = "Zenica";
    QStringList favorites = getFavoriteCities();
    if (favorites.contains(currentCity)) {
        favoriteButton->setIcon(QIcon(":/images/star_filled.png"));
    }
    api->makeRequestforLatandLong(currentCity);


}




void MainWindow::updateWeatherUI() {
    WeatherDataALL cwd = api->getWeather();
    updateCurrentWeather(cwd);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->favouriteCities && event->type() == QEvent::MouseButtonRelease) {
        onFavoriteCityChanged(ui->favouriteCities->currentText());
        return true; // Event je obrađen
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::updateCurrentWeather(const WeatherDataALL& data) {
    qDebug() << data.getCity();
    currentCity = data.getCity();
    ui->city->setText(data.getCity());
    ui->temperature->setText(QString::number(data.getTemperature()) + "°C");
    ui->condition->setText(data.getDescription().toUpper());
    ui->currentWeatherCondition->setText(data.getConditionAsString().toUpper());
    ui->currentWeatherCondition->setAlignment(Qt::AlignCenter);
    humidityValueLabel->setText(QString::number(data.getHumidity()) + '%');
    windValueLabel->setText(QString::number(data.getwindSpeed()) + " km/h");
    sunriseValueLabel->setText(data.getSunrise());
    sunsetValueLabel->setText(data.getSunset());
    uvValueLabel->setText(QString::number(data.getVisibility()) + " m");

    QStringList favorites = getFavoriteCities();
    if (favorites.contains(currentCity)) {
        favoriteButton->setIcon(QIcon(":/images/star_filled.png"));
    } else {
        favoriteButton->setIcon(QIcon(":/images/star_hollow.png"));
    }
    QString iconPath = ":/images/" + data.getIcon() + ".png"; // Pretpostavljamo da su ikone u resources fajlu
    QPixmap icon(iconPath);
    if(!icon.isNull()) {
        ui->weatherIcon->setPixmap(icon.scaled(ui->weatherIcon->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        qDebug() << "Ikonica nije nađena za: " << data.getIcon();
    }
    ui->weatherIcon->setAlignment(Qt::AlignCenter);
    ui->favouriteCities->setCurrentIndex(-1);
}


void MainWindow::onFavoriteCityChanged(const QString &cityName) {
    if (!cityName.isEmpty() && cityName != currentCity) {
        currentCity = cityName;
        api->makeRequestforLatandLong(cityName);
    }
}


void MainWindow::toggleFavorite() {

    QStringList favorites = getFavoriteCities();
    bool isFavorite = favorites.contains(currentCity);

    if (isFavorite) {
        removeCityFromFavorites(currentCity);
        favoriteButton->setIcon(QIcon(":/images/star_hollow.png"));
    } else {
        addCityToFavorites(currentCity);
        favoriteButton->setIcon(QIcon(":/images/star_filled.png"));
    }

    disconnect(ui->favouriteCities, &QComboBox::currentTextChanged, this, &MainWindow::onFavoriteCityChanged);

    loadFavoritesIntoDropdown();


    int newIndex = ui->favouriteCities->findText(currentCity);
    if (newIndex != -1) {
        ui->favouriteCities->setCurrentIndex(newIndex);
    } else {
        qDebug() << "Failed to find city in dropdown after update: " << currentCity;
    }


    connect(ui->favouriteCities, &QComboBox::currentTextChanged, this, &MainWindow::onFavoriteCityChanged);


    qDebug() << "Current index after update:" << ui->favouriteCities->currentIndex();
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

/*void MainWindow::loadFavoritesIntoDropdown() {
    QStringList favoriteCities = getFavoriteCities();
    ui->favouriteCities->clear();
    ui->favouriteCities->addItems(favoriteCities);

    int index = favoriteCities.indexOf(currentCity);
    if (index != -1) {
        ui->favouriteCities->setCurrentIndex(index);
    }
}*/
void MainWindow::loadFavoritesIntoDropdown() {
    QStringList favoriteCities = getFavoriteCities();
    QString currentSelection = ui->favouriteCities->currentText();

    ui->favouriteCities->clear();
    ui->favouriteCities->addItems(favoriteCities);

    // Restore the previous selection if it's still valid
    int index = ui->favouriteCities->findText(currentSelection);
    if (index != -1) {
        ui->favouriteCities->setCurrentIndex(index);
    } else if (!currentCity.isEmpty() && favoriteCities.contains(currentCity)) {
        // If the current city is in the list, select it
        ui->favouriteCities->setCurrentIndex(ui->favouriteCities->findText(currentCity));
    }
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
        QStringList favorites = getFavoriteCities();
        if (favorites.contains(searchText)) {
            favoriteButton->setIcon(QIcon(":/images/star_filled.png"));
        } else {
            favoriteButton->setIcon(QIcon(":/images/star_hollow.png"));
        }
    }
}


void MainWindow::setupSevenDayForecastUi() {
    QGridLayout *gridLayout = new QGridLayout(ui->forecastGrid);

    for (int i = 0; i < 5; ++i) {
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

void MainWindow::updateSevenDayForecast(const QVector<WeatherDataALL>& forecastData) {
    QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->forecastGrid->layout());
    //ui->forecastGrid->setStyleSheet("border:1px solid white;");
    if (!gridLayout) {
        gridLayout = new QGridLayout(ui->forecastGrid);
        ui->forecastGrid->setLayout(gridLayout);
    }

    while (QLayoutItem* item = gridLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    for (int i = 0; i < 5; ++i) {
        const WeatherDataALL &data = forecastData[i*8]; // Pretpostavljam da ovako izdvajaš podatke za određeni dan
        QDate date = QDateTime::currentDateTime().date().addDays(i);
        int dayOfWeek = date.dayOfWeek();
        QString dayName = QLocale::system().dayName(dayOfWeek, QLocale::LongFormat);
        dayName[0] = dayName[0].toUpper();
        QLabel *dayLabel = new QLabel(dayName);
        dayLabel->setAlignment(Qt::AlignCenter);
        QString textStyle = "color: white; font-weight: bold";
        dayLabel->setStyleSheet(textStyle);


        QString iconPath = ":/images/" + data.getIcon() + ".png";
        QPixmap iconPixmap(iconPath);
        QLabel *iconLabel = new QLabel();
        if (!iconPixmap.isNull()) {
            iconLabel->setPixmap(iconPixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            iconLabel->setText("No icon");
        }
        iconLabel->setAlignment(Qt::AlignCenter);

        QLabel *chanceOfRainLabel = new QLabel(QString::number(data.getHumidity()) + "%");
        chanceOfRainLabel->setAlignment(Qt::AlignCenter);
        chanceOfRainLabel->setStyleSheet(textStyle);

        QLabel *temperatureLabel = new QLabel(QString::number(data.getTemperature()) + "°C");
        temperatureLabel->setAlignment(Qt::AlignCenter);
        temperatureLabel->setStyleSheet(textStyle);

        gridLayout->addWidget(dayLabel, i, 0);
        gridLayout->addWidget(iconLabel, i, 1);
        gridLayout->addWidget(chanceOfRainLabel, i, 2);
        gridLayout->addWidget(temperatureLabel, i, 3);
    }

    ui->forecastGrid->update();
    ui->forecastGrid->repaint();
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


void MainWindow::setupWeatherDetailsUi() {
    QGridLayout *gridLayout = new QGridLayout(ui->details);

    QIcon uvIcon = QIcon(":/images/802067.png");
    QIcon humidityIcon = QIcon(":/images/4148460.png");
    QIcon windIcon = QIcon(":/images/172922.png");
    QIcon sunriseIcon = QIcon(":/images/sunrise.png");
    QIcon sunsetIcon = QIcon(":/images/sunsets.png");

    QWidget *uvWidget = createDetailWidget("Visibilty", "0", uvIcon, &uvValueLabel);
    QWidget *humidityWidget = createDetailWidget("Humidity", "81%", humidityIcon, &humidityValueLabel);
    QWidget *windWidget = createDetailWidget("Wind", "3 km/h", windIcon, &windValueLabel);

    QWidget *sunWidget = new QWidget();
    QGridLayout *sunLayout = new QGridLayout();


    sunLayout->setColumnMinimumWidth(0, 100);
    sunLayout->setColumnMinimumWidth(1, 100);

    QLabel *sunriseLabel = new QLabel();
    QLabel *sunsetLabel = new QLabel();
    sunriseLabel->setPixmap(sunriseIcon.pixmap(50, 50));
    sunsetLabel->setPixmap(sunsetIcon.pixmap(50, 50));
    sunriseLabel->setAlignment(Qt::AlignCenter);
    sunsetLabel->setAlignment(Qt::AlignCenter);

    QLabel *sunriseLabelText = new QLabel("Sunrise");
    QLabel *sunsetLabelText = new QLabel("Sunset");


    QString textStyle = "font-weight: bold; color: #ccc; text-align: center;";
    sunriseLabelText->setStyleSheet(textStyle);
    sunsetLabelText->setStyleSheet(textStyle);
    sunriseLabelText->setAlignment(Qt::AlignCenter);
    sunsetLabelText->setAlignment(Qt::AlignCenter);


    sunriseValueLabel = new QLabel("07:23");
    sunsetValueLabel = new QLabel("18:24");

    QString valueLabelStyle = "color: white; text-align: center; font-weight: bold;";
    sunriseValueLabel->setStyleSheet(valueLabelStyle);
    sunsetValueLabel->setStyleSheet(valueLabelStyle);
    sunriseValueLabel->setAlignment(Qt::AlignCenter);
    sunsetValueLabel->setAlignment(Qt::AlignCenter);


    sunLayout->addWidget(sunriseLabel, 0, 0);
    sunLayout->addWidget(sunriseLabelText, 1, 0);
    sunLayout->addWidget(sunriseValueLabel, 2, 0);
    sunLayout->addWidget(sunsetLabel, 0, 1);
    sunLayout->addWidget(sunsetLabelText, 1, 1);
    sunLayout->addWidget(sunsetValueLabel, 2, 1);

    sunWidget->setLayout(sunLayout);


    sunWidget->setStyleSheet("background-color: rgb(16, 116, 164); color: white; border-radius: 10px;");


    sunLayout->setSpacing(5);
    sunLayout->setContentsMargins(10, 10, 10, 10);

    gridLayout->addWidget(uvWidget, 0, 0);
    gridLayout->addWidget(humidityWidget, 0, 1);
    gridLayout->addWidget(windWidget, 1, 0);
    gridLayout->addWidget(sunWidget, 1, 1);

    ui->details->setLayout(gridLayout);
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

    widget->setStyleSheet("background-color: rgb(16, 116, 164); color: white; border-radius: 10px;");
    titleLabel->setStyleSheet("font-weight: bold; color: #ccc;");
    valueLabel->setStyleSheet("font-weight: bold;");

    *valueLabelPtr = valueLabel;

    return widget;
}


void MainWindow::updateHourlyForecast(const QVector<WeatherDataALL>& forecastData) {
    QWidget *container = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(container);

    const QSize widgetSize(150, 170);
    int brojac = 0;
    QString textStyle = "color: white; font-weight: bold; border: none;";

    for (const auto &data : forecastData) {
        if(brojac == 24) // Ograničavamo na 24 sata
            break;
        QWidget *hourWidget = new QWidget(container);
        hourWidget->setFixedSize(widgetSize);
        hourWidget->setStyleSheet("border-right: 1px solid gray; ");
        QVBoxLayout *hourLayout = new QVBoxLayout(hourWidget);


        QLabel *timeLabel = new QLabel(data.getDateTime().toString("HH:mm"), hourWidget);
        timeLabel->setAlignment(Qt::AlignCenter);
        timeLabel->setStyleSheet(textStyle);
        QLabel *tempLabel = new QLabel(QString::number(data.getTemperature()) + "°C", hourWidget);
        tempLabel->setAlignment(Qt::AlignCenter);
        tempLabel->setStyleSheet(textStyle);
        //Dodavanje ikone
        QLabel *iconLabel = new QLabel(hourWidget);
        QString iconPath = ":/images/" + data.getIcon() + ".png"; // Pretpostavljamo da su ikone u resources fajlu
        QPixmap icon(iconPath);
        if(!icon.isNull()) {
            iconLabel->setPixmap(icon.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            qDebug() << "Ikonica nije nađena za: " << data.getIcon();
        }
        iconLabel->setAlignment(Qt::AlignCenter);
        iconLabel->setStyleSheet(textStyle);
        hourLayout->addWidget(timeLabel);
        hourLayout->addSpacing(10);
        hourLayout->addWidget(iconLabel);
        hourLayout->addSpacing(10);
        hourLayout->addWidget(tempLabel);

        layout->addWidget(hourWidget);
        brojac++;
    }

    int visibleWidgets = 6;
    ui->scrollArea->setFixedWidth(widgetSize.width() * visibleWidgets);
    ui->scrollArea->setWidget(container);
}
//izmjena



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
