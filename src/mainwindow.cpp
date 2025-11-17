#include "MainWindow.h"
#include <QVBoxLayout>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : CustomWindowBase(parent)
{
    setupUI();
}

MainWindow::MainWindow(QWidget *parent, const WindowConfig& config)
    : CustomWindowBase(parent, config)
{
    setupUI();
}

void MainWindow::setupUI()
{
    mainContent = new class MainContent(this);

    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(centralWidget()->layout());
    if (mainLayout && mainContent) {
        mainLayout->addWidget(mainContent, 1);
    }
}
