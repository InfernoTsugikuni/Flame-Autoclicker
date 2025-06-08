#include "Functions.h"
#include <QString>
#include <QWidget>
#include <QLineEdit>

void Functions::onHotkeyButtonClicked(QWidget* parent) {
    QMessageBox::StandardButton response = QMessageBox::question(
        parent,
        "Hotkey Window",
        "Current hotkey: Ctrl + Alt + PrtSc\nWould you like to set a new hotkey?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
        );

    if (response == QMessageBox::Yes) {
        QMessageBox::information(parent, "Hotkey", "Hotkey change functionality not yet implemented.");
    }
}
