#include "hotkeysettingswindow.h"
#include <QVBoxLayout>
#include <QPainter>

HotkeySettingsWindow::HotkeySettingsWindow(QWidget *parent)
    : QWidget(parent)
{
    // Window setup
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(650, 530); // Slightly larger to accommodate title bar

    setupWindow();
}

void HotkeySettingsWindow::setupWindow()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Add custom title bar
    titleBar = new TitleBar(this);
    mainLayout->addWidget(titleBar);

    // Add settings tab
    settingsTab = new HotkeySettingsTab(this);
    mainLayout->addWidget(settingsTab, 1);

    // Connect signals
    connect(titleBar, &TitleBar::minimizeClicked, this, &QWidget::showMinimized);
    connect(titleBar, &TitleBar::closeClicked, this, &QWidget::close);
    connect(titleBar, &TitleBar::titleBarDragged, [this](const QPoint &delta) {
        move(pos() + delta);
    });
    connect(settingsTab, &HotkeySettingsTab::hotkeySaved, this, &HotkeySettingsWindow::hotkeySaved);

    // Style the window with rounded corners
    setStyleSheet("HotkeySettingsWindow { background-color: #1e1e1e; border-radius: 10px; }");
}

void HotkeySettingsWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw rounded rectangle background
    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);
    painter.fillPath(path, QColor("#1e1e1e"));

    // Draw border
    painter.setPen(QColor("#3a3a3a"));
    painter.drawPath(path);
}
