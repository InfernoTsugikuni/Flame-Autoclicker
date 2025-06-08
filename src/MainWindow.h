#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include "TitleBar.h"
#include "Content.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onTitleBarDragged(const QPoint &delta);
    void onMinimizeClicked();
    void onCloseClicked();

private:
    TitleBar *titleBar = nullptr;
    InputWidget *inputWidget = nullptr;
};

#endif // MAINWINDOW_H
