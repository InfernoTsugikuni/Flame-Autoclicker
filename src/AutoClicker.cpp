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
    const int MIN_INTERVAL = 5;  // 10ms minimum to prevent system overload
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

    if (m_timer.interval() < 10) {
        qWarning() << "Cannot start: Interval too low" << m_timer.interval();
        emit error("Click interval too fast (minimum 10ms)");
        return false;
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

    // REMOVED: try-catch block

    QPoint clickPos;
    if (m_useDynamicPosition) {
        // Get current cursor position
        POINT pt;
        if (!GetCursorPos(&pt)) {
            DWORD error = GetLastError();
            qWarning() << "Failed to get cursor position. Error:" << error;
            stop();
            emit this->error("Failed to get cursor position");
            return;
        }
        clickPos = QPoint(pt.x, pt.y);
    } else {
        clickPos = m_position;
    }

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
    // REMOVED: try-catch block

    // Validate coordinates first
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (x < 0 || y < 0 || x >= screenWidth || y >= screenHeight) {
        qWarning() << "Click coordinates out of screen bounds:" << x << "," << y;
        return false;
    }

    // Save original cursor position
    POINT originalPos;
    if (!GetCursorPos(&originalPos)) {
        DWORD error = GetLastError();
        qWarning() << "Failed to get cursor position. Error:" << error;
        return false;
    }

    // Use modern SendInput instead of deprecated mouse_event
    INPUT inputs[4] = {}; // Max 4 for double-click
    int inputCount = 0;

    // Move cursor to target position first
    if (!SetCursorPos(x, y)) {
        DWORD error = GetLastError();
        qWarning() << "Failed to set cursor position. Error:" << error;
        return false;
    }

    // Small delay to ensure cursor movement
    Sleep(10);

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
    UINT result = SendInput(inputCount, inputs, sizeof(INPUT));
    if (result != inputCount) {
        DWORD error = GetLastError();
        qWarning() << "SendInput failed. Expected:" << inputCount << "Sent:" << result << "Error:" << error;

        // Try to restore cursor position even if click failed
        SetCursorPos(originalPos.x, originalPos.y);
        return false;
    }

    // Small delay before restoring cursor
    Sleep(25);

    // Restore original cursor position
    if (!SetCursorPos(originalPos.x, originalPos.y)) {
        qWarning() << "Failed to restore cursor position";
        // Don't return false here as the click was successful
    }

    qDebug() << "Click performed successfully at" << x << "," << y
             << (rightClick ? "(right)" : "(left)")
             << (doubleClick ? "(double)" : "(single)");

    return true;
}
