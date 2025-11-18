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
    // Conservative bounds checking
    const int MIN_INTERVAL = 5;  // 5ms minimum to prevent system overload
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
    if (pos.isNull() || (pos.x() < 0 && pos.y() < 0)) {
        m_useDynamicPosition = true;
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

    if (m_timer.interval() < 5) {
        qWarning() << "Cannot start: Interval too low" << m_timer.interval();
        emit error("Click interval too fast (minimum 5ms)");
        return false;
    }

    // m_position shall remain (-1, -1) if we are using dynamic positioning
    if (m_useDynamicPosition) {
        m_position = QPoint(-1, -1);
        qDebug() << "Dynamic position active: Clicking at cursor location";
    } else {
        // Validate screen bounds only if we have a fixed position
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        if (m_position.x() >= screenWidth || m_position.y() >= screenHeight) {
            qWarning() << "Cannot start: Position outside screen bounds";
            emit error("Click position outside screen");
            return false;
        }
    }

    // Validate screen bounds
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (m_position.x() >= screenWidth || m_position.y() >= screenHeight) {
        qWarning() << "Cannot start: Position outside screen bounds";
        emit error("Click position outside screen");
        return false;
    }

    // Test Windows API access with better error checking
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

    // Use the position that was set (either fixed or captured at start)
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

bool AutoClicker::performWindowsClick(int x, int y, bool rightClick, bool doubleClick) {

    // If x and y are -1, we click EXACTLY where the mouse is, without moving it.
    bool isDynamicClick = (x == -1 && y == -1);

    // Only validate bounds if we are moving the mouse
    if (!isDynamicClick) {
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        if (x < 0 || y < 0 || x >= screenWidth || y >= screenHeight) {
            qWarning() << "Click coordinates out of screen bounds:" << x << "," << y;
            return false;
        }
    }

    POINT originalPos;
    // Only save original position if we intend to move the mouse
    if (!isDynamicClick) {
        if (!GetCursorPos(&originalPos)) {
            return false;
        }

        // Move cursor to target position
        if (!SetCursorPos(x, y)) {
            return false;
        }

        // Small delay ONLY if we moved the mouse (reduces lag for dynamic clicks)
        Sleep(1);
    }

    // Prepare mouse input events
    INPUT inputs[4] = {};
    int inputCount = 0;

    DWORD downFlag = rightClick ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN;
    DWORD upFlag = rightClick ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP;

    // First click
    inputs[inputCount].type = INPUT_MOUSE;
    inputs[inputCount].mi.dwFlags = downFlag;
    inputCount++;

    inputs[inputCount].type = INPUT_MOUSE;
    inputs[inputCount].mi.dwFlags = upFlag;
    inputCount++;

    // Double click if requested
    if (doubleClick) {
        inputs[inputCount].type = INPUT_MOUSE;
        inputs[inputCount].mi.dwFlags = downFlag;
        inputCount++;

        inputs[inputCount].type = INPUT_MOUSE;
        inputs[inputCount].mi.dwFlags = upFlag;
        inputCount++;
    }

    UINT result = SendInput(inputCount, inputs, sizeof(INPUT));

    // Only restore cursor if we moved it
    if (!isDynamicClick) {
        // Small delay before restoring
        Sleep(1); // Reduced from 25 to 1 to help speed
        SetCursorPos(originalPos.x, originalPos.y);
    }

    if (result != inputCount) {
        return false;
    }

    return true;
}
