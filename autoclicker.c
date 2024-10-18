#include <stdlib.h>
#include <windows.h>

void clickMouse(int button, int doubleClick) {
    INPUT input = {0};
    input.type = INPUT_MOUSE;

    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    SendInput(1, &input, sizeof(INPUT));

    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));

    if (doubleClick) {
        SendInput(1, &input, sizeof(INPUT));
        SendInput(1, &input, sizeof(INPUT));
    }
}

void autoClick(int interval, int clicks, int button, int doubleClick) {
    for (int i = 0; i < clicks; i++) {
        clickMouse(button, doubleClick);
        Sleep(interval);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        return 1;
    }

    int interval = atoi(argv[1]);
    int clicks = atoi(argv[2]);
    int button = 1;
    int doubleClick = atoi(argv[4]);

    autoClick(interval, clicks, button, doubleClick);
    return 0;
}
