#include "MainWindow.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QRegion>
#include <QPainterPath>
#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(500, 600);

    // Initialize titleBar first
    titleBar = new TitleBar(this);
    if (titleBar) {
        connect(titleBar, &TitleBar::minimizeClicked, this, &MainWindow::onMinimizeClicked);
        connect(titleBar, &TitleBar::closeClicked, this, &MainWindow::onCloseClicked);
        connect(titleBar, &TitleBar::titleBarDragged, this, &MainWindow::onTitleBarDragged);
    }

    // Initialize inputWidget
    inputWidget = new InputWidget(this);

    // Create central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    if (titleBar) {
        layout->addWidget(titleBar);
    }
    if (inputWidget) {
        layout->addWidget(inputWidget);
    }

    layout->addStretch();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setCentralWidget(centralWidget);
    setFocusPolicy(Qt::StrongFocus);
}

// Destructor removed - not needed since Qt handles cleanup automatically

void MainWindow::onTitleBarDragged(const QPoint &delta) {
    move(pos() + delta);
}

void MainWindow::onMinimizeClicked() {
    showMinimized();
}

void MainWindow::onCloseClicked() {
    close();
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    if (!event) {
        return;
    }

    try {
        const int radius = 15;
        QPixmap pixmap(size());
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::black);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(rect(), radius, radius);
        painter.end();

        setMask(pixmap.mask());
    } catch (const std::exception& e) {
        qDebug() << "Exception in resizeEvent:" << e.what();
    }

    QMainWindow::resizeEvent(event);
}

void MainWindow::paintEvent(QPaintEvent* event) {
    if (!event) {
        return;
    }

    try {
        const int radius = 15;
        const int borderWidth = 1;
        const QColor bgColor("#333");
        const QColor borderColor("#757575");

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        QRectF fullRect = rect().adjusted(
            0.5 * borderWidth,
            0.5 * borderWidth,
            -0.5 * borderWidth,
            -0.5 * borderWidth
            );

        QPainterPath fullPath;
        fullPath.addRoundedRect(fullRect, radius, radius);
        painter.fillPath(fullPath, bgColor);

        painter.save();
        if (titleBar) {
            int clipY = titleBar->height();
            painter.setClipRect(QRectF(0, clipY, width(), height()));
        }
        painter.setPen(QPen(borderColor, borderWidth));
        painter.drawPath(fullPath);
        painter.restore();
    } catch (const std::exception& e) {
        qDebug() << "Exception in paintEvent:" << e.what();
    }
}
