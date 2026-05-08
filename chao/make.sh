#!/bin/bash

# Exit immediately if a command exits with a non-zero status
set -e

# Usage function
usage() {
    echo "Usage: $0 [platform] [build_type]"
    echo "  platform: mac, linux, windows"
    echo "  build_type: debug (default), release"
    echo "Examples:"
    echo "  $0 mac"
    echo "  $0 linux release"
    exit 1
}

# Check if at least one argument is provided
if [ $# -lt 1 ] || [ $# -gt 2 ]; then
    usage
fi

TARGET_PLATFORM="$1"
BUILD_TYPE="${2:-debug}"  # Default to debug if not provided

# Validate build type
if [[ "$BUILD_TYPE" != "debug" && "$BUILD_TYPE" != "release" ]]; then
    echo "Invalid build_type: $BUILD_TYPE"
    usage
fi

# Common variables
SOURCES="main.c"
APP_NAME="The Warlocker"
VERSION="1.0"

# Define compiler flags based on build type
if [ "$BUILD_TYPE" == "debug" ]; then
    CFLAGS="-g -DDEBUG"
    echo "Building in DEBUG mode with debug symbols."
elif [ "$BUILD_TYPE" == "release" ]; then
    CFLAGS="-O2 -DNDEBUG"
    echo "Building in RELEASE mode with optimizations."
fi

# Function to build for macOS
build_mac() {
    echo "Building for macOS..."

    # Set macOS-specific compilation flags
    SDL2="-I/Library/Frameworks/SDL2.framework/Headers"
    FRAMEWORKS="SDL2"
    DEFINES="-DPLATFORM_SDL"
    RPATH="-Wl,-rpath,/Library/Frameworks"
    OUTPUT="game"

    # Compile
    gcc $SOURCES -g $SDL2 $DEFINES $CFLAGS $RPATH -F/Library/Frameworks -framework $FRAMEWORKS -o "$OUTPUT"

#     # Bundle into .app only for macOS
#     APP_BUNDLE="${APP_NAME}_mac.app"
#     mkdir -p "${APP_BUNDLE}/Contents/MacOS"
#     mkdir -p "${APP_BUNDLE}/Contents/Resources"

#     cp "$OUTPUT" "${APP_BUNDLE}/Contents/MacOS/"
#     cp -r assets "${APP_BUNDLE}/Contents/Resources/"

#     # Create Info.plist
#     cat <<EOF > "${APP_BUNDLE}/Contents/Info.plist"
# <?xml version="1.0" encoding="UTF-8"?>
# <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
# <plist version="1.0">
# <dict>
#     <key>CFBundleExecutable</key>
#     <string>$(basename "$OUTPUT")</string>
#     <key>CFBundleIdentifier</key>
#     <string>com.yourcompany.${APP_NAME}</string>
#     <key>CFBundleName</key>
#     <string>${APP_NAME}</string>
#     <key>CFBundleShortVersionString</key>
#     <string>${VERSION}</string>
#     <key>CFBundleVersion</key>
#     <string>1</string>
#     <key>LSApplicationCategoryType</key>
#     <string>public.app-category.games</string>
# </dict>
# </plist>
# EOF

#     # Make the executable executable
#     chmod +x "${APP_BUNDLE}/Contents/MacOS/$(basename "$OUTPUT")"

#     echo "macOS build and bundling completed: ${APP_BUNDLE}"
}

# Function to build for Linux
build_linux() {
    echo "Building for Linux..."

    # Set Linux-specific compilation flags
    SDL2=$(sdl2-config --cflags)
    LDFLAGS="$(sdl2-config --libs)"
    DEFINES="-DPLATFORM_SDL"
    OUTPUT="game"

    # Compile
    gcc $SOURCES $SDL2 $DEFINES $CFLAGS -lm -o "$OUTPUT" $LDFLAGS

    echo "Linux build completed: $OUTPUT"
}

# Function to build for Windows using MinGW
build_windows() {
    echo "Building for Windows..."

    # Check if MinGW is installed
    if ! command -v x86_64-w64-mingw32-gcc &> /dev/null; then
        echo "MinGW-w64 is not installed. Please install it to build for Windows."
        exit 1
    fi

    # Set Windows-specific compilation flags
    SDL2_DIR="/c/SDL2"          # Adjust this path based on your SDL2 installation
    CFLAGS_WIN="-I$SDL2_DIR/include $CFLAGS"
    LDFLAGS_WIN="-L$SDL2_DIR/lib -lmingw32 -lSDL2main -lSDL2"
    DEFINES_WIN="-DPLATFORM_SDL"
    OUTPUT="game.exe"

    # Compile
    x86_64-w64-mingw32-gcc $SOURCES $CFLAGS_WIN $DEFINES_WIN -o "$OUTPUT" $LDFLAGS_WIN

    echo "Windows build completed: $OUTPUT"
}

# Main build logic
case "$TARGET_PLATFORM" in
    mac)
        build_mac
        ;;
    linux)
        build_linux
        ;;
    windows)
        build_windows
        ;;
    *)
        echo "Unsupported platform: $TARGET_PLATFORM"
        usage
        ;;
esac
