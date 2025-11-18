#ifndef CONTENT_H
#define CONTENT_H

#include <QWidget>
#include <QPoint>
#include <windows.h>
#include "AutoClicker.h"
#include "hotkeysettingstab.h"

class QLineEdit;
class QPushButton;
class QCheckBox;
class QLabel;

class MainContent : public QWidget {
    Q_OBJECT
public:
    explicit MainContent(QWidget* parent = nullptr);
    ~MainContent();

signals:
    void hotkeyChanged(const QKeySequence& sequence);

protected:
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private slots:
    void toggleAutoclicker();
    void setPositionFromInput();
    void pickPositionFromCursor();
    void clearPosition();
    void updateHotkey();
    void onHotkeySaved(const Hotkey &hotkey);

private:
    // Helper functions
    void setupUi();
    void setupStyles();
    void setupConnections();
    void setupValidators();

    qint64 calculateTotalMs() const;
    qint64 calculateDurationMs() const;
    int validateClicksInput() const;
    void updateStatus(const QString& message);
    void updateStatus(const QString& message, const QColor& color);
    bool startAutoclicker();
    void stopAutoclicker();
    void registerWindowsHotkey(const Hotkey &hotkey);
    void unregisterWindowsHotkey();
    bool isPositionValid(const QPoint& pos) const;

    // Widget helpers
    void applyWidgetStyle(QWidget* widget, const QString& style);
    void setWidgetPlaceholder(QLineEdit* lineEdit, const QString& placeholder);
    void setWidgetCursor(QWidget* widget, Qt::CursorShape cursor);

    // UI Elements
    QLineEdit* hours = nullptr;
    QLineEdit* mins = nullptr;
    QLineEdit* secs = nullptr;
    QLineEdit* ms = nullptr;
    QLineEdit* clicks = nullptr;
    QLineEdit* posInp = nullptr;
    QLineEdit* durationHours = nullptr;
    QLineEdit* durationMins = nullptr;
    QLineEdit* durationSecs = nullptr;

    QPushButton* clickBut = nullptr;
    QPushButton* hotkeyBut = nullptr;
    QPushButton* posSet = nullptr;
    QPushButton* posPick = nullptr;
    QPushButton* posClear = nullptr;

    QCheckBox* doubleClickCheckbox = nullptr;
    QLabel* doubleClickLabel = nullptr;
    QCheckBox* rightClickCheckbox = nullptr;
    QLabel* rightClickLabel = nullptr;

    QLabel* status = nullptr;
    QLabel* interval = nullptr;
    QLabel* clicksLab = nullptr;
    QLabel* press = nullptr;
    QLabel* posLab = nullptr;
    QLabel* durationLab = nullptr;

    // Business logic
    AutoClicker m_autoclicker;
    QPoint m_targetPos;
    Hotkey m_currentHotkey;
    bool m_isActive = false;
    bool m_hotkeyRegistered = false;

    // Style strings for the main button
    QString m_startButtonStyle;
    QString m_stopButtonStyle;

    static constexpr int HOTKEY_ID = 1;
};

#endif // CONTENT_H
