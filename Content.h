#ifndef CONTENT_H
#define CONTENT_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QDialog>
#include <QEvent>
#include <QMouseEvent>
#include <QKeySequence>
#include <QPoint>
#include <QScreen>
#include <QApplication>
#include <QRegularExpression>
#include <QPalette>
#include <QTabWidget>
#include <windows.h>
#include "AutoClicker.h"
#include "hotkeysettingstab.h"

class InputWidget : public QWidget {
    Q_OBJECT
public:
    explicit InputWidget(QWidget* parent = nullptr);
    ~InputWidget();

    QPushButton* hotkeyBut;  // Public access
    QPushButton* clickBut;

signals:
    void hotkeyChanged(const QKeySequence& sequence);

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private slots:
    void toggleAutoclicker();
    void setPositionFromInput();
    void pickPositionFromCursor();
    void updateHotkey();
    void onHotkeySaved(const Hotkey &hotkey);

private:
    // Private helper functions
    qint64 calculateTotalMs() const;
    int validateClicksInput() const;
    void updateStatus(const QString& message);
    bool startAutoclicker();
    void stopAutoclicker();
    void registerWindowsHotkey(const Hotkey &hotkey);
    void unregisterWindowsHotkey();

    // Helper methods for safer widget operations
    void applyWidgetStyle(QWidget* widget, const QString& style);
    void setWidgetPlaceholder(QLineEdit* lineEdit, const QString& placeholder);
    void setWidgetCursor(QWidget* widget, Qt::CursorShape cursor);
    void setupInputValidation();
    void validateNumericInput(QLineEdit* lineEdit, const QString& text, int min, int max);
    bool isPositionValid(const QPoint& pos) const;
    void setupLayout();

    // UI Elements - all properly initialized in constructor
    QLineEdit* hours;
    QLineEdit* mins;
    QLineEdit* secs;
    QLineEdit* ms;
    QLineEdit* clicks;
    QLineEdit* posInp;
    QLineEdit* durationHours;
    QLineEdit* durationMins;
    QLineEdit* durationSecs;
    QPushButton* posSet;
    QPushButton* posPick;
    QCheckBox* doubleClickCheckbox;
    QLabel* doubleClickLabel;
    QCheckBox* rightClickCheckbox;
    QLabel* rightClickLabel;
    QLabel* status;
    QLabel* interval;
    QLabel* clicksLab;
    QLabel* press;
    QLabel* posLab;
    QLabel* durationLab;

    // Hotkey settings dialog
    QDialog* hotkeyDialog;
    HotkeySettingsTab* hotkeySettingsTab;

    // Business logic
    AutoClicker m_autoclicker;
    QPoint m_targetPos;
    Hotkey m_currentHotkey;
    bool m_isActive;
    bool m_hotkeyRegistered;
    static const int HOTKEY_ID = 1;
    qint64 calculateDurationMs() const;
    QString m_startButtonStyle;
    QString m_stopButtonStyle;
};

#endif // CONTENT_H
