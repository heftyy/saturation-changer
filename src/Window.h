#pragma once

#include <QSystemTrayIcon>
#include <QDialog>
#include <Configuration.pb.h>

#include "ProcessMonitor.h"

class QAction;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QPushButton;
class QSpinBox;
class QTextEdit;

namespace SaturationChanger {

#define CONFIGURATION_FILE "saturation_changer.conf"

class Window : public QDialog {
    Q_OBJECT

public:
    Window();

    void setVisible(bool visible) override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void vendorChanged(int index) const;
    void saveConfiguration();

private:
    void createMessageGroupBox();
    void createActions();
    void createTrayIcon();
    void loadConfiguration();
    void fadeOutSave();

    QGroupBox* configurationGroupBox;

    QLabel* processNameLabel;
    QLabel* displayIdLabel;
    QLabel* gpuVendorLabel;
    QLabel* desktopSaturationLabel;
    QLabel* desktopBrightnessLabel;
    QLabel* desktopContrastLabel;
    QLabel* gameSaturationLabel;
    QLabel* gameBrightnessLabel;
    QLabel* gameContrastLabel;

    QLineEdit* processNameLineEdit;
    QSpinBox* displayIdSpinBox;
    QComboBox* gpuVendorComboBox;
    QSpinBox* desktopSaturationSpinBox;
    QSpinBox* desktopBrightnessSpinBox;
    QSpinBox* desktopContrastSpinBox;
    QSpinBox* gameSaturationSpinBox;
    QSpinBox* gameBrightnessSpinBox;
    QSpinBox* gameContrastSpinBox;

    QPushButton* saveConfigurationButton;
    QLabel* saveLabel;

    QAction* minimizeAction;
    QAction* maximizeAction;
    QAction* restoreAction;
    QAction* quitAction;

    QIcon icon;
    QSystemTrayIcon* trayIcon;
    QMenu* trayIconMenu;

    std::unique_ptr<ProcessMonitor> monitor;
	Configuration saturationConfig;
};

}
