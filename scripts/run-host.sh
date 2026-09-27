#!/usr/bin/env zsh
# Build Debug AU + VST3, then open AudioPluginHost with the saved graph + test loop.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"

cmake --build "$ROOT/build" --target Chatrack_AU --config Debug
cmake --build "$ROOT/build" --target Chatrack_VST3 --config Debug

exec "$ROOT/scripts/launch-host.sh"
