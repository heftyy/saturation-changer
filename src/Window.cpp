#include <QtGui>
#include <QtWidgets>
#include <google/protobuf/text_format.h>
#include <Configuration.pb.h>
#include <iostream>

#include "Window.h"
#include "LinuxProcessFinder.h"
#include "WinProcessFinder.h"

namespace SaturationChanger {

Window::Window() : icon(":/saturation_changer.png"), configLoaded(false) {
    createMessageGroupBox();    

    createActions();
    createTrayIcon();

    connect(saveConfigurationButton, SIGNAL(clicked()), this, SLOT(saveConfiguration()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));        

    connect(gpuVendorComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(vendorChanged(int)));

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(configurationGroupBox);
    setLayout(mainLayout);

    trayIcon->show();

    setWindowTitle(tr("Saturation changer"));
    resize(400, 300);

    loadConfiguration();
    monitor = std::make_unique<ProcessMonitor>(&saturationConfig);
    monitor->init();
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

void Window::vendorChanged(int index) const {
    if (configLoaded) {
        QMessageBox::information(nullptr,
                                 tr("Vendor changed"),
                                 tr("Please save the settings and restart the application\n"
                                    "for the changes to take effect"));
    }    

    if (index == AMD) {
        //default saturation
        desktopSaturationSpinBox->setValue(100);
        desktopSaturationSpinBox->setMaximum(200);

        gameSaturationSpinBox->setValue(175);
        gameSaturationSpinBox->setMaximum(200);

        //default brightness
        desktopBrightnessSpinBox->setValue(0);
        desktopBrightnessSpinBox->setMaximum(100);

        gameBrightnessSpinBox->setValue(0);
        gameBrightnessSpinBox->setMaximum(100);

        //default contrast
        desktopContrastSpinBox->setValue(100);
        desktopContrastSpinBox->setMaximum(200);

        gameContrastSpinBox->setValue(100);
        gameContrastSpinBox->setMaximum(200);

        //enable AMD only options
        desktopBrightnessSpinBox->setEnabled(true);
        desktopContrastSpinBox->setEnabled(true);

        gameBrightnessSpinBox->setEnabled(true);
        gameContrastSpinBox->setEnabled(true);
    }
    else {
        //default saturation
        desktopSaturationSpinBox->setValue(0);
        desktopSaturationSpinBox->setMaximum(63);

        gameSaturationSpinBox->setValue(50);
        gameSaturationSpinBox->setMaximum(63);

        //disable AMD only options
        desktopBrightnessSpinBox->setEnabled(false);
        desktopContrastSpinBox->setEnabled(false);

        gameBrightnessSpinBox->setEnabled(false);
        gameContrastSpinBox->setEnabled(false);
    }
}

void Window::saveConfiguration() {
    QFile file(CONFIGURATION_FILE);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);

		saturationConfig.set_process_name(processNameLineEdit->text().toStdString());
		saturationConfig.set_display_id(displayIdSpinBox->value());
		saturationConfig.set_vendor(static_cast<GpuVendor>(gpuVendorComboBox->currentIndex()));

		saturationConfig.set_desktop_saturation(desktopSaturationSpinBox->value());
		saturationConfig.set_desktop_brightness(desktopBrightnessSpinBox->value());
		saturationConfig.set_desktop_contrast(desktopContrastSpinBox->value());

		saturationConfig.set_game_saturation(gameSaturationSpinBox->value());
		saturationConfig.set_game_brightness(gameBrightnessSpinBox->value());
		saturationConfig.set_game_contrast(gameContrastSpinBox->value());

        std::string config_output;
        google::protobuf::TextFormat::PrintToString(saturationConfig, &config_output);

        out << QString::fromStdString(config_output);

        fadeOutSave();
    }
}

void Window::createMessageGroupBox() {
    configurationGroupBox = new QGroupBox(tr("Configuration"), this);

    processNameLabel = new QLabel(tr("Process name"), configurationGroupBox);
    displayIdLabel = new QLabel(tr("Display id"), configurationGroupBox);
    gpuVendorLabel = new QLabel(tr("Gpu vendor"), configurationGroupBox);
    desktopSaturationLabel = new QLabel(tr("Desktop saturation"), configurationGroupBox);
    desktopBrightnessLabel = new QLabel(tr("Desktop brightness (AMD only)"), configurationGroupBox);
    desktopContrastLabel = new QLabel(tr("Desktop contrast (AMD only)"), configurationGroupBox);
    gameSaturationLabel = new QLabel(tr("Game saturation"), configurationGroupBox);
    gameBrightnessLabel = new QLabel(tr("Game brightness (AMD only)"), configurationGroupBox);
    gameContrastLabel = new QLabel(tr("Game contrast (AMD only)"), configurationGroupBox);

    processNameLineEdit = new QLineEdit(configurationGroupBox);

    displayIdSpinBox = new QSpinBox(configurationGroupBox);
    displayIdSpinBox->setRange(0, 3);
    displayIdSpinBox->setValue(0);

    gpuVendorComboBox = new QComboBox(configurationGroupBox);
    gpuVendorComboBox->addItem("AMD", GpuVendor::AMD);
    gpuVendorComboBox->addItem("NVIDIA", GpuVendor::NVIDIA);    

    desktopSaturationSpinBox = new QSpinBox(configurationGroupBox);
    desktopSaturationSpinBox->setRange(0, 200);
    desktopSaturationSpinBox->setValue(0);
    desktopBrightnessSpinBox = new QSpinBox(configurationGroupBox);
    desktopBrightnessSpinBox->setRange(0, 100);
    desktopBrightnessSpinBox->setValue(0);
    desktopContrastSpinBox = new QSpinBox(configurationGroupBox);
    desktopContrastSpinBox->setRange(0, 200);
    desktopContrastSpinBox->setValue(0);

    gameSaturationSpinBox = new QSpinBox(configurationGroupBox);
    gameSaturationSpinBox->setRange(0, 200);
    gameSaturationSpinBox->setValue(0);
    gameBrightnessSpinBox = new QSpinBox(configurationGroupBox);
    gameBrightnessSpinBox->setRange(0, 100);
    gameBrightnessSpinBox->setValue(0);
    gameContrastSpinBox = new QSpinBox(configurationGroupBox);
    gameContrastSpinBox->setRange(0, 200);
    gameContrastSpinBox->setValue(0);

    saveConfigurationButton = new QPushButton(tr("Save"), configurationGroupBox);
    saveConfigurationButton->setDefault(true);
    saveLabel = new QLabel(tr("Saved!"), configurationGroupBox);
    saveLabel->setVisible(false);

    QGridLayout* configurationLayout = new QGridLayout(this);
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

    google::protobuf::TextFormat::ParseFromString(lines.toStdString(), &saturationConfig);

    processNameLineEdit->setText(QString::fromStdString(saturationConfig.process_name()));
    displayIdSpinBox->setValue(saturationConfig.display_id());
    gpuVendorComboBox->setCurrentIndex(saturationConfig.vendor());

    desktopSaturationSpinBox->setValue(saturationConfig.desktop_saturation());
    desktopBrightnessSpinBox->setValue(saturationConfig.desktop_brightness());
    desktopContrastSpinBox->setValue(saturationConfig.desktop_contrast());

    gameSaturationSpinBox->setValue(saturationConfig.game_saturation());
    gameBrightnessSpinBox->setValue(saturationConfig.game_brightness());
    gameContrastSpinBox->setValue(saturationConfig.game_contrast());

    configLoaded = true;
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