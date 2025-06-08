#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QPainterPath>

class TitleBar : public QWidget {
    Q_OBJECT
public:
    explicit TitleBar(QWidget* parent = nullptr);

signals:
    void minimizeClicked();
    void closeClicked();
    void titleBarDragged(const QPoint& delta);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QLabel* titleLabel;
    QPushButton* minimizeButton;
    QPushButton* closeButton;
    QPoint offset;

    void createUI();
};

#endif // TITLEBAR_H
