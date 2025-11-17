#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Config
    WindowConfig config;
    config.width = 500;
    config.height = 600;
    config.borderRadius = 15;
    config.borderWidth = 1;
    config.backgroundColor = QColor("#333");
    config.borderColor = QColor("#757575");
    config.windowTitle = "Flame Autoclicker";
    config.titleTextColor = QColor("#ff6b00");
    config.titleBarColor = QColor("#242424");
    config.titleBarBorderColor = QColor("#757575");
    config.titleFontSize = "16px";
    config.titleBarHeight = 30;
    config.fixedSize = true;
    config.showTitleBar = true;
    config.showMinimizeButton = true;
    config.showCloseButton = true;

    config.minimizeIcon = ":titlebar/Minimize.png";
    config.minimizeHoverIcon = "assets/MinimizeHover.png";
    config.closeIcon = "assets/Close.png";
    config.closeHoverIcon = "assets/CloseHover.png";

    MainWindow customWindow(nullptr, config);
    customWindow.show();

    return app.exec();
}
