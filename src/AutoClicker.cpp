#include "AutoClicker.h"
#include <QApplication>
#include <QDebug>
#include <QScreen>

// Windows-specific includes
#include <windows.h>
#pragma comment(lib, "user32.lib")

AutoClicker::AutoClicker(QObject* parent) : QObject(parent) {
    // Initialize member variables with safe defaults
    m_position = QPoint(-1, -1); // Use invalid position to indicate dynamic cursor
    m_remainingClicks = -1;
    m_doubleClick = false;
    m_rightClick = false;
    m_isRunning = false;
    m_useDynamicPosition = true; // Default to using cursor position

    // Configure timer with safer settings
    m_timer.setSingleShot(false);
    m_timer.setInterval(1000); // Default 1 second
    m_timer.setTimerType(Qt::PreciseTimer);

    // Use queued connection to prevent crashes during rapid start/stop
    connect(&m_timer, &QTimer::timeout, this, &AutoClicker::performClick, Qt::QueuedConnection);

    qDebug() << "AutoClicker initialized for Windows";
}

AutoClicker::~AutoClicker() {
    // Force stop with cleanup
    if (m_isRunning) {
        m_timer.stop();
        m_isRunning = false;
    }

    // Ensure timer is completely stopped
    if (m_timer.isActive()) {
        m_timer.stop();
    }

    qDebug() << "AutoClicker destroyed safely";
}

void AutoClicker::setInterval(qint64 ms) {
    // Conservative bounds checking (User Request: 5ms minimum)
    const int MIN_INTERVAL = 5;
    const int MAX_INTERVAL = 3600000; // 1 hour max

    // Ensure we don't overflow when casting
    if (ms > MAX_INTERVAL) ms = MAX_INTERVAL;
    if (ms < MIN_INTERVAL) ms = MIN_INTERVAL;

    int interval = static_cast<int>(ms);

    // Stop timer before changing interval
    bool wasActive = m_timer.isActive();
    if (wasActive) {
        m_timer.stop();
    }

    m_timer.setInterval(interval);

    // Restart if it was active
    if (wasActive && m_isRunning) {
        m_timer.start();
    }

    qDebug() << "Interval set to:" << interval << "ms";
}

void AutoClicker::setClickCount(int count) {
    m_remainingClicks = count;
    qDebug() << "Click count set to:" << (count < 0 ? "infinite" : QString::number(count));
}

void AutoClicker::setPosition(const QPoint& pos) {
    if (pos.x() == -1 && pos.y() == -1) {
        m_useDynamicPosition = true;
        m_position = QPoint(-1, -1); // Explicitly store -1, -1 for dynamic signal
        qDebug() << "Position set to: current cursor (dynamic)";
    } else {
        m_position = pos;
        m_useDynamicPosition = false;
        qDebug() << "Position set to:" << m_position;
    }
}

void AutoClicker::setDoubleClick(bool enabled) {
    m_doubleClick = enabled;
    qDebug() << "Double click:" << (enabled ? "enabled" : "disabled");
}

void AutoClicker::setRightClick(bool enabled) {
    m_rightClick = enabled;
    qDebug() << "Right click:" << (enabled ? "enabled" : "disabled");
}

bool AutoClicker::start() {
    if (m_isRunning) {
        qDebug() << "AutoClicker already running";
        return true;
    }

    // Start runtime timer
    if (m_duration > 0) {
        m_runtime.start();
    }

    // Enforce 5ms minimum
    if (m_timer.interval() < 5) {
        qWarning() << "Cannot start: Interval too low" << m_timer.interval();
        emit error("Click interval too fast (minimum 5ms)");
        return false;
    }

    // Dynamic vs Fixed Position Check (Fixing Lag & Multi-monitor support)
    if (m_useDynamicPosition) {
        // Dynamic: Ensure position is invalid to signal `performWindowsClick` to skip movement
        m_position = QPoint(-1, -1);
        qDebug() << "Dynamic position mode active: Clicking at live cursor location.";
    } else {
        // Validate using Virtual Screen metrics (Multi-monitor fix)
        int vScreenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int vScreenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
        int vScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int vScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

        if (m_position.x() < vScreenX || m_position.x() >= (vScreenX + vScreenWidth) ||
            m_position.y() < vScreenY || m_position.y() >= (vScreenY + vScreenHeight)) {
            qWarning() << "Cannot start: Position outside virtual screen bounds";
            emit error("Click position outside virtual screen");
            return false;
        }
        qDebug() << "Fixed position set to:" << m_position;
    }

    // Test Windows API access
    POINT pt;
    if (!GetCursorPos(&pt)) {
        DWORD lastError = GetLastError();
        qWarning() << "Cannot access Windows cursor API. Error:" << lastError;
        emit error(QString("Windows API access denied (Error: %1)").arg(lastError));
        return false;
    }

    // Set running flag before starting timer
    m_isRunning = true;

    // Start timer with error checking
    m_timer.start();

    // Verify timer actually started
    if (!m_timer.isActive()) {
        m_isRunning = false;
        emit error("Failed to start internal timer");
        return false;
    }

    qDebug() << "AutoClicker started successfully";
    emit started();
    return true;
}

void AutoClicker::stop() {
    if (!m_isRunning) {
        return;
    }

    // Force stop timer
    m_timer.stop();
    m_isRunning = false;

    // Process any pending events to ensure clean shutdown
    QApplication::processEvents();

    qDebug() << "AutoClicker stopped";
    emit stopped();
}

bool AutoClicker::isActive() const {
    return m_isRunning && m_timer.isActive();
}

void AutoClicker::setDuration(qint64 ms) {
    m_duration = ms;
    if (ms > 0) {
        qDebug() << "Duration set to:" << ms << "ms";
    } else {
        qDebug() << "No duration limit set";
    }
}

void AutoClicker::performClick() {
    // Critical: Check if we're still supposed to be running
    if (!m_isRunning || !m_timer.isActive()) {
        qDebug() << "performClick called but autoclicker is not active";
        return;
    }

    // Check duration limit
    if (m_duration > 0 && m_runtime.hasExpired(m_duration)) {
        stop();
        emit finished();
        return;
    }

    // Check click count limit
    if (m_remainingClicks == 0) {
        stop();
        emit finished();
        return;
    }

    // Use the position that was set (either fixed or the dynamic signal -1, -1)
    QPoint clickPos = m_position;

    qDebug() << "Performing click at:" << clickPos << "Remaining:" << m_remainingClicks;

    // Perform Windows click
    bool success = performWindowsClick(clickPos.x(), clickPos.y(), m_rightClick, m_doubleClick);

    if (!success) {
        qWarning() << "Windows click failed";
        stop();
        emit error("Click operation failed");
        return;
    }

    // Decrement remaining clicks
    if (m_remainingClicks > 0) {
        m_remainingClicks--;
    }

    emit clickPerformed(clickPos);
}

// LAG FIX IMPLEMENTATION
bool AutoClicker::performWindowsClick(int x, int y, bool rightClick, bool doubleClick) {
    // Dynamic detection: (-1, -1) is the signal to click where the mouse is now.
    bool isDynamic = (x == -1 && y == -1);

    POINT originalPos = {0, 0};

    if (!isDynamic) {
        // FIXED POSITION LOGIC: Move cursor

        // Validation using Virtual Screen metrics (Multi-monitor fix)
        int vScreenX = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int vScreenY = GetSystemMetrics(SM_YVIRTUALSCREEN);
        int vScreenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int vScreenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

        if (x < vScreenX || y < vScreenY ||
            x >= (vScreenX + vScreenWidth) || y >= (vScreenY + vScreenHeight)) {
            qWarning() << "Click coordinates out of virtual screen bounds:" << x << "," << y;
            return false;
        }

        // Save original cursor position
        if (!GetCursorPos(&originalPos)) {
            DWORD error = GetLastError();
            qWarning() << "Failed to get cursor position. Error:" << error;
            return false;
        }

        // Move cursor to target position
        if (!SetCursorPos(x, y)) {
            DWORD error = GetLastError();
            qWarning() << "Failed to set cursor position. Error:" << error;
            return false;
        }

        // Reduced delay after moving the cursor (LAG FIX)
        Sleep(1);
    }

    // Use modern SendInput instead of deprecated mouse_event
    INPUT inputs[4] = {}; // Max 4 for double-click
    int inputCount = 0;

    // Prepare mouse input events
    DWORD downFlag = rightClick ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN;
    DWORD upFlag = rightClick ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP;

    // First click down
    inputs[inputCount].type = INPUT_MOUSE;
    inputs[inputCount].mi.dwFlags = downFlag;
    inputCount++;

    // First click up
    inputs[inputCount].type = INPUT_MOUSE;
    inputs[inputCount].mi.dwFlags = upFlag;
    inputCount++;

    // Double click if requested
    if (doubleClick) {
        // Second click down
        inputs[inputCount].type = INPUT_MOUSE;
        inputs[inputCount].mi.dwFlags = downFlag;
        inputCount++;

        // Second click up
        inputs[inputCount].type = INPUT_MOUSE;
        inputs[inputCount].mi.dwFlags = upFlag;
        inputCount++;
    }

    // Send all input events
    // If isDynamic is true, this click happens at the current cursor position,
    // and no movement/restore is needed, fixing the lag.
    UINT result = SendInput(inputCount, inputs, sizeof(INPUT));
    if (result != inputCount) {
        DWORD error = GetLastError();
        qWarning() << "SendInput failed. Expected:" << inputCount << "Sent:" << result << "Error:" << error;

        // Restore cursor position ONLY if we moved it
        if (!isDynamic) {
            SetCursorPos(originalPos.x, originalPos.y);
        }
        return false;
    }

    // Restore cursor ONLY if we moved it (Fixed position)
    if (!isDynamic) {
        // Reduced delay before restoring cursor (LAG FIX)
        Sleep(1);
        // Restore original cursor position
        if (!SetCursorPos(originalPos.x, originalPos.y)) {
            qWarning() << "Failed to restore cursor position";
        }
    }

    qDebug() << "Click performed successfully at" << (isDynamic ? "live cursor" : QString::number(x) + "," + QString::number(y))
             << (rightClick ? "(right)" : "(left)")
             << (doubleClick ? "(double)" : "(single)");

    return true;
}
