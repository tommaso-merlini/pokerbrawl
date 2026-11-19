#!/usr/bin/env bash
# Compila e avvia PokerBrawl chiedendo la piattaforma.
set -e
cd "$(dirname "$0")"

case "$(uname -s)" in
  Darwin) detected=2 ;;
  *)      detected=1 ;;
esac

echo "Su che piattaforma vuoi avviare PokerBrawl?"
echo "  1) Linux"
echo "  2) macOS"
read -r -p "Scelta [${detected}]: " choice
choice=${choice:-$detected}

case "$choice" in
  1|linux|Linux)  platform=linux ;;
  2|mac|macos|macOS) platform=macos ;;
  *) echo "Scelta non valida: $choice"; exit 1 ;;
esac

if [ "$platform" = macos ] && [ ! -f raylib/build/raylib/libraylib.a ] \
   && ! brew --prefix raylib >/dev/null 2>&1; then
  echo "raylib non trovata. Installala con: brew install raylib"
  exit 1
fi

# Ricompila sempre, così il binario corrisponde alla piattaforma scelta.
rm -f game
make PLATFORM="$platform"
./game
