#include <QtGui>
#include <QtWidgets>
#include <google/protobuf/text_format.h>
#include <Configuration.pb.h>
#include <iostream>

#include "Window.h"
#include "LinuxProcessFinder.h"
#include "WinProcessFinder.h"

namespace SaturationChanger {

Window::Window() : icon(":/saturation_changer.png") {
    createMessageGroupBox();

    createActions();
    createTrayIcon();

    connect(saveConfigurationButton, SIGNAL(clicked()), this, SLOT(saveConfiguration()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(configurationGroupBox);
    setLayout(mainLayout);

    trayIcon->show();

    setWindowTitle(tr("Saturation changer"));
    resize(400, 300);

    loadConfiguration();

#ifdef __linux__
    processFinder = new LinuxProcessFinder;
#elif WIN32
    processFinder = new WinProcessFinder;
#endif

}

void Window::setVisible(bool visible) {
    minimizeAction->setEnabled(visible);
    maximizeAction->setEnabled(!isMaximized());
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}

void Window::closeEvent(QCloseEvent* event) {
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    }
}

void Window::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        showNormal();
        break;
    case QSystemTrayIcon::MiddleClick:
        saveConfiguration();
        break;
    default:;
    }
}

void Window::saveConfiguration() {
    QFile file(CONFIGURATION_FILE);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);

        Configuration conf;

        conf.set_process_name(processNameLineEdit->text().toStdString());
        conf.set_display_id(displayIdSpinBox->value());
        conf.set_vendor(static_cast<GpuVendor>(gpuVendorComboBox->currentIndex()));

        conf.set_desktop_saturation(desktopSaturationSpinBox->value());
        conf.set_desktop_brightness(desktopBrightnessSpinBox->value());
        conf.set_desktop_contrast(desktopContrastSpinBox->value());

        conf.set_game_saturation(gameSaturationSpinBox->value());
        conf.set_game_brightness(gameBrightnessSpinBox->value());
        conf.set_game_contrast(gameContrastSpinBox->value());

        std::string config_output;
        google::protobuf::TextFormat::PrintToString(conf, &config_output);

        out << QString::fromStdString(config_output);

        fadeOutSave();
    }
}

void Window::createMessageGroupBox() {
    configurationGroupBox = new QGroupBox(tr("Configuration"));

    processNameLabel = new QLabel(tr("Process name"));
    displayIdLabel = new QLabel(tr("Display id"));
    gpuVendorLabel = new QLabel(tr("Gpu vendor"));
    desktopSaturationLabel = new QLabel(tr("Desktop saturation"));
    desktopBrightnessLabel = new QLabel(tr("Desktop brightness"));
    desktopContrastLabel = new QLabel(tr("Desktop contrast"));
    gameSaturationLabel = new QLabel(tr("Game saturation"));
    gameBrightnessLabel = new QLabel(tr("Game brightness"));
    gameContrastLabel = new QLabel(tr("Game contrast"));

    processNameLineEdit = new QLineEdit;

    displayIdSpinBox = new QSpinBox;
    displayIdSpinBox->setRange(0, 3);
    displayIdSpinBox->setValue(0);

    gpuVendorComboBox = new QComboBox;
    gpuVendorComboBox->addItem("AMD", GpuVendor::AMD);
    gpuVendorComboBox->addItem("NVIDIA", GpuVendor::NVIDIA);
    gpuVendorComboBox->addItem("INTEL", GpuVendor::INTEL);

    desktopSaturationSpinBox = new QSpinBox;
    desktopSaturationSpinBox->setRange(0, 100);
    desktopSaturationSpinBox->setValue(0);
    desktopBrightnessSpinBox = new QSpinBox;
    desktopBrightnessSpinBox->setRange(0, 100);
    desktopBrightnessSpinBox->setValue(0);
    desktopContrastSpinBox = new QSpinBox;
    desktopContrastSpinBox->setRange(0, 100);
    desktopContrastSpinBox->setValue(0);

    gameSaturationSpinBox = new QSpinBox;
    gameSaturationSpinBox->setRange(0, 100);
    gameSaturationSpinBox->setValue(0);
    gameBrightnessSpinBox = new QSpinBox;
    gameBrightnessSpinBox->setRange(0, 100);
    gameBrightnessSpinBox->setValue(0);
    gameContrastSpinBox = new QSpinBox;
    gameContrastSpinBox->setRange(0, 100);
    gameContrastSpinBox->setValue(0);

    saveConfigurationButton = new QPushButton(tr("Save"));
    saveConfigurationButton->setDefault(true);
    saveLabel = new QLabel(tr("Saved!"));
    saveLabel->setVisible(false);

    QGridLayout* configurationLayout = new QGridLayout;
    configurationLayout->addWidget(processNameLabel, 0, 0);
    configurationLayout->addWidget(processNameLineEdit, 0, 1);
    configurationLayout->addWidget(displayIdLabel, 1, 0);
    configurationLayout->addWidget(displayIdSpinBox, 1, 1);

    configurationLayout->addWidget(gpuVendorLabel, 2, 0);
    configurationLayout->addWidget(gpuVendorComboBox, 2, 1);

    configurationLayout->addWidget(desktopSaturationLabel, 3, 0);
    configurationLayout->addWidget(desktopSaturationSpinBox, 3, 1);
    configurationLayout->addWidget(desktopBrightnessLabel, 4, 0);
    configurationLayout->addWidget(desktopBrightnessSpinBox, 4, 1);
    configurationLayout->addWidget(desktopContrastLabel, 5, 0);
    configurationLayout->addWidget(desktopContrastSpinBox, 5, 1);

    configurationLayout->addWidget(gameSaturationLabel, 6, 0);
    configurationLayout->addWidget(gameSaturationSpinBox, 6, 1);
    configurationLayout->addWidget(gameBrightnessLabel, 7, 0);
    configurationLayout->addWidget(gameBrightnessSpinBox, 7, 1);
    configurationLayout->addWidget(gameContrastLabel, 8, 0);
    configurationLayout->addWidget(gameContrastSpinBox, 8, 1);

    configurationLayout->addWidget(saveConfigurationButton, 9, 0);
    configurationLayout->addWidget(saveLabel, 9, 1);

    configurationLayout->setColumnStretch(3, 1);
    configurationGroupBox->setLayout(configurationLayout);
}

void Window::createActions() {
    minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void Window::createTrayIcon() {
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    trayIcon->setToolTip(tr("Saturation changer"));
}

void Window::loadConfiguration() {
    // Create a new file
    QFile file(CONFIGURATION_FILE);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString lines = in.readAll();

    Configuration conf;
    google::protobuf::TextFormat::ParseFromString(lines.toStdString(), &conf);

    processNameLineEdit->setText(QString::fromStdString(conf.process_name()));
    displayIdSpinBox->setValue(conf.display_id());
    gpuVendorComboBox->setCurrentIndex(conf.vendor());

    desktopSaturationSpinBox->setValue(conf.desktop_saturation());
    desktopBrightnessSpinBox->setValue(conf.desktop_brightness());
    desktopContrastSpinBox->setValue(conf.desktop_contrast());

    gameSaturationSpinBox->setValue(conf.game_saturation());
    gameBrightnessSpinBox->setValue(conf.game_brightness());
    gameContrastSpinBox->setValue(conf.game_contrast());
}

void Window::fadeOutSave() {
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this);
    saveLabel->setGraphicsEffect(effect);
    saveLabel->setVisible(true);

    QPropertyAnimation* fadeoutAnimation = new QPropertyAnimation(effect, "opacity");
    fadeoutAnimation->setDuration(1000);
    fadeoutAnimation->setStartValue(1);
    fadeoutAnimation->setEndValue(0);
    fadeoutAnimation->setEasingCurve(QEasingCurve::OutBack);
    fadeoutAnimation->start(QPropertyAnimation::DeleteWhenStopped);
}

}