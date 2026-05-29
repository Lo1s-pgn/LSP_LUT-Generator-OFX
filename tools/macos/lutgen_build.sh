#!/usr/bin/env bash
# macOS build helper — configure + build (release/LSP_Simple_LUT_Generator_<version>_macos/).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"
cmake -S . -B build/macos -DCMAKE_BUILD_TYPE=Release "$@"
cmake --build build/macos --target lutgen_all --parallel "$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"
