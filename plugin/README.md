# Plugin sources

Portable OFX plug-in logic and platform UI.

| Path | Role |
|------|------|
| **`core/`** | Describe, plugin instance, CPU processor, LUT pattern, `.cube` writer, logging |
| **`LSPLutGeneratorDialogs.h`** | Shared folder-picker API |
| **`macos/LSPLutGeneratorDialogs.mm`** | AppKit `NSOpenPanel` (main thread) |
| **`windows/LSPLutGeneratorDialogs.cpp`** | Windows `IFileOpenDialog` folder picker |

Version constants are generated from **`VERSION`** at CMake configure time.

Build: root [README.md](../README.md). Shipped artifact: **`LSP_Simple_LUT_Generator_<triplet>.ofx.bundle`** inside **`release/LSP_Simple_LUT_Generator_<triplet>_macos/`** or **`_windows/`**.
