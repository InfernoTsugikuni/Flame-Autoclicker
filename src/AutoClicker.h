#ifndef AUTOCLICKER_H
#define AUTOCLICKER_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QPoint>

class AutoClicker : public QObject {
    Q_OBJECT

public:
    explicit AutoClicker(QObject* parent = nullptr);
    ~AutoClicker();

    // Configuration methods
    void setInterval(qint64 ms);
    void setClickCount(int count);
    void setPosition(const QPoint& pos);
    void setDoubleClick(bool enabled);
    void setRightClick(bool enabled);
    bool useDynamicPosition() const { return m_useDynamicPosition; }

    // Control methods
    bool start();
    void stop();
    bool isActive() const;

    // Getters
    QPoint position() const { return m_position; }
    int interval() const { return m_timer.interval(); }
    int remainingClicks() const { return m_remainingClicks; }
    bool isDoubleClick() const { return m_doubleClick; }
    bool isRightClick() const { return m_rightClick; }

    void setDuration(qint64 ms);

signals:
    void started();
    void stopped();
    void finished();
    void clickPerformed(const QPoint& position);
    void error(const QString& message);

private slots:
    void performClick();

private:
    // Windows-specific clicking method
    bool performWindowsClick(int x, int y, bool rightClick, bool doubleClick);

    QTimer m_timer;
    QPoint m_position;
    int m_remainingClicks = -1;
    bool m_doubleClick = false;
    bool m_rightClick = false;
    bool m_isRunning = false;
    bool m_useDynamicPosition = true;

    qint64 m_duration = -1;
    QElapsedTimer m_runtime;
};

#endif // AUTOCLICKER_H
