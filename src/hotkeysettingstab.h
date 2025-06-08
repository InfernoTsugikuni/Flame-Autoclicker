#ifndef HOTKEYSETTINGSTAB_H
#define HOTKEYSETTINGSTAB_H

#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QVector>
#include <QPair>

// Represents a hotkey combination with modifier keys and a main key
struct Hotkey {
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
    bool win = false;
    int keyCode = 0;  // Windows virtual key code
};

class HotkeySettingsTab : public QWidget {
    Q_OBJECT

public:
    explicit HotkeySettingsTab(QWidget *parent = nullptr);

    // Returns currently configured hotkey
    Hotkey getCurrentHotkey() const;

signals:
    // Signal emitted when hotkey is saved
    void hotkeySaved(const Hotkey &hotkey);

private slots:
    void onSaveClicked();

private:
    void setupModifiers();
    void setupMainKeys();
    void createHeaderSection(QVBoxLayout *mainLayout);
    void createModifierSection(QVBoxLayout *mainLayout);
    void createMainKeySection(QVBoxLayout *mainLayout);
    void createPreviewSection(QVBoxLayout *mainLayout);
    void createButtonSection(QVBoxLayout *mainLayout);
    void setDefaultValues();
    void updatePreview();
    void onResetClicked();

    // Modifier checkboxes
    QCheckBox *ctrlCheck;
    QCheckBox *shiftCheck;
    QCheckBox *altCheck;
    QCheckBox *winCheck;

    // Main key dropdown
    QComboBox *keyCombo;

    // Buttons
    QPushButton *saveButton;

    // Labels
    QLabel *previewLabel;
    QLabel *statusLabel;
    QPushButton *resetButton;

    // Stores key display names and their VK codes
    QVector<QPair<QString, int>> keyMap;
};

#endif // HOTKEYSETTINGSTAB_H
