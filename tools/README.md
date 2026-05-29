# Tools

Optional helpers for building the plug-in. **CMake** is the only build system (see root [README.md](../README.md)).

Each platform build writes:

`release/LSP_Simple_LUT_Generator_<version>_macos/` or `_windows/`

containing **`LSP_Simple_LUT_Generator_<version>.ofx.bundle`**.

## Build

| Script | Platform | Purpose |
|--------|----------|---------|
| [macos/lutgen_build.sh](macos/lutgen_build.sh) | macOS | Configure `build/macos/` → versioned macOS release folder |
| [windows/lutgen_build.bat](windows/lutgen_build.bat) | Windows | Configure `build/windows/` → versioned Windows release folder |

Equivalent manual commands:

```bash
# macOS
cmake -S . -B build/macos -DCMAKE_BUILD_TYPE=Release
cmake --build build/macos --target lutgen_all
```

```powershell
# Windows
cmake -S . -B build/windows -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
cmake --build build/windows --target lutgen_all
```

Install the `.ofx.bundle` from your platform’s release folder — see root README **Installation**.
