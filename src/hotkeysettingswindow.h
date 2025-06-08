#ifndef HOTKEYSETTINGSWINDOW_H
#define HOTKEYSETTINGSWINDOW_H

#include <QWidget>
#include "TitleBar.h"
#include "hotkeysettingstab.h"

class HotkeySettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HotkeySettingsWindow(QWidget *parent = nullptr);

signals:
    void hotkeySaved(const Hotkey &hotkey);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void setupWindow();

    TitleBar *titleBar;
    HotkeySettingsTab *settingsTab;
};

#endif // HOTKEYSETTINGSWINDOW_H
