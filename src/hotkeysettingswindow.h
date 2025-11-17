#ifndef HOTKEYSETTINGSWINDOW_H
#define HOTKEYSETTINGSWINDOW_H

#include "QtBlaze.h"
#include "hotkeysettingstab.h"

class HotkeySettingsWindow : public CustomWindowBase
{
    Q_OBJECT

public:
    explicit HotkeySettingsWindow(QWidget *parent = nullptr);

signals:
    void hotkeySaved(const Hotkey &hotkey);

private:
    void setupWindow();

    static WindowConfig createConfig();

    HotkeySettingsTab *settingsTab;
};

#endif // HOTKEYSETTINGSWINDOW_H
