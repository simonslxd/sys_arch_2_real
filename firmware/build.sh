#!/bin/bash
# Builds the micro:bit V2 firmware and writes build/MICROBIT.hex.
#
#   bash firmware/build.sh        (in WSL; from any folder)
#
# CODAL, the official micro:bit C++ runtime, is not part of this repository. The first run
# downloads it into build/codal/ (pinned: SAMPLES_TAG below and firmware/codal.json) and
# builds its stock sample once, which fetches the libraries. Every run then puts our sources
# into build/codal/source/ and builds them; our own files as C++20 (firmware/cxx20.cmake).
# To start again from scratch, delete build/codal/.
set -euo pipefail

SAMPLES_TAG=v0.3.0   # github.com/lancaster-university/microbit-v2-samples
REPO="$(cd "$(dirname "$0")/.." && pwd)"
CODAL="$REPO/build/codal"

# 1. CODAL and its libraries, once.
if [ ! -d "$CODAL/libraries" ]; then
    rm -rf "$CODAL"
    git clone --depth 1 --branch "$SAMPLES_TAG" \
        https://github.com/lancaster-university/microbit-v2-samples.git "$CODAL"
    cp "$REPO/firmware/codal.json" "$CODAL/codal.json"
    (cd "$CODAL" && python3 build.py)
fi

# 2. Our sources, flat in source/ with the domain headers in source/labutlaan/, so that
#    #include "labutlaan/lab.hpp" works exactly as in the PC build.
rm -rf "$CODAL/source"
mkdir -p "$CODAL/source/labutlaan"
cp "$REPO"/firmware/device/*.cpp "$REPO"/firmware/device/*.hpp "$CODAL/source/"
cp "$REPO"/firmware/station/*.cpp "$REPO"/firmware/station/*.hpp "$CODAL/source/"
cp "$REPO"/project/impl/src/{lab,lab_utstyr,student,utlaan}.cpp "$CODAL/source/"
cp "$REPO"/project/impl/include/labutlaan/*.hpp "$CODAL/source/labutlaan/"

# 3. Build, and put the result where it is easy to find.
mkdir -p "$CODAL/build"
cd "$CODAL/build"
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_PROJECT_INCLUDE="$REPO/firmware/cxx20.cmake" > /dev/null
ninja
cp "$CODAL/MICROBIT.hex" "$REPO/build/MICROBIT.hex"
echo "Firmware: build/MICROBIT.hex. Copy it onto the MICROBIT drive to flash the micro:bit."
