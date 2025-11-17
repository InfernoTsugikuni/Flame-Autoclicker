#include "hotkeysettingstab.h"
#include <windows.h>
#include <QLabel>
#include <QFrame>
#include <QApplication>
#include <QGraphicsDropShadowEffect>

HotkeySettingsTab::HotkeySettingsTab(QWidget *parent)
    : QWidget(parent)
{
    // Adjusted size for better proportions
    setFixedSize(650, 500);

    QWidget *background = new QWidget(this);
    background->setStyleSheet("background-color: #1e1e1e;");
    background->setGeometry(0, 0, width(), height());
    background->lower();

    // Apply modern dark theme styling to the entire widget
    setStyleSheet(R"(
        HotkeySettingsTab {
            background-color: #1e1e1e;
            border-radius: 0px;
        }

        QWidget {
            background-color: #1e1e1e;
            color: #ffffff;
            font-family: 'Segoe UI', Arial, sans-serif;
        }

        QGroupBox {
            font-size: 13px;
            font-weight: bold;
            color: #ffffff;
            border: 2px solid #3a3a3a;
            border-radius: 10px;
            margin: 8px 0px;
            padding-top: 15px;
            background-color: #2a2a2a;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 15px;
            background-color: #1e1e1e;
            border-radius: 6px;
            color: #ff6b00;
        }

        QCheckBox {
            font-size: 13px;
            font-weight: 500;
            color: #e0e0e0;
            spacing: 8px;
            padding: 8px;
        }

        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 4px;
            border: 2px solid #555555;
            background-color: #2a2a2a;
        }

        QCheckBox::indicator:hover {
            border-color: #ff6b00;
            background-color: #3a3a3a;
        }

        QCheckBox::indicator:checked {
            border-color: #ff6b00;
            background-color: #ff6b00;
            image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTIiIGhlaWdodD0iOSIgdmlld0JveD0iMCAwIDEyIDkiIGZpbGw9Im5vbmUiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+CjxwYXRoIGQ9Ik0xMC42IDEuNEw0LjMgNy43TDEuNCA0LjgiIHN0cm9rZT0id2hpdGUiIHN0cm9rZS13aWR0aD0iMiIgc3Ryb2tlLWxpbmVjYXA9InJvdW5kIiBzdHJva2UtbGluZWpvaW49InJvdW5kIi8+Cjwvc3ZnPgo=);
        }

        QComboBox {
            font-size: 13px;
            font-weight: 500;
            color: #ffffff;
            background-color: #2a2a2a;
            border: 2px solid #555555;
            border-radius: 8px;
            padding: 10px 12px;
            min-height: 18px;
        }

        QComboBox:focus {
            border-color: #ff6b00;
            background-color: #3a3a3a;
        }

        QComboBox::drop-down {
            border: none;
            width: 25px;
        }

        QComboBox::down-arrow {
            image: url(data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTIiIGhlaWdodD0iOCIgdmlld0JveD0iMCAwIDEyIDgiIGZpbGw9Im5vbmUiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+CjxwYXRoIGQ9Ik0xIDFMNiA2TDExIDEiIHN0cm9rZT0iI2ZmNmIwMCIgc3Ryb2tlLXdpZHRoPSIyIiBzdHJva2UtbGluZWNhcD0icm91bmQiIHN0cm9rZS1saW5lam9pbj0icm91bmQiLz4KPC9zdmc+Cg==);
            width: 12px;
            height: 8px;
        }

        QComboBox QAbstractItemView {
            background-color: #2a2a2a;
            border: 2px solid #ff6b00;
            border-radius: 8px;
            selection-background-color: #ff6b00;
            selection-color: #ffffff;
            color: #ffffff;
            font-size: 13px;
            padding: 4px;
        }

        QPushButton {
            font-size: 14px;
            font-weight: bold;
            color: #ffffff;
            background-color: #ff6b00;
            border: none;
            border-radius: 8px;
            padding: 12px 25px;
            min-height: 18px;
        }

        QPushButton:hover {
            background-color: #e65c00;
        }

        QPushButton:pressed {
            background-color: #cc5200;
        }

        QPushButton:disabled {
            background-color: #666666;
            color: #999999;
        }

        QLabel {
            color: #cccccc;
            font-size: 13px;
        }

        QFrame {
            background-color: #2a2a2a;
            border-radius: 8px;
        }
    )");

    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(12);

    // Header section with title and description
    createHeaderSection(mainLayout);

    // Create a horizontal layout for the main content
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(15);

    // Left side - Modifier keys and main key
    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(12);
    createModifierSection(leftLayout);
    createMainKeySection(leftLayout);
    leftLayout->addStretch(1);

    // Right side - Preview and buttons
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(12);
    createPreviewSection(rightLayout);
    rightLayout->addStretch(1);
    createButtonSection(rightLayout);

    contentLayout->addLayout(leftLayout, 2);
    contentLayout->addLayout(rightLayout, 1);

    mainLayout->addLayout(contentLayout);
    mainLayout->addStretch(1);

    // Connect signals
    connect(saveButton, &QPushButton::clicked, this, &HotkeySettingsTab::onSaveClicked);
    connect(resetButton, &QPushButton::clicked, this, &HotkeySettingsTab::onResetClicked);

    // Connect modifier checkboxes to update preview
    connect(ctrlCheck, &QCheckBox::toggled, this, &HotkeySettingsTab::updatePreview);
    connect(shiftCheck, &QCheckBox::toggled, this, &HotkeySettingsTab::updatePreview);
    connect(altCheck, &QCheckBox::toggled, this, &HotkeySettingsTab::updatePreview);
    connect(winCheck, &QCheckBox::toggled, this, &HotkeySettingsTab::updatePreview);
    connect(keyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &HotkeySettingsTab::updatePreview);

    // Set default values and update preview
    setDefaultValues();
    updatePreview();
}

void HotkeySettingsTab::createHeaderSection(QVBoxLayout *mainLayout) {
    // Title
    QLabel *titleLabel = new QLabel("Hotkey Configuration");
    titleLabel->setStyleSheet(R"(
        QLabel {
            font-size: 20px;
            font-weight: bold;
            color: #ff6b00;
            margin-bottom: 3px;
        }
    )");
    titleLabel->setAlignment(Qt::AlignCenter);

    // Description
    QLabel *descLabel = new QLabel("Configure the global hotkey to start/stop the autoclicker");
    descLabel->setStyleSheet(R"(
        QLabel {
            font-size: 12px;
            color: #aaaaaa;
            line-height: 1.3;
            margin-bottom: 10px;
        }
    )");
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(descLabel);

    // Add separator line
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("QFrame { background-color: #3a3a3a; max-height: 1px; margin: 3px 0 10px 0; }");
    mainLayout->addWidget(separator);
}

void HotkeySettingsTab::createModifierSection(QVBoxLayout *mainLayout) {
    QGroupBox *modGroup = new QGroupBox("Modifier Keys");

    // Create a grid layout for better organization
    QGridLayout *modLayout = new QGridLayout();
    modLayout->setSpacing(10);
    modLayout->setContentsMargins(15, 18, 15, 15);

    setupModifiers();

    // Add modifier checkboxes in a 2x2 grid
    modLayout->addWidget(ctrlCheck, 0, 0);
    modLayout->addWidget(shiftCheck, 0, 1);
    modLayout->addWidget(altCheck, 1, 0);
    modLayout->addWidget(winCheck, 1, 1);

    modGroup->setLayout(modLayout);
    mainLayout->addWidget(modGroup);
}

void HotkeySettingsTab::createMainKeySection(QVBoxLayout *mainLayout) {
    QGroupBox *keyGroup = new QGroupBox("Main Key");
    QVBoxLayout *keyLayout = new QVBoxLayout();
    keyLayout->setContentsMargins(15, 18, 15, 15);
    keyLayout->setSpacing(10);

    // Key selection combo with better styling
    keyCombo = new QComboBox();
    keyCombo->setMinimumHeight(35);
    keyCombo->setCursor(Qt::PointingHandCursor);
    setupMainKeys();

    // Add description
    QLabel *keyHelpText = new QLabel("Choose the main key for the combination");
    keyHelpText->setStyleSheet("QLabel { color: #888888; font-size: 11px; font-style: italic; }");

    keyLayout->addWidget(keyCombo);
    keyLayout->addWidget(keyHelpText);

    keyGroup->setLayout(keyLayout);
    mainLayout->addWidget(keyGroup);
}

void HotkeySettingsTab::createPreviewSection(QVBoxLayout *mainLayout) {
    QGroupBox *previewGroup = new QGroupBox("Preview");
    QVBoxLayout *previewLayout = new QVBoxLayout();
    previewLayout->setContentsMargins(15, 18, 15, 15);
    previewLayout->setSpacing(8);

    // Preview display
    previewLabel = new QLabel("Ctrl + Alt + Print Screen");
    previewLabel->setStyleSheet(R"(
        QLabel {
            font-size: 16px;
            font-weight: bold;
            color: #ff6b00;
            background-color: #2a2a2a;
            border: 2px solid #3a3a3a;
            border-radius: 8px;
            padding: 15px;
            qproperty-alignment: AlignCenter;
            min-height: 30px;
        }
    )");

    // Status label
    statusLabel = new QLabel("✓ Valid hotkey combination");
    statusLabel->setStyleSheet("QLabel { color: #4CAF50; font-size: 11px; margin-top: 5px; }");
    statusLabel->setAlignment(Qt::AlignCenter);

    previewLayout->addWidget(previewLabel);
    previewLayout->addWidget(statusLabel);

    previewGroup->setLayout(previewLayout);
    mainLayout->addWidget(previewGroup);
}

void HotkeySettingsTab::createButtonSection(QVBoxLayout *mainLayout) {
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setSpacing(8);
    buttonLayout->setAlignment(Qt::AlignCenter);

    // Save button
    saveButton = new QPushButton("Apply Hotkey");
    saveButton->setMinimumHeight(35);
    saveButton->setCursor(Qt::PointingHandCursor);
    saveButton->setMinimumWidth(180);

    // Reset button
    resetButton = new QPushButton("Reset to Default");
    resetButton->setStyleSheet(R"(
        QPushButton {
            background-color: #333333;
            color: #ffffff;
            font-size: 12px;
            font-weight: normal;
            padding: 10px 20px;
            min-height: 18px;
            border: 1px solid #555555;
        }
        QPushButton:hover {
            background-color: #3a3a3a;
            border-color: #ff6b00;
        }
        QPushButton:pressed {
            background-color: #2a2a2a;
            border-color: #cc5200;
        }
    )");
    resetButton->setMinimumHeight(32);
    resetButton->setCursor(Qt::PointingHandCursor);
    resetButton->setMinimumWidth(180);

    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(resetButton);

    mainLayout->addLayout(buttonLayout);
}

void HotkeySettingsTab::setupModifiers() {
    ctrlCheck = new QCheckBox("Ctrl");
    shiftCheck = new QCheckBox("Shift");
    altCheck = new QCheckBox("Alt");
    winCheck = new QCheckBox("Win");

    // Set cursor for all checkboxes
    ctrlCheck->setCursor(Qt::PointingHandCursor);
    shiftCheck->setCursor(Qt::PointingHandCursor);
    altCheck->setCursor(Qt::PointingHandCursor);
    winCheck->setCursor(Qt::PointingHandCursor);

    // Add tooltips for better UX
    ctrlCheck->setToolTip("Control key modifier");
    shiftCheck->setToolTip("Shift key modifier");
    altCheck->setToolTip("Alt key modifier");
    winCheck->setToolTip("Windows key modifier");
}

void HotkeySettingsTab::setupMainKeys() {
    // Enhanced key mapping with better organization
    keyMap = {
        // Letters
        {"A", 'A'}, {"B", 'B'}, {"C", 'C'}, {"D", 'D'}, {"E", 'E'},
        {"F", 'F'}, {"G", 'G'}, {"H", 'H'}, {"I", 'I'}, {"J", 'J'},
        {"K", 'K'}, {"L", 'L'}, {"M", 'M'}, {"N", 'N'}, {"O", 'O'},
        {"P", 'P'}, {"Q", 'Q'}, {"R", 'R'}, {"S", 'S'}, {"T", 'T'},
        {"U", 'U'}, {"V", 'V'}, {"W", 'W'}, {"X", 'X'}, {"Y", 'Y'}, {"Z", 'Z'},

        // Numbers
        {"0", '0'}, {"1", '1'}, {"2", '2'}, {"3", '3'}, {"4", '4'},
        {"5", '5'}, {"6", '6'}, {"7", '7'}, {"8", '8'}, {"9", '9'},

        // Function Keys
        {"F1", VK_F1}, {"F2", VK_F2}, {"F3", VK_F3}, {"F4", VK_F4},
        {"F5", VK_F5}, {"F6", VK_F6}, {"F7", VK_F7}, {"F8", VK_F8},
        {"F9", VK_F9}, {"F10", VK_F10}, {"F11", VK_F11}, {"F12", VK_F12},

        // Special Keys
        {"Print Screen", VK_SNAPSHOT}, {"Insert", VK_INSERT},
        {"Delete", VK_DELETE}, {"Home", VK_HOME}, {"End", VK_END},
        {"Page Up", VK_PRIOR}, {"Page Down", VK_NEXT},

        // Arrow Keys
        {"← Left Arrow", VK_LEFT}, {"→ Right Arrow", VK_RIGHT},
        {"↑ Up Arrow", VK_UP}, {"↓ Down Arrow", VK_DOWN},

        // Other Keys
        {"Escape", VK_ESCAPE}, {"Enter", VK_RETURN}, {"Space", VK_SPACE},
        {"Backspace", VK_BACK}, {"Tab", VK_TAB}, {"Caps Lock", VK_CAPITAL}
    };

    // Populate combo box with organized sections
    for (const auto &pair : keyMap) {
        keyCombo->addItem(pair.first);
    }

    // Set F6 as default
    for (int i = 0; i < keyCombo->count(); ++i) {
        // MODIFICATION: Changed "Print Screen" to "F6"
        if (keyCombo->itemText(i) == "F6") {
            keyCombo->setCurrentIndex(i);
            break;
        }
    }
}

void HotkeySettingsTab::setDefaultValues() {
    // MODIFICATION: Set all modifiers to false by default for F6
    ctrlCheck->setChecked(false);
    altCheck->setChecked(false);
    shiftCheck->setChecked(false);
    winCheck->setChecked(false);
}

void HotkeySettingsTab::updatePreview() {
    QStringList parts;
    if (ctrlCheck->isChecked()) parts << "Ctrl";
    if (shiftCheck->isChecked()) parts << "Shift";
    if (altCheck->isChecked()) parts << "Alt";
    if (winCheck->isChecked()) parts << "Win";

    if (keyCombo->currentIndex() >= 0) {
        parts << keyCombo->currentText();
    }

    QString hotkeyText = parts.join(" + ");

    // MODIFICATION: Relaxed validation. Any selected main key is valid.
    if (hotkeyText.isEmpty()) {
        hotkeyText = "No main key selected";
        statusLabel->setText("⚠ Please select a main key");
        statusLabel->setStyleSheet("QLabel { color: #FFC107; font-size: 11px; }");
    } else {
        // Any combination (even a single key) is now considered valid
        statusLabel->setText("✓ Valid hotkey combination");
        statusLabel->setStyleSheet("QLabel { color: #4CAF50; font-size: 11px; }");
    }

    previewLabel->setText(hotkeyText);
}

Hotkey HotkeySettingsTab::getCurrentHotkey() const {
    Hotkey hk;
    hk.ctrl = ctrlCheck->isChecked();
    hk.shift = shiftCheck->isChecked();
    hk.alt = altCheck->isChecked();
    hk.win = winCheck->isChecked();

    // Get selected key code
    if (keyCombo->currentIndex() >= 0) {
        hk.keyCode = keyMap[keyCombo->currentIndex()].second;
    }

    return hk;
}

void HotkeySettingsTab::onSaveClicked() {
    Hotkey hk = getCurrentHotkey();

    // MODIFICATION: Removed modifier check. Only check if a main key is selected.
    if (hk.keyCode == 0) {
        statusLabel->setText("⚠ Please select a main key");
        statusLabel->setStyleSheet("QLabel { color: #dc3545; font-size: 11px; }");
        return;
    }

    // Construct readable string for logging
    QStringList parts;
    if (hk.ctrl) parts << "Ctrl";
    if (hk.shift) parts << "Shift";
    if (hk.alt) parts << "Alt";
    if (hk.win) parts << "Win";
    parts << keyCombo->currentText();

    qDebug() << "Saved Hotkey:" << parts.join(" + ");
    emit hotkeySaved(hk);
}

void HotkeySettingsTab::onResetClicked() {
    // MODIFICATION: Also need to set the key back to F6 on reset
    setDefaultValues();
    for (int i = 0; i < keyCombo->count(); ++i) {
        if (keyCombo->itemText(i) == "F6") {
            keyCombo->setCurrentIndex(i);
            break;
        }
    }
    updatePreview();
}
