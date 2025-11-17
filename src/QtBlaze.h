#ifndef QTBLAZE_H
#define QTBLAZE_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

// Configuration struct for window appearance
struct WindowConfig {
    int width = 800;
    int height = 600;
    int borderRadius = 15;
    int borderWidth = 1;
    QColor backgroundColor = QColor("#333");
    QColor borderColor = QColor("#757575");
    bool fixedSize = false;

    // Title bar config
    bool showTitleBar = true;
    bool showMinimizeButton = true;
    bool showCloseButton = true;
    QString windowTitle = "Custom Window";
    QColor titleBarColor = QColor("#242424");
    QColor titleBarBorderColor = QColor("#ff6b00");
    QColor titleTextColor = QColor("#ff6b00");
    QString titleFontSize = "16px";
    int titleBarHeight = 30;

    // Button assets (optional - will use default styles if empty)
    QString minimizeIcon = "";
    QString minimizeHoverIcon = "";
    QString closeIcon = "";
    QString closeHoverIcon = "";
};

// Custom TitleBar class
class CustomTitleBar : public QWidget {
    Q_OBJECT
public:
    explicit CustomTitleBar(QWidget* parent = nullptr, const WindowConfig& config = WindowConfig{})
        : QWidget(parent), m_config(config) {
        setFixedHeight(config.titleBarHeight);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        createUI();
    }

    void setTitle(const QString& title) {
        if (titleLabel) {
            titleLabel->setText(title);
        }
    }

signals:
    void minimizeClicked();
    void closeClicked();
    void titleBarDragged(const QPoint& delta);

protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            offset = event->globalPosition().toPoint();
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (event->buttons() & Qt::LeftButton) {
            QPoint delta = event->globalPosition().toPoint() - offset;
            offset = event->globalPosition().toPoint();
            emit titleBarDragged(delta);
        }
    }

    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        // Use config
        const int radius = m_config.borderRadius;
        const QColor bgColor = m_config.titleBarColor;
        const QColor borderColor = m_config.titleBarBorderColor;
        const int borderWidth = m_config.borderWidth;

        const float halfBorder = borderWidth / 2.0f;

        // Create rounded path for top corners
        QPainterPath path;
        path.moveTo(halfBorder, radius + halfBorder);
        path.quadTo(halfBorder, halfBorder, radius + halfBorder, halfBorder);
        path.lineTo(width() - radius - halfBorder, halfBorder);
        path.quadTo(width() - halfBorder, halfBorder, width() - halfBorder, radius + halfBorder);
        path.lineTo(width() - halfBorder, height() - halfBorder);
        path.lineTo(halfBorder, height() - halfBorder);
        path.closeSubpath();

        // Draw background
        p.fillPath(path, bgColor);

        // Draw border
        p.setPen(QPen(borderColor, borderWidth));
        p.drawPath(path);
    }

private:
    WindowConfig m_config;
    QLabel* titleLabel = nullptr;
    QPushButton* minimizeButton = nullptr;
    QPushButton* closeButton = nullptr;
    QPoint offset;

    void createUI() {
        titleLabel = new QLabel(m_config.windowTitle, this);
        titleLabel->setStyleSheet(QString("color: %1; font-size: %2; font-weight: bold;")
                                      .arg(m_config.titleTextColor.name())
                                      .arg(m_config.titleFontSize));

        if (m_config.showMinimizeButton) {
            minimizeButton = new QPushButton(this);
            minimizeButton->setFixedSize(18, 18);
            minimizeButton->setCursor(Qt::PointingHandCursor);

            QString minimizeStyle = "QPushButton { background-color: transparent; border: none; }";
            if (!m_config.minimizeIcon.isEmpty()) {
                minimizeStyle = QString(
                                    "QPushButton {"
                                    "    background-image: url(%1);"
                                    "    background-color: transparent;"
                                    "    border: none;"
                                    "}").arg(m_config.minimizeIcon);

                if (!m_config.minimizeHoverIcon.isEmpty()) {
                    minimizeStyle += QString(
                                         "QPushButton:hover {"
                                         "    background-image: url(%1);"
                                         "}").arg(m_config.minimizeHoverIcon);
                }
            } else {
                // Default minimize button style
                minimizeStyle =
                    "QPushButton {"
                    "    background-color: #666;"
                    "    border: none;"
                    "    color: white;"
                    "}"
                    "QPushButton:hover {"
                    "    background-color: #888;"
                    "}";
                minimizeButton->setText("−");
            }
            minimizeButton->setStyleSheet(minimizeStyle);
            connect(minimizeButton, &QPushButton::clicked, this, &CustomTitleBar::minimizeClicked);
        }

        if (m_config.showCloseButton) {
            closeButton = new QPushButton(this);
            closeButton->setFixedSize(18, 18);
            closeButton->setCursor(Qt::PointingHandCursor);

            QString closeStyle = "QPushButton { background-color: transparent; border: none; }";
            if (!m_config.closeIcon.isEmpty()) {
                closeStyle = QString(
                                 "QPushButton {"
                                 "    background-image: url(%1);"
                                 "    background-color: transparent;"
                                 "    border: none;"
                                 "}").arg(m_config.closeIcon);

                if (!m_config.closeHoverIcon.isEmpty()) {
                    closeStyle += QString(
                                      "QPushButton:hover {"
                                      "    background-image: url(%1);"
                                      "}").arg(m_config.closeHoverIcon);
                }
            } else {
                // Default close button style
                closeStyle =
                    "QPushButton {"
                    "    background-color: #d32f2f;"
                    "    border: none;"
                    "    color: white;"
                    "}"
                    "QPushButton:hover {"
                    "    background-color: #f44336;"
                    "}";
                closeButton->setText("×");
            }
            closeButton->setStyleSheet(closeStyle);
            connect(closeButton, &QPushButton::clicked, this, &CustomTitleBar::closeClicked);
        }

        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(10, 0, 10, 0);
        layout->addWidget(titleLabel);
        layout->addStretch();

        if (m_config.showMinimizeButton && minimizeButton) {
            layout->addWidget(minimizeButton);
        }

        if (m_config.showCloseButton && closeButton) {
            layout->addWidget(closeButton);
        }

        setLayout(layout);
    }
};

// Function to setup a custom frameless window with rounded corners
inline void setupCustomWindow(QMainWindow* window, const WindowConfig& config = WindowConfig{}) {
    if (!window) return;

    // Set window flags for frameless, translucent window
    window->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    window->setAttribute(Qt::WA_TranslucentBackground);

    // Set window size
    if (config.fixedSize) {
        window->setFixedSize(config.width, config.height);
    } else {
        window->resize(config.width, config.height);
    }

    window->setFocusPolicy(Qt::StrongFocus);
}

// Base class that handles everything automatically
class CustomWindowBase : public QMainWindow {
    Q_OBJECT

public:
    explicit CustomWindowBase(QWidget *parent = nullptr, const WindowConfig& config = WindowConfig{})
        : QMainWindow(parent), m_config(config) {
        setupCustomWindow(this, config);
        setupUI();
    }

    void setWindowConfig(const WindowConfig& config) {
        m_config = config;
        if (m_titleBar) {
            m_titleBar->setTitle(config.windowTitle);
        }
        update();
    }

    void setWindowTitle(const QString& title) {
        if (m_titleBar) {
            m_titleBar->setTitle(title);
        }
        QMainWindow::setWindowTitle(title);
    }

    CustomTitleBar* getTitleBar() const {
        return m_titleBar;
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event);

        if (!m_titleBar) return; // Safety check if title bar pointer is null

        const int radius = m_config.borderRadius;
        const int borderWidth = m_config.borderWidth;
        const QColor bgColor = m_config.backgroundColor;
        const QColor borderColor = m_config.titleBarBorderColor;

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // Adjust for border width
        QRectF fullRect = rect().adjusted(
            borderWidth / 2.0,
            borderWidth / 2.0,
            -borderWidth / 2.0,
            -borderWidth / 2.0
            );

        QPainterPath fullPath;
        fullPath.addRoundedRect(fullRect, radius, radius);

        // Fill background
        painter.fillPath(fullPath, bgColor);

        // Save painter state before clipping
        painter.save();

        // Clip to skip title bar area if shown
        if (m_config.showTitleBar && m_titleBar) {
            int clipY = m_titleBar->height();
            painter.setClipRect(QRectF(0, clipY, width(), height() - clipY));
        }

        // Draw border
        QPen borderPen(borderColor, borderWidth);
        borderPen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(borderPen);
        painter.drawPath(fullPath);

        // Restore painter state
        painter.restore();
    }

    void resizeEvent(QResizeEvent* event) override {
        if (!event) return;

        try {
            // Create rounded mask
            QPixmap pixmap(size());
            pixmap.fill(Qt::transparent);

            QPainter painter(&pixmap);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setBrush(Qt::black);
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(rect(), m_config.borderRadius, m_config.borderRadius);
            painter.end();

            setMask(pixmap.mask());
        } catch (const std::exception& e) {
            qDebug() << "Exception in resizeEvent:" << e.what();
        }

        QMainWindow::resizeEvent(event);
    }

private slots:
    void onTitleBarDragged(const QPoint &delta) {
        move(pos() + delta);
    }

    void onMinimizeClicked() {
        showMinimized();
    }

    void onCloseClicked() {
        close();
    }

private:
    WindowConfig m_config;
    CustomTitleBar* m_titleBar = nullptr;

    virtual void setupUI() {
        // Create central widget and layout
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);

        // Add title bar if enabled
        if (m_config.showTitleBar) {
            m_titleBar = new CustomTitleBar(this, m_config);
            connect(m_titleBar, &CustomTitleBar::minimizeClicked, this, &CustomWindowBase::onMinimizeClicked);
            connect(m_titleBar, &CustomTitleBar::closeClicked, this, &CustomWindowBase::onCloseClicked);
            connect(m_titleBar, &CustomTitleBar::titleBarDragged, this, &CustomWindowBase::onTitleBarDragged);
            layout->addWidget(m_titleBar);
        }

        layout->addStretch();
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

        setCentralWidget(centralWidget);
    }
};

#endif // QTBLAZE_H
