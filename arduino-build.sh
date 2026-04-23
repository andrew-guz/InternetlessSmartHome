#!/usr/bin/env bash

set -e

CONFIG_FILE=".vscode/arduino.json"

if [ ! -f "$CONFIG_FILE" ]; then
  echo "arduino.json not found"
  exit 1
fi

# fqbn
FQBN=$(grep '"board"' "$CONFIG_FILE" | sed -E 's/.*"board": *"([^"]+)".*/\1/')

# путь к ino
SKETCH=$(grep '"sketch"' "$CONFIG_FILE" | sed -E 's/.*"sketch": *"([^"]+)".*/\1/')

if [ -z "$FQBN" ]; then
  echo "board not found"
  exit 1
fi

if [ -z "$SKETCH" ]; then
  echo "sketch not found"
  exit 1
fi

# получаем папку скетча
SKETCH_DIR=$(dirname "$SKETCH")

arduino-cli compile --fqbn "$FQBN" "$SKETCH_DIR" --build-path build
