#!/usr/bin/env bash

set -e

CONFIG_FILE=".vscode/arduino.json"

if [ ! -f "$CONFIG_FILE" ]; then
  echo "arduino.json not found"
  exit 1
fi

FQBN_BASE=$(grep '"board"' "$CONFIG_FILE" | sed -E 's/.*"board": *"([^"]+)".*/\1/')
CONFIG=$(grep '"configuration"' "$CONFIG_FILE" | sed -E 's/.*"configuration": *"([^"]+)".*/\1/')
SKETCH=$(grep '"sketch"' "$CONFIG_FILE" | sed -E 's/.*"sketch": *"([^"]+)".*/\1/')

if [ -z "$FQBN_BASE" ]; then
  echo "board not found"
  exit 1
fi

if [ -z "$SKETCH" ]; then
  echo "sketch not found"
  exit 1
fi

if [ -n "$CONFIG" ]; then
  FQBN="$FQBN_BASE:$CONFIG"
else
  FQBN="$FQBN_BASE"
fi

SKETCH_DIR=$(dirname "$SKETCH")

mkdir -p build

arduino-cli compile --fqbn "$FQBN" "$SKETCH_DIR" --build-path build
