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
#include <QIntValidator> // Use QIntValidator
#include <windows.h>

// Includes for classes forward-declared in header
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>

namespace {
// Helper function to get key names
QString getKeyName(int keyCode) {
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

    if (keyCode >= VK_F1 && keyCode <= VK_F24) {
        return "F" + QString::number(keyCode - VK_F1 + 1);
    }

    UINT scanCode = MapVirtualKeyW(keyCode, MAPVK_VK_TO_VSC);
    if (scanCode == 0) return "Key " + QString::number(keyCode);

    wchar_t name[256];
    LONG lParam = scanCode << 16;
    if (GetKeyNameTextW(lParam, name, 256) > 0) {
        return QString::fromWCharArray(name).trimmed();
    }
    lParam = (scanCode << 16) | (1 << 24);
    if (GetKeyNameTextW(lParam, name, 256) > 0) {
        return QString::fromWCharArray(name).trimmed();
    }
    return "Key " + QString::number(keyCode);
}

// Helper function to format the hotkey string
QString hotkeyString(const Hotkey& hotkey) {
    QStringList parts;
    if (hotkey.ctrl) parts << "Ctrl";
    if (hotkey.shift) parts << "Shift";
    if (hotkey.alt) parts << "Alt";
    if (hotkey.win) parts << "Win";
    parts << getKeyName(hotkey.keyCode);
    return parts.join(" + ");
}
} // namespace


MainContent::MainContent(QWidget* parent)
    : QWidget(parent)
    , m_currentHotkey({false, false, false, false, VK_F6}) // Default: F6
    , m_isActive(false)
    , m_hotkeyRegistered(false)
    , m_targetPos(0, 0)
{
    setupUi();
    setupStyles();
    setupValidators();
    setupConnections();

    if (press) {
        press->setText("Press " + hotkeyString(m_currentHotkey) + " to start/stop clicking");
    }

    registerWindowsHotkey(m_currentHotkey);
    qDebug() << "MainContent initialized successfully";
}

MainContent::~MainContent() {
    qDebug() << "MainContent destructor called";
    unregisterWindowsHotkey();

    if (m_autoclicker.isActive()) {
        m_autoclicker.stop();
        qDebug() << "Autoclicker stopped in destructor";
    }
    qDebug() << "MainContent destroyed safely";
}

void MainContent::setupUi() {
    // Create widgets
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
    posClear = new QPushButton("Clear/Dynamic", this); // <--- NEW BUTTON
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

    // Set placeholders
    setWidgetPlaceholder(hours, "Hours");
    setWidgetPlaceholder(mins, "Minutes");
    setWidgetPlaceholder(secs, "Seconds");
    setWidgetPlaceholder(ms, "Milliseconds");
    setWidgetPlaceholder(clicks, "0");
    setWidgetPlaceholder(posInp, "e.g., 100, 200");
    setWidgetPlaceholder(durationHours, "Hours");
    setWidgetPlaceholder(durationMins, "Minutes");
    setWidgetPlaceholder(durationSecs, "Seconds");

    // Initialize ms text to 5 by default (User Request)
    ms->setText("5");

    // Set cursors
    setWidgetCursor(rightClickCheckbox, Qt::PointingHandCursor);
    setWidgetCursor(doubleClickCheckbox, Qt::PointingHandCursor);
    setWidgetCursor(posSet, Qt::PointingHandCursor);
    setWidgetCursor(posPick, Qt::PointingHandCursor);
    setWidgetCursor(posClear, Qt::PointingHandCursor); // <--- NEW CURSOR
    setWidgetCursor(clickBut, Qt::PointingHandCursor);
    setWidgetCursor(hotkeyBut, Qt::PointingHandCursor);

    // Create layouts
    QHBoxLayout* inputLayout = new QHBoxLayout;
    inputLayout->addWidget(hours, 1);
    inputLayout->addWidget(mins, 1);
    inputLayout->addWidget(secs, 1);
    inputLayout->addWidget(ms, 1);
    inputLayout->setSpacing(10);

    QHBoxLayout* durationLayout = new QHBoxLayout;
    durationLayout->addWidget(durationHours, 1);
    durationLayout->addWidget(durationMins, 1);
    durationLayout->addWidget(durationSecs, 1);
    durationLayout->setSpacing(10);

    QHBoxLayout* checkboxLayout = new QHBoxLayout;
    QHBoxLayout* doubleClickLayout = new QHBoxLayout;
    doubleClickLayout->addWidget(doubleClickCheckbox);
    doubleClickLayout->addWidget(doubleClickLabel);
    doubleClickLayout->addStretch();

    QHBoxLayout* rightClickLayout = new QHBoxLayout;
    rightClickLayout->addWidget(rightClickCheckbox);
    rightClickLayout->addWidget(rightClickLabel);
    rightClickLayout->addStretch();

    checkboxLayout->addLayout(doubleClickLayout);
    checkboxLayout->addLayout(rightClickLayout);
    checkboxLayout->setSpacing(20);

    QHBoxLayout* posButsLayout = new QHBoxLayout;
    posButsLayout->addWidget(posSet);
    posButsLayout->addWidget(posPick);
    posButsLayout->addWidget(posClear); // <--- ADDED BUTTON
    posButsLayout->setSpacing(10);

    QHBoxLayout* posInpLayout = new QHBoxLayout;
    posInpLayout->addWidget(posInp, 1);
    posInpLayout->addLayout(posButsLayout);
    posInpLayout->setSpacing(10);

    QHBoxLayout* bottomButLayout = new QHBoxLayout;
    bottomButLayout->addWidget(clickBut);
    bottomButLayout->setAlignment(Qt::AlignCenter);

    QHBoxLayout* statusLayout = new QHBoxLayout;
    statusLayout->addWidget(status);
    statusLayout->setAlignment(Qt::AlignCenter);

    QHBoxLayout* bottomTextLayout = new QHBoxLayout;
    bottomTextLayout->addWidget(press, 1);
    bottomTextLayout->addWidget(hotkeyBut);
    bottomTextLayout->setAlignment(Qt::AlignCenter);
    bottomTextLayout->setSpacing(10);

    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    mainLayout->addWidget(interval);
    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(durationLab);
    mainLayout->addLayout(durationLayout);
    mainLayout->addWidget(clicksLab);
    mainLayout->addWidget(clicks);
    mainLayout->addLayout(checkboxLayout);
    mainLayout->addWidget(posLab);
    mainLayout->addLayout(posInpLayout);
    mainLayout->addStretch(1);
    mainLayout->addLayout(bottomButLayout);
    mainLayout->addLayout(statusLayout);
    mainLayout->addLayout(bottomTextLayout);

    setLayout(mainLayout);
}

void MainContent::setupStyles() {
    const QString inputStyle = R"(
        QLineEdit {
            padding: 9px; border: 1px solid #555; border-radius: 6px;
            background-color: #2d2d2d; color: #ddd; font-size: 14px;
        }
        QLineEdit:focus {
            border-color: #ff6b00; background-color: #3a3a3a; color: #fff;
        }
    )";

    const QString buttonStyle = R"(
        QPushButton {
            padding: 10px; border-radius: 6px; background-color: #2d2d2d;
            color: #fff; font-weight: bold; font-size: 14px; border: 1px solid #555;
        }
        QPushButton:hover {
            background-color: #ff6b00; border-color: #ff6b00; color: #fff;
        }
    )";

    // Apply buttonStyle to the new clear button
    QString secondaryButtonStyle = buttonStyle;
    secondaryButtonStyle.replace("#ff6b00", "#555"); // Use a neutral color for secondary buttons

    // Store button styles as members
    m_startButtonStyle = R"(
        QPushButton {
            padding: 12px; border-radius: 8px; background-color: #ff6b00;
            color: #fff; font-weight: bold; font-size: 16px; border: none;
        }
        QPushButton:hover { background-color: #e65c00; }
        QPushButton:disabled { background-color: #666; color: #999; }
    )";

    m_stopButtonStyle = m_startButtonStyle;
    m_stopButtonStyle.replace("#ff6b00", "#dc3545"); // Red for stop
    m_stopButtonStyle.replace("#e65c00", "#c82333"); // Darker red for hover

    const QString sectionLabelStyle = "font-weight: bold; color: #bbb; font-size: 16px;";
    const QString statusLabelStyle = "color: #aaa; font-size: 14px;";

    const QString checkboxStyle = R"(
        QCheckBox::indicator {
            width: 16px; height: 16px; border-radius: 10px;
            border: 2px solid #555; background-color: #2d2d2d;
        }
        QCheckBox::indicator:checked {
            border: 2px solid #ff6b00; background-color: #ff6b00;
        }
        QCheckBox::indicator:unchecked:hover { border-color: #aaaaaa; }
    )";

    // Apply styles
    applyWidgetStyle(hours, inputStyle);
    applyWidgetStyle(mins, inputStyle);
    applyWidgetStyle(secs, inputStyle);
    applyWidgetStyle(ms, inputStyle);
    applyWidgetStyle(clicks, inputStyle);
    applyWidgetStyle(posInp, inputStyle);
    applyWidgetStyle(durationHours, inputStyle);
    applyWidgetStyle(durationMins, inputStyle);
    applyWidgetStyle(durationSecs, inputStyle);
    applyWidgetStyle(doubleClickCheckbox, checkboxStyle);
    applyWidgetStyle(rightClickCheckbox, checkboxStyle);
    applyWidgetStyle(posSet, secondaryButtonStyle);
    applyWidgetStyle(posPick, secondaryButtonStyle);
    applyWidgetStyle(posClear, secondaryButtonStyle);
    applyWidgetStyle(clickBut, m_startButtonStyle);
    applyWidgetStyle(hotkeyBut, secondaryButtonStyle);
    applyWidgetStyle(interval, sectionLabelStyle);
    applyWidgetStyle(clicksLab, sectionLabelStyle);
    applyWidgetStyle(durationLab, sectionLabelStyle);
    applyWidgetStyle(posLab, sectionLabelStyle);
    applyWidgetStyle(doubleClickLabel, "color: #bbb; font-size: 12px;");
    applyWidgetStyle(rightClickLabel, "color: #bbb; font-size: 12px;");
    applyWidgetStyle(status, statusLabelStyle);
    applyWidgetStyle(press, statusLabelStyle);
}

void MainContent::setupValidators() {
    // Use QIntValidator for automatic input validation
    if (hours) hours->setValidator(new QIntValidator(0, 24, this));
    if (mins) mins->setValidator(new QIntValidator(0, 59, this));
    if (secs) secs->setValidator(new QIntValidator(0, 59, this));
    if (ms) ms->setValidator(new QIntValidator(0, 999, this));
    if (clicks) clicks->setValidator(new QIntValidator(0, 1000000, this));
    if (durationHours) durationHours->setValidator(new QIntValidator(0, 24, this));
    if (durationMins) durationMins->setValidator(new QIntValidator(0, 59, this));
    if (durationSecs) durationSecs->setValidator(new QIntValidator(0, 59, this));
}

void MainContent::setupConnections() {
    connect(&m_autoclicker, &AutoClicker::started, this, [this]() {
        updateStatus("Autoclicking started");
    });
    connect(&m_autoclicker, &AutoClicker::stopped, this, [this]() {
        updateStatus("Autoclicking stopped");
        m_isActive = false;
        if (clickBut) {
            clickBut->setText("Start Clicking");
            applyWidgetStyle(clickBut, m_startButtonStyle);
        }
    });
    connect(&m_autoclicker, &AutoClicker::finished, this, [this]() {
        updateStatus("Autoclicking completed");
        m_isActive = false;
        if (clickBut) {
            clickBut->setText("Start Clicking");
            applyWidgetStyle(clickBut, m_startButtonStyle);
        }
    });
    connect(&m_autoclicker, &AutoClicker::error, this, [this](const QString& error) {
        updateStatus("Error: " + error);
        m_isActive = false;
        if (clickBut) {
            clickBut->setText("Start Clicking");
            applyWidgetStyle(clickBut, m_startButtonStyle);
        }
    });

    // Connect button signals
    if (clickBut) connect(clickBut, &QPushButton::clicked, this, &MainContent::toggleAutoclicker);
    if (hotkeyBut) connect(hotkeyBut, &QPushButton::clicked, this, &MainContent::updateHotkey);
    if (posSet) connect(posSet, &QPushButton::clicked, this, &MainContent::setPositionFromInput);
    if (posPick) connect(posPick, &QPushButton::clicked, this, &MainContent::pickPositionFromCursor);
    if (posClear) connect(posClear, &QPushButton::clicked, this, &MainContent::clearPosition); // <--- NEW CONNECTION
}

bool MainContent::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
    if (eventType == "windows_generic_MSG") {
        MSG* msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY && msg->wParam == HOTKEY_ID) {
            toggleAutoclicker();
            return true;
        }
    }
    return QWidget::nativeEvent(eventType, message, result);
}

void MainContent::toggleAutoclicker() {
    qDebug() << "toggleAutoclicker called, isActive:" << m_isActive;

    if (!m_isActive) {
        startAutoclicker();
    } else {
        stopAutoclicker();
    }
}

bool MainContent::startAutoclicker() {
    setWindowTitle("Clicking - FlameAutoclicker");

    qint64 intervalMs = calculateTotalMs();
    const int clickCount = validateClicksInput();
    const qint64 durationMs = calculateDurationMs();

    // The minimum interval check is handled in calculateTotalMs() and AutoClicker::start()

    if (clickCount <= 0 && durationMs <= 0) {
        updateStatus("Info: Clicking will continue until stopped");
    }

    m_autoclicker.setInterval(intervalMs);
    m_autoclicker.setClickCount(clickCount);
    m_autoclicker.setDuration(durationMs);
    m_autoclicker.setPosition(m_targetPos); // Passes (-1, -1) for dynamic mode

    if (doubleClickCheckbox) {
        m_autoclicker.setDoubleClick(doubleClickCheckbox->isChecked());
    }
    if (rightClickCheckbox) {
        m_autoclicker.setRightClick(rightClickCheckbox->isChecked());
    }

    // m_autoclicker.setUseDynamicPosition is set in setPositionFromInput/clearPosition

    if (m_autoclicker.start()) {
        m_isActive = true;
        updateStatus("Autoclicking started successfully");
        if (clickBut) {
            clickBut->setText("Stop Clicking");
            applyWidgetStyle(clickBut, m_stopButtonStyle);
        }
        return true;
    } else {
        updateStatus("Error: Failed to start autoclicker");
        return false;
    }
}

void MainContent::stopAutoclicker() {
    m_autoclicker.stop();
    m_isActive = false;
    updateStatus("Autoclicking stopped");
    setWindowTitle("FlameAutoclicker");
    if (clickBut) {
        clickBut->setText("Start Clicking");
        applyWidgetStyle(clickBut, m_startButtonStyle);
    }
    qDebug() << "Autoclicker stopped successfully";
}

void MainContent::setPositionFromInput() {
    if (!posInp) {
        updateStatus("Error: Position input not available");
        return;
    }

    // If input is empty, default to dynamic clicking
    if (posInp->text().trimmed().isEmpty()) {
        clearPosition();
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
    // Multi-monitor support means coordinates can be negative, so don't check x < 0 || y < 0 here
    const int x = parts[0].trimmed().toInt(&xOk);
    const int y = parts[1].trimmed().toInt(&yOk);

    if (!xOk || !yOk) {
        updateStatus("Error: Coordinates must be valid numbers");
        return;
    }

    QPoint newPos(x, y);

    // Use Qt's virtualGeometry to check against all screens
    if (!isPositionValid(newPos)) {
        updateStatus("Warning: Position may be outside the virtual screen bounds (all monitors)");
    }

    m_targetPos = newPos;
    updateStatus(QString("Position set to: %1, %2").arg(x).arg(y));
    qDebug() << "Position set manually to:" << m_targetPos;

    // Turn dynamic mode OFF when a fixed position is set
    m_autoclicker.setUseDynamicPosition(false);
}

// NEW FUNCTION: Clears position and sets to dynamic mode
void MainContent::clearPosition() {
    if (m_autoclicker.isActive()) {
        updateStatus("Warning: Cannot change position while running. Stop first.");
        return;
    }

    // Clear the visual input
    if (posInp) {
        posInp->clear();
        setWidgetPlaceholder(posInp, "Current Cursor Position (Dynamic)");
    }

    // Set internal state to dynamic mode
    m_targetPos = QPoint(-1, -1); // Signal for dynamic mode
    m_autoclicker.setUseDynamicPosition(true);

    updateStatus("Click position cleared. Using **Current Cursor Position** (Dynamic).");
}

void MainContent::pickPositionFromCursor() {
    if (!posPick || !status) {
        updateStatus("Error: Position picker not available");
        return;
    }

    posPick->setText("Click anywhere...");
    posPick->setDisabled(true);
    updateStatus("Move cursor to desired position and wait 3 seconds...");

    QTimer* pickTimer = new QTimer(this);
    pickTimer->setSingleShot(true);

    connect(pickTimer, &QTimer::timeout, this, [this, pickTimer]() {
        QPoint cursorPos = QCursor::pos();

        if (!cursorPos.isNull()) {
            m_targetPos = cursorPos;

            if (posInp) {
                posInp->setText(QString("%1, %2").arg(m_targetPos.x()).arg(m_targetPos.y()));
                setWidgetPlaceholder(posInp, "e.g., 100, 200");
            }

            m_autoclicker.setUseDynamicPosition(false);

            updateStatus(QString("Position picked: %1, %2").arg(cursorPos.x()).arg(cursorPos.y()));
            qDebug() << "Position picked:" << m_targetPos;
        } else {
            updateStatus("Error: Invalid cursor position detected");
        }

        if (posPick) {
            posPick->setText("Pick Position");
            posPick->setDisabled(false);
        }
        pickTimer->deleteLater();
    });

    pickTimer->start(3000); // 3 second delay
}

void MainContent::updateHotkey() {
    // Create and show the HotkeySettingsWindow
    HotkeySettingsWindow *settingsWindow = new HotkeySettingsWindow(this);
    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(settingsWindow, &HotkeySettingsWindow::hotkeySaved,
            this, &MainContent::onHotkeySaved);

    settingsWindow->show();
    settingsWindow->raise();
    settingsWindow->activateWindow();
}

void MainContent::onHotkeySaved(const Hotkey &hotkey) {
    unregisterWindowsHotkey();
    m_currentHotkey = hotkey;
    registerWindowsHotkey(hotkey);

    if (press) {
        press->setText("Press " + hotkeyString(m_currentHotkey) + " to start/stop clicking");
    }
    updateStatus("Hotkey changed successfully");
}

void MainContent::registerWindowsHotkey(const Hotkey &hotkey) {
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
        updateStatus("Warning: Could not register hotkey (may be in use)");
    }
}

void MainContent::unregisterWindowsHotkey() {
    if (m_hotkeyRegistered) {
        HWND hwnd = reinterpret_cast<HWND>(this->winId());
        if (UnregisterHotKey(hwnd, HOTKEY_ID)) {
            m_hotkeyRegistered = false;
            qDebug() << "Hotkey unregistered successfully";
        } else {
            qWarning() << "Failed to unregister hotkey. Error:" << GetLastError();
        }
    }
}

// MODIFIED: Enforce 5ms floor
qint64 MainContent::calculateTotalMs() const {
    qint64 totalMs = 0;
    if (hours) totalMs += hours->text().toLongLong() * 3600 * 1000;
    if (mins) totalMs += mins->text().toLongLong() * 60 * 1000;
    if (secs) totalMs += secs->text().toLongLong() * 1000;

    long long msValue = 0;
    if (ms) {
        // Read ms value, default to 0 if empty
        msValue = ms->text().toLongLong();
    }
    totalMs += msValue;

    // ENFORCE 5MS MINIMUM
    if (totalMs < 5) {
        return 5;
    }

    return totalMs;
}

qint64 MainContent::calculateDurationMs() const {
    qint64 totalMs = 0;
    if (durationHours) totalMs += durationHours->text().toLongLong() * 3600 * 1000;
    if (durationMins) totalMs += durationMins->text().toLongLong() * 60 * 1000;
    if (durationSecs) totalMs += durationSecs->text().toLongLong() * 1000;
    return totalMs;
}

int MainContent::validateClicksInput() const {
    if (!clicks || clicks->text().trimmed().isEmpty()) {
        return -1; // Infinite clicks
    }

    bool ok;
    int count = clicks->text().trimmed().toInt(&ok);
    return (ok && count > 0) ? count : -1;
}

void MainContent::updateStatus(const QString& message) {
    if (!status) return;

    status->setText(message);

    QPalette palette = status->palette();
    if (message.startsWith("Error")) {
        palette.setColor(QPalette::WindowText, QColor(220, 53, 69)); // Red
    } else if (message.startsWith("Warning")) {
        palette.setColor(QPalette::WindowText, QColor(255, 193, 7)); // Yellow
    } else if (message.contains("started") || message.contains("completed")) {
        palette.setColor(QPalette::WindowText, QColor(40, 167, 69)); // Green
    } else {
        palette.setColor(QPalette::WindowText, QColor(170, 170, 170)); // Default #aaa
    }
    status->setPalette(palette);

    qDebug() << "Status updated:" << message;
}

// Helper methods for safer widget operations
void MainContent::applyWidgetStyle(QWidget* widget, const QString& style) {
    if (widget) {
        widget->setStyleSheet(style);
    }
}

void MainContent::setWidgetPlaceholder(QLineEdit* lineEdit, const QString& placeholder) {
    if (lineEdit) {
        lineEdit->setPlaceholderText(placeholder);
    }
}

void MainContent::setWidgetCursor(QWidget* widget, Qt::CursorShape cursor) {
    if (widget) {
        widget->setCursor(cursor);
    }
}

// MODIFIED: Use virtualGeometry to check against all monitors (multi-monitor support)
bool MainContent::isPositionValid(const QPoint& pos) const {
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) return false;

    // Use virtualGeometry() which spans all connected screens
    return QApplication::primaryScreen()->virtualGeometry().contains(pos);
}
