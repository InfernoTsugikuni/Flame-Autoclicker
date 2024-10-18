# LICENSE
Read `LICENSE` for more information. This software is licensed under the MIT License.

# Auto Clicker

This is a simple Auto Clicker application for Windows built with Python and C. It features a GUI for easy interaction and allows users to automate mouse clicks at customizable intervals.

## Features

- Set a custom click interval (in milliseconds).
- Specify the number of clicks (0 for infinite).
- Toggle double-clicking functionality.
- Easily set the position of the mouse cursor for clicking.
- Start and stop clicking with a hotkey (F6).
- Built-in GUI using the CustomTkinter library.

## Instructions
1. **Open `autoclicker_gui.exe`.**
2. **Fill out whatever you need and press F6.**

 ## Building from scratch
First download this repository as a .zip file.

 **Requirements**

- Python 3.x
- GCC (C compiler for Windows)
  
You can install the required packages via pip:

```bash
@echo off
SETLOCAL

REM Name of your virtual enviroment
SET VENV=myenv

IF NOT EXIST %VENV% (
    python -m venv %VENV%
)

CALL %VENV%\Scripts\activate.bat

pip install -r requirements.txt

IF ERRORLEVEL 1 (
    echo Compilation failed.
    pause
    exit /b 1
)

python autoclicker_gui.py

pause
ENDLOCAL
```
Save this as a .bat file and run it. This is only for using the .py file if your AntiVirus detects the prebuilt binary as malware because of it being compiled using PyInstaller. For compiling the autoclicker.c file use this command:

```bash
gcc autoclicker.c -o autoclicker.exe
```
