#!/usr/bin/env zsh
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
GRAPH="$ROOT/chatrack.filtergraph"
WAV="$ROOT/resources/test-loop.wav"
HOST_PROJ="$ROOT/libs/juce/extras/AudioPluginHost/Builds/MacOSX/AudioPluginHost.xcodeproj"

# Xcode ignores SYMROOT for this extras project; the product lands next to the xcodeproj.
HOST_BINS=(
  "$ROOT/libs/juce/extras/AudioPluginHost/Builds/MacOSX/build/Debug/AudioPluginHost.app/Contents/MacOS/AudioPluginHost"
  "$ROOT/build/host/Debug/AudioPluginHost.app/Contents/MacOS/AudioPluginHost"
)

host_bin() {
  local c
  for c in "${HOST_BINS[@]}"; do
    [[ -x "$c" ]] && { echo "$c"; return 0; }
  done
  return 1
}

HOST_BIN="$(host_bin || true)"
if [[ -z "${HOST_BIN:-}" ]]; then
  echo "Building AudioPluginHost (first time)..."
  xcodebuild \
    -project "$HOST_PROJ" \
    -scheme "AudioPluginHost - App" \
    -configuration Debug \
    -quiet
  HOST_BIN="$(host_bin || true)"
fi

if [[ -z "${HOST_BIN:-}" || ! -x "$HOST_BIN" ]]; then
  echo "AudioPluginHost binary not found" >&2
  exit 1
fi

killall AudioPluginHost 2>/dev/null || true
sleep 0.4

echo "Launching AudioPluginHost"
echo "  graph: $GRAPH"
echo "  loop:  $WAV"
echo "If the file player is empty: load test-loop.wav and File → Save so it sticks."

exec "$HOST_BIN" "$GRAPH"
