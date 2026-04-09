# Aegleseeker
  
A DirectX 11 internal hook with an ImGui interface for runtime client modification. This project provides a modular DLL payload with in-game feature toggles, a command terminal, and JSON-based config persistence.

<p align="center">
  <img src="docs/preview.png" width="700"/>
</p>

<p align="center">
  <img src="https://img.shields.io/github/license/iVyz3r/aegledll" alt="License" />
  <img src="https://img.shields.io/github/stars/iVyz3r/aegledll" alt="Stars" />
  <img src="https://img.shields.io/github/issues/iVyz3r/aegledll" alt="Issues" />
  <img src="https://img.shields.io/github/last-commit/iVyz3r/aegledll" alt="Last Commit" />
  <img src="https://img.shields.io/github/topics/iVyz3r/aegledll?label=topics" alt="Topics" />
</p>

## Overview

Aegleseeker is an internal client modification framework that hooks into DirectX 11 applications. It provides:

- **DirectX 11 Integration**: Full D3D11 hook for frame-level access
- **ImGui Interface**: Modern, draggable UI for in-game menu
- **Modular Architecture**: Well-organized feature modules (Combat, Movement, Visuals, Misc)
- **Centralized Animation System**: Unified easing functions and animation utilities
- **Advanced Features**:
  - Reach/Hitbox modifications
  - AutoSprint functionality
  - Motion Blur effects
  - Keystrokes display with CPS counter
  - Render information overlay
  - FullBright mode
  - Frame rate control (Unlock FPS)
  - Watermark display
  - Timer modifications
  
## Requirements

### System
- Windows 10 or later (x64)
- DirectX 11 compatible GPU
- Visual Studio 2015+ or MinGW-w64

### Build Dependencies
- **Compiler**:
  - MSVC (Visual Studio 2015+) OR
  - GCC/MinGW (MinGW-w64 recommended)
- **Windows SDK** (for DirectX 11 headers)
- **Dependencies**:
  - DirectX 11 SDK libraries
  - ImGui (included)
  - MinHook (included)

## Building

### Using PowerShell (recomended)

```powershell
.\build.ps1
```

This generates the DLL in `build/` directory.

## Project Structure

```
│   .gitignore                  # Files to ignore in git
│   build.ps1                   # PowerShell build script
│   dllmain.cpp                 # DLL entry and hook setup
│   README.md                   # Project documentation
│   
├───Animations
│       Animations.cpp          # Easing function implementations
│       Animations.hpp          # Animation declarations
│       
├───build
│       internal_hook.dll       # Compiled output DLL
│       
├───GUI
│       GUI.cpp                 # ImGui menu rendering
│       GUI.hpp                 # GUI declarations
│       
├───Hook
│       Hook.cpp                # DirectX 11 hook logic
│       Hook.hpp                # Hook declarations
│       
├───ImGui
│   │   imconfig.h              # ImGui configuration
│   │   imgui.cpp               # ImGui core implementation
│   │   imgui.h                 # ImGui interface definitions
│   │   imgui_demo.cpp          # ImGui demo examples
│   │   imgui_draw.cpp          # ImGui drawing routines
│   │   imgui_internal.h        # Internal ImGui declarations
│   │   imgui_tables.cpp        # Table widget implementation
│   │   imgui_widgets.cpp       # Widget implementation
│   │   imstb_rectpack.h        # Packing helper header
│   │   imstb_textedit.h        # Text edit helper
│   │   imstb_truetype.h        # Font rasterizer helper
│   │   
│   └───backend
│           imgui_impl_dx11.cpp # DX11 backend integration
│           imgui_impl_dx11.h   # DX11 backend declarations
│           imgui_impl_win32.cpp# Win32 backend integration
│           imgui_impl_win32.h  # Win32 backend declarations
│           
├───Input
│       Input.cpp               # Keyboard/mouse input processing
│       Input.hpp               # Input definitions
│       
├───minhook
│       buffer.c                # MinHook buffer handling
│       buffer.h                # Buffer declarations
│       hde32.c                 # 32-bit instruction decoder
│       hde32.h                 # 32-bit decoder header
│       hde64.c                 # 64-bit instruction decoder
│       hde64.h                 # 64-bit decoder header
│       hook.c                  # Hook implementation
│       MinHook.def             # Library export definitions
│       MinHook.h               # MinHook API declarations
│       MinHook.rc              # Resource script
│       pstdint.h               # Fixed-width integer types
│       table32.h               # 32-bit hook tables
│       table64.h               # 64-bit hook tables
│       trampoline.c            # Hook trampoline generation
│       trampoline.h            # Trampoline declarations
│       
├───Modules
│   │   ModuleHeader.hpp        # Central module includes
│   │   
│   ├───Combat
│   │   ├───Hitbox
│   │   │       Hitbox.cpp       # Hitbox feature logic
│   │   │       Hitbox.hpp       # Hitbox declarations
│   │   │       
│   │   └───Reach
│   │           Reach.cpp        # Reach feature logic
│   │           Reach.hpp        # Reach declarations
│   │           
│   ├───Misc
│   │   └───UnlockFPS
│   │           UnlockFPS.cpp    # FPS unlock logic
│   │           UnlockFPS.hpp    # UnlockFPS declarations
│   │           
│   ├───Movement
│   │   ├───AutoSprint
│   │   │       AutoSprint.cpp   # Auto-sprint feature logic
│   │   │       AutoSprint.hpp   # AutoSprint declarations
│   │   │       
│   │   └───Timer
│   │           Timer.cpp        # Timer modification logic
│   │           Timer.hpp        # Timer declarations
│   │           
│   ├───Terminal
│   │       Terminal.cpp         # In-game terminal logic
│   │       Terminal.hpp         # Terminal declarations
│   │       
│   └───Visuals
│       ├───CPSCounter
│       │       CPSCounter.cpp   # CPS counter overlay logic
│       │       CPSCounter.hpp   # CPS counter declarations
│       │       
│       ├───FullBright
│       │       FullBright.cpp   # Full bright feature logic
│       │       FullBright.hpp   # FullBright declarations
│       │       
│       ├───Keystrokes
│       │   │   Keystrokes.cpp   # Keystroke overlay logic
│       │   │   Keystrokes.hpp   # Keystrokes declarations
│       │   │   
│       │   └───Helper
│       │           HelperFunctions.hpp # Keystrokes helper utilities
│       │           
│       ├───MotionBlur
│       │       MotionBlur.cpp    # Motion blur effect logic
│       │       MotionBlur.hpp    # MotionBlur declarations
│       │       
│       ├───RenderInfo
│       │       RenderInfo.cpp    # Render info overlay logic
│       │       RenderInfo.hpp    # RenderInfo declarations
│       │       
│       └───Watermark
│               Watermark.cpp    # Watermark display logic
│               Watermark.hpp    # Watermark declarations
│               
├───nlohmann
│       json.hpp                # JSON parser header
│       
└───Utils
        HudElement.hpp         # HUD element definitions
```

## Features

### Combat
- **Reach**: Modify entity interaction distance
- **Hitbox**: Extend target collision boxes
- **Smart detection**: Automatic animation-aware hitbox scaling

### Movement
- **AutoSprint**: Continuous sprint without interaction
- **Timer**: Frame time multiplier for speed modifications
- **FullBright**: Enhanced visibility in dark areas
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

1. **Compile** the project using PowerShell script
2. **Inject** `internal_hook.dll` into a DirectX 11 application
3. **Press INSERT** key to toggle the menu
4. **Configure** features via the tabbed interface
5. **Customize** window positions by dragging

### Keyboard Shortcuts
- **INSERT**: Toggle main menu
- **Drag windows**: Left mouse button when menu is open

## Technical Details

### DirectX 11 Hooking
The project hooks `IDXGISwapChain::Present()` and `ResizeBuffers()` to intercept rendering. ImGui is initialized with DX11 backend for native rendering without performance overhead.

### Input Handling
- Manual input system for compatibility
- Virtual key mapping to ImGui keys
- Character input for IME support
- Mouse and keyboard state tracking

### Animation System
- **Centralized Framework**: All easing functions in `Animations` namespace
- **Easing Functions**: Cubic, quadratic, exponential, and elastic easing
- **Animation Utilities**: Progress calculation and value clamping
- **Thread-Safe Design**: Static methods for safe concurrent access
- **Smooth Transitions**: Configurable duration and interpolation

## Build System

This project uses the included PowerShell build script:

- `build.ps1`: PowerShell build script using `g++`

### Source Organization
- **Core Sources**: Main application logic and module implementations
- **ImGui Sources**: GUI framework implementation
- **Backend Sources**: Platform-specific ImGui backends (DX11, Win32)
- **MinHook Sources**: API hooking library

### Module Structure
Features are organized in a hierarchical module system:
- `Modules/Combat/`: Combat modifications (Hitbox, Reach)
- `Modules/Movement/`: Movement enhancements (AutoSprint, Timer)
- `Modules/Visuals/`: Visual features (MotionBlur, Keystrokes, etc.)
- `Modules/Misc/`: Miscellaneous utilities (UnlockFPS)

### Dependencies
- **ImGui**: Included as source files
- **MinHook**: Lightweight API hooking library
- **DirectX 11**: Windows graphics API
- **Windows SDK**: System libraries and headers

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
- Build using the included build script or your preferred MSVC workflow.

### MinGW-w64
- Use `build.ps1` and ensure `g++` is installed.

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

**Last Updated**: April 4, 2026  
**Status**: Active Development
