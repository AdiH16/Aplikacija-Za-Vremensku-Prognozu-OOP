#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPixmap>
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

    //setupSevenDayForecastUi();
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

    //connect(ui->favouriteCities, &QComboBox::currentTextChanged, this, &MainWindow::onFavoriteCityChanged);

    connect(ui->favouriteCities, &QComboBox::currentTextChanged, this, &MainWindow::onFavoriteCityChanged);
   // connect(ui->favouriteCities, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFavoriteCityChanged);


    currentCity = "Zenica";
    QStringList favorites = getFavoriteCities();
    if (favorites.contains(currentCity)) {
        favoriteButton->setIcon(QIcon(":/images/star_filled.png"));
    }
    api->makeRequestforLatandLong(currentCity);


}

// void MainWindow::updateThreeHourForecast(const QVector<WeatherDataALL>& forecastData) {
//     // Pretpostavimo da postoji container widget sa layout-om za tročasovne intervale
//     QWidget *container = new QWidget();
//     QHBoxLayout *layout = new QHBoxLayout(container);

//     for (const auto &data : forecastData) {
//         QWidget *forecastWidget = new QWidget(container);
//         QVBoxLayout *forecastLayout = new QVBoxLayout(forecastWidget);

//         QLabel *timeLabel = new QLabel(data.getTime().toString("hh:mm a"), forecastWidget);
//         QLabel *tempLabel = new QLabel(QString::number(data.getTemperature()) + "°C", forecastWidget);
//         QLabel *iconLabel = new QLabel(forecastWidget);
//         QPixmap iconPixmap(":/images/" + data.getIcon() + ".png");
//         iconLabel->setPixmap(iconPixmap.scaled(50, 50, Qt::KeepAspectRatio));

//         forecastLayout->addWidget(timeLabel);
//         forecastLayout->addWidget(iconLabel);
//         forecastLayout->addWidget(tempLabel);

//         layout->addWidget(forecastWidget);
//     }

//     ui->scrollArea->setWidget(container);
// }


void MainWindow::updateWeatherUI() {
    WeatherDataALL cwd = api->getWeather();
    updateCurrentWeather(cwd);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->favouriteCities && event->type() == QEvent::MouseButtonRelease) {
        onFavoriteCityChanged(ui->favouriteCities->currentText());
        return true; // Event je obrađen
    }
    return QMainWindow::eventFilter(watched, event); // Neka se ostali eventi obrađuju normalno
}
//izmjena
void MainWindow::updateCurrentWeather(const WeatherDataALL& data) {
    qDebug() << data.getCity();
    currentCity =  data.getCity();
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
    //int w = ui->weatherIcon->width();
    //int h = ui->weatherIcon->height();
    //ui->weatherIcon->setPixmap(data.getIcon().scaled(w, h, Qt::KeepAspectRatio));
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

// void MainWindow::onFavoriteCityChanged(const QString &cityName) {
//     if(!cityName.isEmpty())
//     {
//         api->makeRequestforLatandLong(cityName);
//     }
// }
void MainWindow::onFavoriteCityChanged(const QString &cityName) {
    if (!cityName.isEmpty() && cityName != currentCity) {
        currentCity = cityName;
        api->makeRequestforLatandLong(cityName);
    }
}
// void MainWindow::onFavoriteCityChanged(int index) {
//     // Ova funkcija sada uzima indeks, što znači da će se uvek pozvati kada korisnik izabere stavku iz padajuće liste
//     QString cityName = ui->favouriteCities->itemText(index);
//     if (!cityName.isEmpty()) {
//         api->makeRequestforLatandLong(cityName);
//         currentCity = cityName; // Ažuriramo trenutni grad
//     }
// }

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

// void MainWindow::updateSevenDayForecast(const QVector<WeatherDataALL>& forecastData) {
//     QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->forecastGrid->layout());
//     if (!gridLayout) {
//         gridLayout = new QGridLayout(ui->forecastGrid);
//         ui->forecastGrid->setLayout(gridLayout);
//     }


//     while (QLayoutItem* item = gridLayout->takeAt(0)) {
//         delete item->widget();
//         delete item;
//     }

//     QString labelStyle = "QLabel { background-color : transparent; color : black; }";

//     for (int i = 0; i < forecastData.size(); ++i) {
//         const WeatherDataALL &data = forecastData[i];

//         QLabel *dayLabel = new QLabel(data.getDateTime().toString("dddd"));
//         dayLabel->setAlignment(Qt::AlignCenter);
//         dayLabel->setStyleSheet(labelStyle);

//         QLabel *iconLabel = new QLabel();
//         QString iconPath = ":/images/" + data.getIcon() + ".png";
//         QPixmap icon(iconPath);
//         if(!icon.isNull()) {
//             iconLabel->setPixmap(icon.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
//         }
//         iconLabel->setAlignment(Qt::AlignCenter);
//         iconLabel->setStyleSheet(labelStyle);

//         QLabel *chanceOfRainLabel = new QLabel(QString::number(data.getHumidity()) + "%");
//         chanceOfRainLabel->setAlignment(Qt::AlignCenter);
//         chanceOfRainLabel->setStyleSheet(labelStyle);

//         QLabel *temperatureLabel = new QLabel(QString::number(data.getTemperature()) + "°C");
//         temperatureLabel->setAlignment(Qt::AlignCenter);
//         temperatureLabel->setStyleSheet(labelStyle);

//         gridLayout->addWidget(dayLabel, i, 0);
//         gridLayout->addWidget(iconLabel, i, 1);
//         gridLayout->addWidget(chanceOfRainLabel, i, 2);
//         gridLayout->addWidget(temperatureLabel, i, 3);
//     }


//     ui->forecastGrid->update();
//     ui->forecastGrid->repaint();
// }

//izmjena

// void MainWindow::updateSevenDayForecast(const QVector<WeatherDataALL>& forecastData) {
//     QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->forecastGrid->layout());
//     if (!gridLayout) {
//         gridLayout = new QGridLayout(ui->forecastGrid);
//         ui->forecastGrid->setLayout(gridLayout);
//     }

//     while (QLayoutItem* item = gridLayout->takeAt(0)) {
//         delete item->widget();
//         delete item;
//     }

//     for (int i = 0; i < 5; ++i) {
//         const WeatherDataALL &data = forecastData[i*8];
//         QDate date = QDateTime::currentDateTime().date().addDays(i);
//         int dayOfWeek = date.dayOfWeek();

//         QString dayName = QLocale::system().dayName(dayOfWeek, QLocale::LongFormat);

//         QLabel *dayLabel = new QLabel(dayName);

//         QPixmap iconPixmap(data.getIcon());
//         QLabel *iconLabel = new QLabel();
//         iconLabel->setPixmap(iconPixmap);

//         QLabel *chanceOfRainLabel = new QLabel(QString::number(data.getHumidity()) + "%");
//         QLabel *temperatureLabel = new QLabel(QString::number(data.getTemperature()) + "°C");

//         gridLayout->addWidget(dayLabel, i, 0);
//         gridLayout->addWidget(iconLabel, i, 1);
//         gridLayout->addWidget(chanceOfRainLabel, i, 2);
//         gridLayout->addWidget(temperatureLabel, i, 3);
//     }

//     ui->forecastGrid->update();
//     ui->forecastGrid->repaint();
// }
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

        // Provjeravamo da li je putanja ikone validna
        QString iconPath = ":/images/" + data.getIcon() + ".png";
        QPixmap iconPixmap(iconPath);
        QLabel *iconLabel = new QLabel();
        if (!iconPixmap.isNull()) {
            iconLabel->setPixmap(iconPixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            iconLabel->setText("No icon"); // Ako ikona nije pronađena, postavljamo placeholder tekst
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

    // Set the grid layout to have fixed column sizes to ensure centering
    sunLayout->setColumnMinimumWidth(0, 100); // Adjust the minimum width as needed
    sunLayout->setColumnMinimumWidth(1, 100); // Adjust the minimum width as needed

    QLabel *sunriseLabel = new QLabel();
    QLabel *sunsetLabel = new QLabel();
    sunriseLabel->setPixmap(sunriseIcon.pixmap(50, 50));
    sunsetLabel->setPixmap(sunsetIcon.pixmap(50, 50));
    sunriseLabel->setAlignment(Qt::AlignCenter);
    sunsetLabel->setAlignment(Qt::AlignCenter);

    QLabel *sunriseLabelText = new QLabel("Sunrise");
    QLabel *sunsetLabelText = new QLabel("Sunset");

    // Set the labels to use the same alignment and styling
    QString textStyle = "font-weight: bold; color: #ccc; text-align: center;";
    sunriseLabelText->setStyleSheet(textStyle);
    sunsetLabelText->setStyleSheet(textStyle);
    sunriseLabelText->setAlignment(Qt::AlignCenter);
    sunsetLabelText->setAlignment(Qt::AlignCenter);

    // Initialize the labels for sunrise and sunset times
    sunriseValueLabel = new QLabel("07:23");
    sunsetValueLabel = new QLabel("18:24");

    QString valueLabelStyle = "color: white; text-align: center; font-weight: bold;";
    sunriseValueLabel->setStyleSheet(valueLabelStyle);
    sunsetValueLabel->setStyleSheet(valueLabelStyle);
    sunriseValueLabel->setAlignment(Qt::AlignCenter);
    sunsetValueLabel->setAlignment(Qt::AlignCenter);

    // Add widgets to the layout with proper alignment
    sunLayout->addWidget(sunriseLabel, 0, 0);
    sunLayout->addWidget(sunriseLabelText, 1, 0);
    sunLayout->addWidget(sunriseValueLabel, 2, 0);
    sunLayout->addWidget(sunsetLabel, 0, 1);
    sunLayout->addWidget(sunsetLabelText, 1, 1);
    sunLayout->addWidget(sunsetValueLabel, 2, 1);

    sunWidget->setLayout(sunLayout); // Assign the layout to sunWidget

    // Styling for sunWidget to match the desired appearance
    sunWidget->setStyleSheet("background-color: rgb(16, 116, 164); color: white; border-radius: 10px;");

    // Set uniform spacing and margins for the layout
    sunLayout->setSpacing(5); // Adjust spacing as needed
    sunLayout->setContentsMargins(10, 10, 10, 10); // Adjust margins as needed

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

    const QSize widgetSize(150, 170); // Prilagodi ovo prema potrebama
    int brojac = 0;
    QString textStyle = "color: white; font-weight: bold; border: none;";

    for (const auto &data : forecastData) {
        if(brojac == 24) // Ograničavamo na 24 sata
            break;
        QWidget *hourWidget = new QWidget(container);
        hourWidget->setFixedSize(widgetSize);
        hourWidget->setStyleSheet("border-right: 1px solid gray; ");
        QVBoxLayout *hourLayout = new QVBoxLayout(hourWidget);

        //int hoursToAdd = (brojac * 3)+1; // Increment by 3 hours each iteration
        //QDateTime incrementedTime = QDateTime::currentDateTime().addSecs(hoursToAdd * 3600); // Convert hours to seconds
        //QLabel *timeLabel = new QLabel(incrementedTime.toString("h:mm a"), hourWidget);
        //izmjena
        QLabel *timeLabel = new QLabel(data.getDateTime().toString("HH:mm"), hourWidget);
        timeLabel->setAlignment(Qt::AlignCenter);
        timeLabel->setStyleSheet(textStyle);
        QLabel *tempLabel = new QLabel(QString::number(data.getTemperature()) + "°C", hourWidget);
        tempLabel->setAlignment(Qt::AlignCenter);
        tempLabel->setStyleSheet(textStyle);
        // Ovdje dodajemo ikonu
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
        hourLayout->addSpacing(10);  // Add spacing between timeLabel and iconLabel
        hourLayout->addWidget(iconLabel);
        hourLayout->addSpacing(10);  // Add spacing between iconLabel and tempLabel
        hourLayout->addWidget(tempLabel);

        layout->addWidget(hourWidget);
        brojac++;
    }

    int visibleWidgets = 6; // Koliko widgeta želimo vidjeti odjednom
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
