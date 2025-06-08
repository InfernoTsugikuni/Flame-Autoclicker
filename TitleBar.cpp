#include "TitleBar.h"
#include <QMouseEvent>
#include <QPainter>
#include <QHBoxLayout>

TitleBar::TitleBar(QWidget* parent) : QWidget(parent) {
    setFixedHeight(30);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    createUI();
}

void TitleBar::createUI() {
    setAttribute(Qt::WA_TranslucentBackground, false);

    titleLabel = new QLabel("Flame Autoclicker", this);
    titleLabel->setStyleSheet("color: #ff6b00; font-size: 16px; font-weight: bold;");

    minimizeButton = new QPushButton(this);
    minimizeButton->setFixedSize(18, 18);
    minimizeButton->setCursor(Qt::PointingHandCursor);
    minimizeButton->setStyleSheet(
        "QPushButton {"
        "    background-image: url(assets/Minimize.png);"
        "    background-color: transparent;"
        "    border: none;"
        "}"
        "QPushButton:hover {"
        "    background-image: url(assets/MinimizeHover.png);"
        "}"
        );
    connect(minimizeButton, &QPushButton::clicked, this, &TitleBar::minimizeClicked);

    closeButton = new QPushButton(this);
    closeButton->setFixedSize(18, 18);
    closeButton->setCursor(Qt::PointingHandCursor);
    closeButton->setStyleSheet(
        "QPushButton {"
        "    background-image: url(assets/Close.png);"
        "    background-color: transparent;"
        "    border: none;"
        "}"
        "QPushButton:hover {"
        "    background-image: url(assets/CloseHover.png);"
        "}"
        );
    connect(closeButton, &QPushButton::clicked, this, &TitleBar::closeClicked);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 0, 10, 0);
    layout->addWidget(titleLabel);
    layout->addStretch();
    layout->addWidget(minimizeButton);
    layout->addWidget(closeButton);
    setLayout(layout);
}

void TitleBar::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        offset = event->globalPosition().toPoint();
    }
}

void TitleBar::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() & Qt::LeftButton) {
        QPoint delta = event->globalPosition().toPoint() - offset;
        offset = event->globalPosition().toPoint();
        emit titleBarDragged(delta);
    }
}

void TitleBar::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int radius = 15;
    const QColor bgColor("#242424");
    const QColor borderColor("#757575");

    QPainterPath path;
    path.moveTo(0, radius);
    path.quadTo(0, 0, radius, 0);
    path.lineTo(width() - radius, 0);
    path.quadTo(width(), 0, width(), radius);
    path.lineTo(width(), height());
    path.lineTo(0, height());
    path.closeSubpath();

    p.fillPath(path, bgColor);
    p.setPen(QPen(borderColor, 1));
    p.drawLine(0, height() - 1, width(), height() - 1);
}
