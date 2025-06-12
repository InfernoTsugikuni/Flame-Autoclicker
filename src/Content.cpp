#include "Content.h"
#include "hotkeysettingswindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>
#include <QTimer>
#include <QInputDialog>
#include <QKeySequenceEdit>
#include <QScreen>
#include <QRegularExpression>
#include <QPalette>
#include <windows.h>

namespace {
QString getKeyName(int keyCode) {
    // Map common keys to shorter names
    switch (keyCode) {
    case VK_SNAPSHOT: return "PrtSc";
    case VK_SPACE: return "Space";
    case VK_RETURN: return "Enter";
    case VK_BACK: return "Backspace";
    case VK_DELETE: return "Del";
    case VK_INSERT: return "Ins";
    case VK_ESCAPE: return "Esc";
    case VK_HOME: return "Home";
    case VK_END: return "End";
    case VK_PRIOR: return "PgUp";
    case VK_NEXT: return "PgDn";
    case VK_LEFT: return "Left";
    case VK_RIGHT: return "Right";
    case VK_UP: return "Up";
    case VK_DOWN: return "Down";
    }

    // Handle function keys F1-F24
    if (keyCode >= VK_F1 && keyCode <= VK_F24) {
        return "F" + QString::number(keyCode - VK_F1 + 1);
    }

    // Use WinAPI to get key name
    UINT scanCode = MapVirtualKeyW(keyCode, MAPVK_VK_TO_VSC);
    if (scanCode == 0) return "Key " + QString::number(keyCode);

    wchar_t name[256];
    // Try without extended flag
    LONG lParam = scanCode << 16;
    if (GetKeyNameTextW(lParam, name, 256) > 0) {
        return QString::fromWCharArray(name).trimmed();
    }
    // Try with extended flag
    lParam = (scanCode << 16) | (1 << 24);
    if (GetKeyNameTextW(lParam, name, 256) > 0) {
        return QString::fromWCharArray(name).trimmed();
    }
    return "Key " + QString::number(keyCode);
}

QString hotkeyString(const Hotkey& hotkey) {
    QStringList parts;
    if (hotkey.ctrl) parts << "Ctrl";
    if (hotkey.shift) parts << "Shift";
    if (hotkey.alt) parts << "Alt";
    if (hotkey.win) parts << "Win";
    parts << getKeyName(hotkey.keyCode);
    return parts.join(" + ");
}
}


InputWidget::InputWidget(QWidget* parent) : QWidget(parent) {
    // Initialize all member variables first
    m_currentHotkey = {false, false, false, false, VK_F6}; // Default: F6
    m_isActive = false;
    m_hotkeyRegistered = false;
    m_targetPos = QPoint(0, 0);

    // Initialize all pointers to nullptr first
    hours = nullptr;
    mins = nullptr;
    secs = nullptr;
    ms = nullptr;
    clicks = nullptr;
    posInp = nullptr;
    durationHours = nullptr;
    durationMins = nullptr;
    durationSecs = nullptr;
    clickBut = nullptr;
    hotkeyBut = nullptr;
    posSet = nullptr;
    posPick = nullptr;
    doubleClickCheckbox = nullptr;
    doubleClickLabel = nullptr;
    rightClickCheckbox = nullptr;
    rightClickLabel = nullptr;
    interval = nullptr;
    clicksLab = nullptr;
    status = nullptr;
    press = nullptr;
    posLab = nullptr;
    durationLab = nullptr;
    hotkeyDialog = nullptr;

    // Define styles
    const QString inputStyle = R"(
        QLineEdit {
            padding: 9px;
            border: 1px solid #555;
            border-radius: 6px;
            background-color: #2d2d2d;
            color: #ddd;
            font-size: 14px;
        }
        QLineEdit:focus {
            border-color: #ff6b00;
            background-color: #3a3a3a;
            color: #fff;
        }
    )";

    const QString buttonStyle = R"(
        QPushButton {
            padding: 10px;
            border-radius: 6px;
            background-color: #2d2d2d;
            color: #fff;
            font-weight: bold;
            font-size: 14px;
            border: 1px solid #555;
        }
        QPushButton:hover {
            background-color: #ff6b00;
            border-color: #ff6b00;
            color: #fff;
        }
    )";

    const QString mainButtonStyle = R"(
        QPushButton {
            padding: 12px;
            border-radius: 8px;
            background-color: #ff6b00;
            color: #fff;
            font-weight: bold;
            font-size: 16px;
            border: none;
        }
        QPushButton:hover {
            background-color: #e65c00;
        }
        QPushButton:disabled {
            background-color: #666;
            color: #999;
        }
    )";

    const QString secondaryButtonStyle = R"(
        QPushButton {
            padding: 9px;
            border-radius: 5px;
            background-color: #2d2d2d;
            color: #919191;
            font-size: 14px;
            border: 1px solid #555;
        }
        QPushButton:hover {
            background-color: #3f3f3f;
            color: #fff;
        }
        QPushButton:disabled {
            background-color: #1a1a1a;
            color: #666;
        }
    )";

    const QString sectionLabelStyle = "font-weight: bold; color: #bbb; font-size: 16px;";
    const QString statusLabelStyle = "color: #aaa; font-size: 14px;";

    const QString checkboxStyle = R"(
        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border-radius: 10px;
            border: 2px solid #555;
            background-color: #2d2d2d;
        }
        QCheckBox::indicator:checked {
            border: 2px solid #ff6b00;
            background-color: #ff6b00;
        }
        QCheckBox::indicator:unchecked:hover {
            border-color: #aaaaaa;
        }
    )";

    // Create all widgets with error handling
    try {
        hours = new QLineEdit(this);
        mins = new QLineEdit(this);
        secs = new QLineEdit(this);
        ms = new QLineEdit(this);
        clicks = new QLineEdit(this);
        posInp = new QLineEdit(this);
        durationHours = new QLineEdit(this);
        durationMins = new QLineEdit(this);
        durationSecs = new QLineEdit(this);
        clickBut = new QPushButton("Start Clicking", this);
        hotkeyBut = new QPushButton("Change Hotkey", this);
        posSet = new QPushButton("Set Position", this);
        posPick = new QPushButton("Pick Position", this);
        doubleClickCheckbox = new QCheckBox(this);
        doubleClickLabel = new QLabel("Double Click", this);
        rightClickCheckbox = new QCheckBox(this);
        rightClickLabel = new QLabel("Right Click", this);
        interval = new QLabel("Interval | Blank for none:", this);
        clicksLab = new QLabel("Number of Clicks | Blank for infinite (until stopped):", this);
        durationLab = new QLabel("Duration | Blank for until stopped:", this);
        status = new QLabel("Ready to use", this);
        press = new QLabel(this);
        posLab = new QLabel("Position | Blank for current pos:", this);
    } catch (const std::exception& e) {
        qCritical() << "Failed to create widgets:" << e.what();
        updateStatus("Error: Failed to initialize interface");
        return;
    } catch (...) {
        qCritical() << "Unknown error creating widgets";
        updateStatus("Error: Failed to initialize interface");
        return;
    }

    // Verify all critical widgets were created
    if (!hours || !mins || !secs || !ms || !clicks || !posInp ||
        !clickBut || !hotkeyBut || !posSet || !posPick ||
        !doubleClickCheckbox || !rightClickCheckbox || !status) {
        qCritical() << "Critical widget creation failed";
        updateStatus("Error: Critical interface components missing");
        return;
    }

    // Connect autoclicker signals for better feedback
    connect(&m_autoclicker, &AutoClicker::started, this, [this]() {
        updateStatus("Autoclicking started");
    });

    connect(&m_autoclicker, &AutoClicker::stopped, this, [this]() {
        updateStatus("Autoclicking stopped");
        m_isActive = false;
        if (clickBut) clickBut->setText("Start Clicking");
    });

    connect(&m_autoclicker, &AutoClicker::finished, this, [this]() {
        updateStatus("Autoclicking completed");
        m_isActive = false;
        if (clickBut) clickBut->setText("Start Clicking");
    });

    connect(&m_autoclicker, &AutoClicker::error, this, [this](const QString& error) {
        updateStatus("Error: " + error);
        m_isActive = false;
        if (clickBut) clickBut->setText("Start Clicking");
    });

    // Connect button signals with proper error handling
    if (clickBut) {
        connect(clickBut, &QPushButton::clicked, this, &InputWidget::toggleAutoclicker);
    }
    if (hotkeyBut) {
        connect(hotkeyBut, &QPushButton::clicked, this, &InputWidget::updateHotkey);
    }
    if (posSet) {
        connect(posSet, &QPushButton::clicked, this, &InputWidget::setPositionFromInput);
    }
    if (posPick) {
        connect(posPick, &QPushButton::clicked, this, &InputWidget::pickPositionFromCursor);
    }
    if (press) {
        press->setText("Press " + hotkeyString(m_currentHotkey) + " to start/stop clicking");
    }

    // Apply styles with null checks
    applyWidgetStyle(hours, inputStyle);
    applyWidgetStyle(mins, inputStyle);
    applyWidgetStyle(secs, inputStyle);
    applyWidgetStyle(ms, inputStyle);
    applyWidgetStyle(clicks, inputStyle);
    applyWidgetStyle(posInp, inputStyle);
    applyWidgetStyle(doubleClickCheckbox, checkboxStyle);
    applyWidgetStyle(rightClickCheckbox, checkboxStyle);
    applyWidgetStyle(posSet, buttonStyle);
    applyWidgetStyle(posPick, buttonStyle);
    applyWidgetStyle(clickBut, mainButtonStyle);
    applyWidgetStyle(hotkeyBut, secondaryButtonStyle);
    applyWidgetStyle(interval, sectionLabelStyle);
    applyWidgetStyle(clicksLab, sectionLabelStyle);
    applyWidgetStyle(posLab, sectionLabelStyle);
    applyWidgetStyle(doubleClickLabel, "color: #bbb; font-size: 12px;");
    applyWidgetStyle(rightClickLabel, "color: #bbb; font-size: 12px;");
    applyWidgetStyle(status, statusLabelStyle);
    applyWidgetStyle(press, statusLabelStyle);
    applyWidgetStyle(durationHours, inputStyle);
    applyWidgetStyle(durationMins, inputStyle);
    applyWidgetStyle(durationSecs, inputStyle);
    applyWidgetStyle(durationLab, sectionLabelStyle);

    // Set placeholders with null checks
    setWidgetPlaceholder(hours, "Hours");
    setWidgetPlaceholder(mins, "Minutes");
    setWidgetPlaceholder(secs, "Seconds");
    setWidgetPlaceholder(ms, "Milliseconds");
    setWidgetPlaceholder(clicks, "0");
    setWidgetPlaceholder(posInp, "e.g., 100, 200");
    setWidgetPlaceholder(durationHours, "Hours");
    setWidgetPlaceholder(durationMins, "Minutes");
    setWidgetPlaceholder(durationSecs, "Seconds");

    // Set cursors with null checks
    setWidgetCursor(rightClickCheckbox, Qt::PointingHandCursor);
    setWidgetCursor(doubleClickCheckbox, Qt::PointingHandCursor);
    setWidgetCursor(posSet, Qt::PointingHandCursor);
    setWidgetCursor(posPick, Qt::PointingHandCursor);
    setWidgetCursor(clickBut, Qt::PointingHandCursor);
    setWidgetCursor(hotkeyBut, Qt::PointingHandCursor);

    // Input validation
    setupInputValidation();

    // Create and set up the layout
    setupLayout();

    // Register default hotkey
    registerWindowsHotkey(m_currentHotkey);

    qDebug() << "InputWidget initialized successfully";
}

InputWidget::~InputWidget() {
    qDebug() << "InputWidget destructor called";

    // Unregister hotkey first
    unregisterWindowsHotkey();

    // Stop autoclicker with error handling
    try {
        if (m_autoclicker.isActive()) {
            m_autoclicker.stop();
            qDebug() << "Autoclicker stopped in destructor";
        }
    } catch (const std::exception& e) {
        qCritical() << "Error stopping autoclicker in destructor:" << e.what();
    } catch (...) {
        qCritical() << "Unknown error stopping autoclicker in destructor";
    }

    // Clean up hotkey dialog
    if (hotkeyDialog) {
        delete hotkeyDialog;
        hotkeyDialog = nullptr;
    }

    qDebug() << "InputWidget destroyed safely";
}

bool InputWidget::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
    if (eventType == "windows_generic_MSG") {
        MSG* msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY && msg->wParam == HOTKEY_ID) {
            // Hotkey was pressed
            toggleAutoclicker();
            return true;
        }
    }
    return QWidget::nativeEvent(eventType, message, result);
}

void InputWidget::toggleAutoclicker()
{
    qDebug() << "toggleAutoclicker called, isActive:" << m_isActive;

    if (!m_isActive) {
        // Validate and start autoclicker
        if (!startAutoclicker()) {
            return; // Error message already shown
        }
    } else {
        // Stop autoclicker
        stopAutoclicker();
    }
}

bool InputWidget::startAutoclicker()
{
    // Validate inputs
    qint64 intervalMs = calculateTotalMs();
    const int clickCount = validateClicksInput();
    const qint64 durationMs = calculateDurationMs();

    // Handle blank interval - set to minimum 50ms
    if (intervalMs <= 0) {
        intervalMs = 50;
    }

    // Enhanced validation: minimum interval check
    if (intervalMs < 50) {
        updateStatus("Error: Interval too fast (minimum 50ms for safety)");
        return false;
    }

    // Validate at least one stopping condition exists
    if (clickCount <= 0 && durationMs <= 0) {
        updateStatus("Info: Clicking will continue until stopped");
    }

    try {
        // Configure autoclicker with safe defaults
        m_autoclicker.setInterval(intervalMs);
        m_autoclicker.setClickCount(clickCount);
        m_autoclicker.setDuration(durationMs);
        m_autoclicker.setPosition(m_targetPos);

        if (doubleClickCheckbox) {
            m_autoclicker.setDoubleClick(doubleClickCheckbox->isChecked());
        }
        if (rightClickCheckbox) {
            m_autoclicker.setRightClick(rightClickCheckbox->isChecked());
        }

        if (m_autoclicker.start()) {
            m_isActive = true;
            updateStatus("Autoclicking started successfully");
            if (clickBut) {
                clickBut->setText("Stop Clicking");
                clickBut->setStyleSheet(clickBut->styleSheet().replace("#ff6b00", "#dc3545"));
            }
            return true;
        } else {
            updateStatus("Error: Failed to start autoclicker");
            return false;
        }
    } catch (const std::exception& e) {
        updateStatus(QString("Error: %1").arg(e.what()));
        return false;
    } catch (...) {
        updateStatus("Error: Unknown error starting autoclicker");
        return false;
    }
}

void InputWidget::stopAutoclicker()
{
    try {
        m_autoclicker.stop();
        m_isActive = false;
        updateStatus("Autoclicking stopped");
        if (clickBut) {
            clickBut->setText("Start Clicking");
            clickBut->setStyleSheet(clickBut->styleSheet().replace("#dc3545", "#ff6b00"));
        }
        qDebug() << "Autoclicker stopped successfully";
    } catch (const std::exception& e) {
        qCritical() << "Error stopping autoclicker:" << e.what();
        updateStatus("Error stopping autoclicker");
    } catch (...) {
        qCritical() << "Unknown error stopping autoclicker";
        updateStatus("Error stopping autoclicker");
    }
}

void InputWidget::setPositionFromInput()
{
    if (!posInp) {
        updateStatus("Error: Position input not available");
        return;
    }

    QString text = posInp->text().trimmed();
    if (text.isEmpty()) {
        updateStatus("Error: Please enter position coordinates");
        return;
    }

    QRegularExpression regex("[,\\s]+");
    QStringList parts = text.split(regex, Qt::SkipEmptyParts);

    if (parts.size() != 2) {
        updateStatus("Error: Invalid format - use 'x, y' (e.g., '100, 200')");
        return;
    }

    bool xOk, yOk;
    const int x = parts[0].trimmed().toInt(&xOk);
    const int y = parts[1].trimmed().toInt(&yOk);

    if (!xOk || !yOk) {
        updateStatus("Error: Coordinates must be valid numbers");
        return;
    }

    if (x < 0 || y < 0) {
        updateStatus("Error: Coordinates must be positive numbers");
        return;
    }

    QPoint newPos(x, y);
    if (!isPositionValid(newPos)) {
        updateStatus("Warning: Position may be outside screen bounds");
        // Continue anyway - user might know what they're doing
    }

    m_targetPos = newPos;
    updateStatus(QString("Position set to: %1, %2").arg(x).arg(y));
    qDebug() << "Position set manually to:" << m_targetPos;
}

void InputWidget::pickPositionFromCursor() {
    if (!posPick || !status) {
        updateStatus("Error: Position picker not available");
        return;
    }

    // Disable button and show feedback
    posPick->setText("Click anywhere...");
    posPick->setDisabled(true);
    updateStatus("Move cursor to desired position and wait 3 seconds...");

    // Use a more reliable timer-based approach
    QTimer* pickTimer = new QTimer(this);
    pickTimer->setSingleShot(true);

    connect(pickTimer, &QTimer::timeout, this, [this, pickTimer]() {
        try {
            // Get current cursor position
            QPoint cursorPos = QCursor::pos();

            // Validate the position
            if (cursorPos.x() >= 0 && cursorPos.y() >= 0) {
                m_targetPos = cursorPos;

                if (posInp) {
                    posInp->setText(QString("%1, %2").arg(m_targetPos.x()).arg(m_targetPos.y()));
                }

                updateStatus(QString("Position picked: %1, %2").arg(cursorPos.x()).arg(cursorPos.y()));
                qDebug() << "Position picked:" << m_targetPos;
            } else {
                updateStatus("Error: Invalid cursor position detected");
            }
        } catch (const std::exception& e) {
            updateStatus(QString("Error picking position: %1").arg(e.what()));
        } catch (...) {
            updateStatus("Error: Unknown error picking position");
        }

        // Re-enable button
        if (posPick) {
            posPick->setText("Pick Position");
            posPick->setDisabled(false);
        }

        // Clean up timer
        pickTimer->deleteLater();
    });

    // Start timer (3 second delay)
    pickTimer->start(3000);
}

void InputWidget::updateHotkey() {
    try {
        // Create and show the HotkeySettingsWindow directly
        HotkeySettingsWindow *settingsWindow = new HotkeySettingsWindow(this);
        connect(settingsWindow, &HotkeySettingsWindow::hotkeySaved,
                this, &InputWidget::onHotkeySaved);

        settingsWindow->show();
        settingsWindow->raise();
        settingsWindow->activateWindow();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Failed to open hotkey settings: %1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(this, "Error", "Unknown error opening hotkey settings");
    }
}

void InputWidget::onHotkeySaved(const Hotkey &hotkey) {
    try {
        // Unregister old hotkey first
        unregisterWindowsHotkey();

        // Update current hotkey
        m_currentHotkey = hotkey;

        // Register new hotkey
        registerWindowsHotkey(hotkey);

        // Update UI to show new hotkey
        QStringList parts;
        if (hotkey.ctrl) parts << "Ctrl";
        if (hotkey.shift) parts << "Shift";
        if (hotkey.alt) parts << "Alt";
        if (hotkey.win) parts << "Win";

        // Find the key name from the key code
        QString keyName = "Unknown";
        if (hotkeySettingsTab) {
            // Get the current text from the combo box
            keyName = hotkeySettingsTab->getCurrentHotkey().keyCode == hotkey.keyCode ?
                          "Key" : QString::number(hotkey.keyCode);
        }

        parts << keyName;

        if (press) {
            press->setText("Press " + hotkeyString(m_currentHotkey) + " to start/stop clicking");
        }

        updateStatus("Hotkey changed successfully");

        // Close the dialog
        if (hotkeyDialog) {
            hotkeyDialog->close();
        }

    } catch (const std::exception& e) {
        updateStatus(QString("Error saving hotkey: %1").arg(e.what()));
    } catch (...) {
        updateStatus("Error: Unknown error saving hotkey");
    }
}

void InputWidget::registerWindowsHotkey(const Hotkey &hotkey) {
    try {
        HWND hwnd = reinterpret_cast<HWND>(this->winId());

        UINT modifiers = 0;
        if (hotkey.ctrl) modifiers |= MOD_CONTROL;
        if (hotkey.shift) modifiers |= MOD_SHIFT;
        if (hotkey.alt) modifiers |= MOD_ALT;
        if (hotkey.win) modifiers |= MOD_WIN;

        if (RegisterHotKey(hwnd, HOTKEY_ID, modifiers, hotkey.keyCode)) {
            m_hotkeyRegistered = true;
            qDebug() << "Hotkey registered successfully";
        } else {
            qWarning() << "Failed to register hotkey. Error:" << GetLastError();
            updateStatus("Warning: Could not register hotkey (may be in use by another application)");
        }
    } catch (const std::exception& e) {
        qCritical() << "Exception registering hotkey:" << e.what();
    } catch (...) {
        qCritical() << "Unknown exception registering hotkey";
    }
}

void InputWidget::unregisterWindowsHotkey() {
    try {
        if (m_hotkeyRegistered) {
            HWND hwnd = reinterpret_cast<HWND>(this->winId());
            if (UnregisterHotKey(hwnd, HOTKEY_ID)) {
                m_hotkeyRegistered = false;
                qDebug() << "Hotkey unregistered successfully";
            } else {
                qWarning() << "Failed to unregister hotkey. Error:" << GetLastError();
            }
        }
    } catch (const std::exception& e) {
        qCritical() << "Exception unregistering hotkey:" << e.what();
    } catch (...) {
        qCritical() << "Unknown exception unregistering hotkey";
    }
}

qint64 InputWidget::calculateTotalMs() const
{
    qint64 totalMs = 0;

    try {
        if (hours && !hours->text().trimmed().isEmpty()) {
            bool ok;
            qint64 h = hours->text().toLongLong(&ok);
            if (ok && h >= 0 && h <= 24) { // Max 24 hours
                totalMs += h * 3600 * 1000;
            }
        }

        if (mins && !mins->text().trimmed().isEmpty()) {
            bool ok;
            qint64 m = mins->text().toLongLong(&ok);
            if (ok && m >= 0 && m <= 59) {
                totalMs += m * 60 * 1000;
            }
        }

        if (secs && !secs->text().trimmed().isEmpty()) {
            bool ok;
            qint64 s = secs->text().toLongLong(&ok);
            if (ok && s >= 0 && s <= 59) {
                totalMs += s * 1000;
            }
        }

        if (ms && !ms->text().trimmed().isEmpty()) {
            bool ok;
            qint64 milliseconds = ms->text().toLongLong(&ok);
            if (ok && milliseconds >= 0 && milliseconds <= 999) {
                totalMs += milliseconds;
            }
        }
    } catch (const std::exception& e) {
        qWarning() << "Error calculating total milliseconds:" << e.what();
        return 0;
    } catch (...) {
        qWarning() << "Unknown error calculating total milliseconds";
        return 0;
    }

    return totalMs;
}

int InputWidget::validateClicksInput() const
{
    if (!clicks || clicks->text().trimmed().isEmpty()) {
        return -1; // Infinite clicks
    }

    try {
        bool ok;
        const int count = clicks->text().trimmed().toInt(&ok);
        if (ok && count > 0 && count <= 1000000) { // Max 1 million clicks
            return count;
        }
    } catch (...) {
        // Fall through to return -1
    }

    return -1; // Invalid or infinite
}

void InputWidget::updateStatus(const QString& message)
{
    if (!status) return;

    try {
        status->setText(message);

        // Set color based on message type
        QPalette palette = status->palette();
        if (message.startsWith("Error")) {
            palette.setColor(QPalette::WindowText, QColor(220, 53, 69)); // Bootstrap danger red
        } else if (message.startsWith("Warning")) {
            palette.setColor(QPalette::WindowText, QColor(255, 193, 7)); // Bootstrap warning yellow
        } else if (message.contains("started") || message.contains("completed")) {
            palette.setColor(QPalette::WindowText, QColor(40, 167, 69)); // Bootstrap success green
        } else {
            palette.setColor(QPalette::WindowText, Qt::white);
        }
        status->setPalette(palette);

        qDebug() << "Status updated:" << message;
    } catch (const std::exception& e) {
        qCritical() << "Error updating status:" << e.what();
    } catch (...) {
        qCritical() << "Unknown error updating status";
    }
}

// Helper methods for safer widget operations
void InputWidget::applyWidgetStyle(QWidget* widget, const QString& style) {
    if (widget) {
        try {
            widget->setStyleSheet(style);
        } catch (...) {
            qWarning() << "Failed to apply style to widget";
        }
    }
}

void InputWidget::setWidgetPlaceholder(QLineEdit* lineEdit, const QString& placeholder) {
    if (lineEdit) {
        try {
            lineEdit->setPlaceholderText(placeholder);
        } catch (...) {
            qWarning() << "Failed to set placeholder for line edit";
        }
    }
}

void InputWidget::setWidgetCursor(QWidget* widget, Qt::CursorShape cursor) {
    if (widget) {
        try {
            widget->setCursor(cursor);
        } catch (...) {
            qWarning() << "Failed to set cursor for widget";
        }
    }
}

void InputWidget::setupInputValidation() {
    // Add input validation for numeric fields
    try {
        if (hours) {
            connect(hours, &QLineEdit::textChanged, this, [this](const QString& text) {
                validateNumericInput(hours, text, 0, 24);
            });
        }

        if (mins) {
            connect(mins, &QLineEdit::textChanged, this, [this](const QString& text) {
                validateNumericInput(mins, text, 0, 59);
            });
        }

        if (secs) {
            connect(secs, &QLineEdit::textChanged, this, [this](const QString& text) {
                validateNumericInput(secs, text, 0, 59);
            });
        }

        if (ms) {
            connect(ms, &QLineEdit::textChanged, this, [this](const QString& text) {
                validateNumericInput(ms, text, 0, 999);
            });
        }

        if (clicks) {
            connect(clicks, &QLineEdit::textChanged, this, [this](const QString& text) {
                validateNumericInput(clicks, text, 0, 1000000);
            });
        }

        if (durationHours) {
            connect(durationHours, &QLineEdit::textChanged, this, [this](const QString& text) {
                validateNumericInput(durationHours, text, 0, 24);
            });
        }

        if (durationMins) {
            connect(durationMins, &QLineEdit::textChanged, this, [this](const QString& text) {
                validateNumericInput(durationMins, text, 0, 59);
            });
        }

        if (durationSecs) {
            connect(durationSecs, &QLineEdit::textChanged, this, [this](const QString& text) {
                validateNumericInput(durationSecs, text, 0, 59);
            });
        }
    } catch (const std::exception& e) {
        qWarning() << "Error setting up input validation:" << e.what();
    } catch (...) {
        qWarning() << "Unknown error setting up input validation";
    }
}

void InputWidget::validateNumericInput(QLineEdit* lineEdit, const QString& text, int min, int max) {
    if (!lineEdit || text.isEmpty()) return;

    try {
        bool ok;
        int value = text.toInt(&ok);

        QPalette palette = lineEdit->palette();
        if (!ok || value < min || value > max) {
            palette.setColor(QPalette::Text, Qt::red);
        } else {
            palette.setColor(QPalette::Text, QColor(221, 221, 221)); // Normal text color
        }
        lineEdit->setPalette(palette);
    } catch (...) {
        // Ignore validation errors
    }
}

bool InputWidget::isPositionValid(const QPoint& pos) const {
    try {
        QScreen* screen = QApplication::primaryScreen();
        if (!screen) return false;

        QRect screenGeometry = screen->geometry();
        return screenGeometry.contains(pos);
    } catch (...) {
        return false;
    }
}

qint64 InputWidget::calculateDurationMs() const
{
    qint64 totalMs = 0;

    if (durationHours && !durationHours->text().trimmed().isEmpty()) {
        bool ok;
        qint64 h = durationHours->text().toLongLong(&ok);
        if (ok && h >= 0) totalMs += h * 3600 * 1000;
    }

    if (durationMins && !durationMins->text().trimmed().isEmpty()) {
        bool ok;
        qint64 m = durationMins->text().toLongLong(&ok);
        if (ok && m >= 0) totalMs += m * 60 * 1000;
    }

    if (durationSecs && !durationSecs->text().trimmed().isEmpty()) {
        bool ok;
        qint64 s = durationSecs->text().toLongLong(&ok);
        if (ok && s >= 0) totalMs += s * 1000;
    }

    return totalMs;
}

void InputWidget::setupLayout() {
    try {
        // Create layouts with proper widget checks
        QHBoxLayout* inputLayout = new QHBoxLayout;
        if (hours && mins && secs && ms) {
            inputLayout->addWidget(hours, 1);
            inputLayout->addWidget(mins, 1);
            inputLayout->addWidget(secs, 1);
            inputLayout->addWidget(ms, 1);
            inputLayout->setSpacing(10);
        }

        QHBoxLayout* durationLayout = new QHBoxLayout;
        if (durationHours && durationMins && durationSecs) {
            durationLayout->addWidget(durationHours, 1);
            durationLayout->addWidget(durationMins, 1);
            durationLayout->addWidget(durationSecs, 1);
            durationLayout->setSpacing(10);
        }

        QHBoxLayout* checkboxLayout = new QHBoxLayout;
        QHBoxLayout* doubleClickLayout = new QHBoxLayout;
        if (doubleClickCheckbox && doubleClickLabel) {
            doubleClickLayout->addWidget(doubleClickCheckbox);
            doubleClickLayout->addWidget(doubleClickLabel);
            doubleClickLayout->addStretch();
        }

        QHBoxLayout* rightClickLayout = new QHBoxLayout;
        if (rightClickCheckbox && rightClickLabel) {
            rightClickLayout->addWidget(rightClickCheckbox);
            rightClickLayout->addWidget(rightClickLabel);
            rightClickLayout->addStretch();
        }

        checkboxLayout->addLayout(doubleClickLayout);
        checkboxLayout->addLayout(rightClickLayout);
        checkboxLayout->setSpacing(20);

        QHBoxLayout* posButsLayout = new QHBoxLayout;
        if (posSet && posPick) {
            posButsLayout->addWidget(posSet);
            posButsLayout->addWidget(posPick);
            posButsLayout->setSpacing(10);
        }

        QHBoxLayout* posInpLayout = new QHBoxLayout;
        if (posInp) {
            posInpLayout->addWidget(posInp, 1);
            posInpLayout->addLayout(posButsLayout);
            posInpLayout->setSpacing(10);
        }

        QHBoxLayout* bottomButLayout = new QHBoxLayout;
        if (clickBut) {
            bottomButLayout->addWidget(clickBut);
            bottomButLayout->setAlignment(Qt::AlignCenter);
        }

        QHBoxLayout* statusLayout = new QHBoxLayout;
        if (status) {
            statusLayout->addWidget(status);
            statusLayout->setAlignment(Qt::AlignCenter);
        }

        QHBoxLayout* bottomTextLayout = new QHBoxLayout;
        if (press && hotkeyBut) {
            bottomTextLayout->addWidget(press, 1);
            bottomTextLayout->addWidget(hotkeyBut);
            bottomTextLayout->setAlignment(Qt::AlignCenter);
            bottomTextLayout->setSpacing(10);
        }

        // Main layout
        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        mainLayout->setSpacing(15);

        if (interval) mainLayout->addWidget(interval);
        mainLayout->addLayout(inputLayout);

        if (durationLab) mainLayout->addWidget(durationLab);
        mainLayout->addLayout(durationLayout);

        if (clicksLab) mainLayout->addWidget(clicksLab);
        if (clicks) mainLayout->addWidget(clicks);

        mainLayout->addLayout(checkboxLayout);

        if (posLab) mainLayout->addWidget(posLab);
        mainLayout->addLayout(posInpLayout);

        mainLayout->addStretch(1);
        mainLayout->addLayout(bottomButLayout);
        mainLayout->addLayout(statusLayout);
        mainLayout->addLayout(bottomTextLayout);

        setLayout(mainLayout);

    } catch (const std::exception& e) {
        qCritical() << "Error setting up layout:" << e.what();
    } catch (...) {
        qCritical() << "Unknown error setting up layout";
    }
}
