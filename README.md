# Aegleseeker

A DirectX 11 internal hook with ImGui interface for advanced client modifications. This project provides a DLL injection framework with a comprehensive feature set including hitbox extensions, motion blur, keystroke tracking, and more.

## Overview

Aegleseeker is an internal client modification framework that hooks into DirectX 11 applications. It provides:

- **DirectX 11 Integration**: Full D3D11 hook for frame-level access
- **ImGui Interface**: Modern, draggable UI for in-game menu
- **Advanced Features**:
  - Reach/Hitbox modifications
  - AutoSprint functionality
  - Motion Blur effects
  - Keystrokes display with CPS counter
  - Render information overlay
  - FullBright mode
  - Frame rate control (Unlock FPS)
  - Watermark display
  
## Requirements

### System
- Windows 10 or later (x64)
- DirectX 11 compatible GPU
- Visual Studio 2015+ or MinGW-w64

### Build Dependencies
- **CMake** 3.15 or later
- **Compiler**:
  - MSVC (Visual Studio 2015+) OR
  - GCC/MinGW (MinGW-w64 recommended)
- **Windows SDK** (for DirectX 11 headers)
- **Dependencies**:
  - DirectX 11 SDK libraries
  - ImGui (included)
  - MinHook (included)

## Building

### Using CMake

```bash
# Create build directory
mkdir build
cd build

# Configure project
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release
```

The compiled DLL will be located at: `build/bin/internal_hook.dll`

### Using PowerShell (Legacy)

```powershell
.\build.ps1
```

This generates the DLL in `build/` directory.

## Project Structure

```
AegleDLL/
├── dllmain.cpp              # Main DLL entry point and hook implementation
├── CMakeLists.txt           # CMake build configuration
├── build.ps1                # PowerShell build script
├── ImGui/                   # ImGui library
│   ├── imgui.cpp
│   ├── imgui_draw.cpp
│   ├── imgui_widgets.cpp
│   ├── imgui_tables.cpp
│   ├── backend/
│   │   ├── imgui_impl_dx11.cpp
│   │   └── imgui_impl_win32.cpp
│   └── [header files]
├── minhook/                 # MinHook library for API hooks
│   ├── hook.c
│   ├── buffer.c
│   ├── trampoline.c
│   ├── hde64.c
│   └── [header files]
└── build/                   # Build output directory
    └── internal_hook.dll
```

## Features

### Combat
- **Reach**: Modify entity interaction distance
- **Hitbox**: Extend target collision boxes
- **Smart detection**: Automatic animation-aware hitbox scaling

### Movement
- **AutoSprint**: Continuous sprint without interaction
- **FullBright**: Enhanced visibility in dark areas
- **Timer**: Frame time multiplier for speed modifications
- **Unlock FPS**: Remove frame rate limitations

### Visuals
- **Motion Blur**: GPU-accelerated blur effects with multiple modes
- **Render Info**: Real-time FPS and performance metrics
- **Keystrokes**: Display keyboard inputs with CPS counter
- **Watermark**: Customizable watermark overlay
- **ArrayList**: Module status display with smooth animations

### UI Features
- **Draggable Windows**: Move all UI elements with mouse
- **Tabbed Interface**: Organized settings by category
- **Smooth Animations**: Easing functions for visual polish
- **Real-time Updates**: Live value adjustments during gameplay

## Usage

1. **Compile** the project using CMake or PowerShell script
2. **Inject** `internal_hook.dll` into a DirectX 11 application
3. **Press INSERT** key to toggle the menu
4. **Configure** features via the tabbed interface
5. **Customize** window positions by dragging

### Keyboard Shortcuts
- **INSERT**: Toggle main menu
- **Drag windows**: Left mouse button when menu is open

## Configuration

All settings are stored in ImGui's `.ini` file for persistence. The menu provides real-time sliders and toggles for:

- Reach distance
- Hitbox scale
- Motion blur intensity and mode
- Animation timings
- Color customization
- Positioning offsets

## Technical Details

### DirectX 11 Hooking
The project hooks `IDXGISwapChain::Present()` and `ResizeBuffers()` to intercept rendering. ImGui is initialized with DX11 backend for native rendering without performance overhead.

### Input Handling
- Manual input system for compatibility
- Virtual key mapping to ImGui keys
- Character input for IME support
- Mouse and keyboard state tracking

### Animation System
- Easing functions (cubic, exponential, elastic)
- Smooth transitions with configurable duration
- Chroma color cycling effects
- Per-module state animations

## Compilation Flags

### Performance Optimization
- `-O2`: Optimization level 2
- `-march=x86-64`: Target x86-64 architecture
- `-m64`: 64-bit compilation

### Linking
- `-static`: Static C runtime linking
- `-static-libgcc`: Static GCC library linking
- `-static-libstdc++`: Static C++ standard library

## Dependencies

### External Libraries (Included)
- **ImGui**: Immediate-mode GUI library
- **MinHook**: Minimalist API hooking library

### System Libraries
- **d3d11.lib**: Direct3D 11
- **dxgi.lib**: DXGI for swap chain
- **d3dcompiler.lib**: HLSL shader compilation
- **dwmapi.lib**: Desktop Window Manager
- **user32.lib**: Windows API
- **gdi32.lib**: Graphics Device Interface
- **psapi.lib**: Process API

## Platform Support

- **OS**: Windows 10+ (x64)
- **Compilers**: MSVC 2015+, MinGW-w64
- **Architecture**: x86-64 only
- **Graphics**: DirectX 11

## Known Limitations

- x64 builds only (x86 not supported)
- DirectX 11 applications only
- Windows platform only
- Requires administrative privileges for injection
- Animation system tied to frame timing

## Building on Different Compilers

### Visual Studio
```bash
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

### MinGW-w64
```bash
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| DLL fails to load | Ensure target app is x64 and uses DirectX 11 |
| Menu doesn't appear | Press INSERT key, check if app has focus |
| Rendering artifacts | Update graphics drivers |
| Performance issues | Disable motion blur or reduce FPS limit |
| Build failures | Install Windows SDK and verify compiler setup |

## License

This project is licensed under the [MIT license](LICENSE)

## Disclaimer

This software is provided for educational purposes only. Unauthorized modification of third-party applications may violate terms of service or applicable laws. Users are responsible for compliance with applicable regulations and terms of service of the target application.

## Contributing

Contributions are welcome. Please ensure code follows the existing style and includes appropriate comments.

## Support

For issues, questions, or feature requests, please open an issue on the repository.

Discord: notvyzer

---

**Last Updated**: April 3, 2026  
**Status**: Active Development
