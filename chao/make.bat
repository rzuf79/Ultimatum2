
@echo off
setlocal

:: Usage: build.bat [build_type]
:: build_type: debug (default), release

if "%~1"=="" (
    set BUILD_TYPE=debug
) else (
    set BUILD_TYPE=%~1
)

if /I "%BUILD_TYPE%" NEQ "debug" if /I "%BUILD_TYPE%" NEQ "release" (
    echo Invalid build_type: %BUILD_TYPE%
    echo Usage: %~nx0 [build_type]
    echo   build_type: debug (default), release
    exit /b 1
)

if /I "%BUILD_TYPE%"=="debug" (
    set CFLAGS=-g -DDEBUG
    echo Building in DEBUG mode with debug symbols.
) else (
    set CFLAGS=-O2 -DNDEBUG
    echo Building in RELEASE mode with optimizations.
)

:: Check if gcc is available
where gcc >nul 2>nul
if errorlevel 1 (
    echo gcc is not installed or not in PATH. Please install MinGW and add gcc to PATH.
    exit /b 1
)

:: Set SDL2 paths
:: Adjust SDL2_DIR to your SDL2 installation path
set SDL2_DIR=C:\SDL2

set INCLUDE_FLAGS=-I"%SDL2_DIR%\include"
set LIB_FLAGS=-L"%SDL2_DIR%\lib" -lmingw32 -lSDL2main -lSDL2
set DEFINES=-DPLATFORM_SDL

:: Compile
gcc main.c %INCLUDE_FLAGS% %DEFINES% %CFLAGS% -o game.exe %LIB_FLAGS%

if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo Windows build completed: game.exe
