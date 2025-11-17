#include "hotkeysettingswindow.h"
#include <QVBoxLayout>

WindowConfig HotkeySettingsWindow::createConfig()
{
    WindowConfig config;
    config.width = 650;
    config.height = 500;
    config.borderRadius = 10;
    config.borderWidth = 1;
    config.backgroundColor = QColor("#1e1e1e");
    config.borderColor = QColor("#242424 ");
    config.windowTitle = "Hotkey Settings";
    config.titleTextColor = QColor("#ff6b00");
    config.titleBarColor = QColor("#242424");
    config.titleBarBorderColor = QColor("#757575");
    config.titleFontSize = "14px";
    config.titleBarHeight = 30;
    config.fixedSize = true;
    config.showTitleBar = true;
    config.showMinimizeButton = false;
    config.showCloseButton = true;

    config.minimizeIcon = ":titlebar/Minimize.png";
    config.minimizeHoverIcon = "assets/MinimizeHover.png";
    config.closeIcon = "assets/Close.png";
    config.closeHoverIcon = "assets/CloseHover.png";

    return config;
}

HotkeySettingsWindow::HotkeySettingsWindow(QWidget *parent)
    : CustomWindowBase(parent, createConfig())
{
    setupWindow();
}

void HotkeySettingsWindow::setupWindow()
{
    settingsTab = new HotkeySettingsTab(this);

    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(centralWidget()->layout());

    if (mainLayout) {
        mainLayout->addWidget(settingsTab, 1);
    }

    connect(settingsTab, &HotkeySettingsTab::hotkeySaved, this, &HotkeySettingsWindow::hotkeySaved);
}
