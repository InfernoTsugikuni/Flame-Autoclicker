#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "QtBlaze.h"
#include "Content.h"

class MainWindow : public CustomWindowBase
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    explicit MainWindow(QWidget *parent, const WindowConfig& config);

private:
    MainContent *mainContent = nullptr;

    void setupUI();
};

#endif // MAINWINDOW_H
