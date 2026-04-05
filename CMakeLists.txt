cmake_minimum_required(VERSION 3.15)
project(AegleInternal
    VERSION 1.0.0
    LANGUAGES C CXX
    DESCRIPTION "DirectX 11 internal hook with ImGui interface"
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Compiler flags
if(MSVC)
    # Visual C++ compiler
    add_compile_options(/O2 /W4)
else()
    # GCC/MinGW compiler
    add_compile_options(
        -O2
        -m64
        -march=x86-64
        -fpermissive
        -Wall
    )
    
    # Static linking
    add_link_options(
        -static
        -static-libgcc
        -static-libstdc++
    )
endif()

# Main project sources
set(CORE_SOURCES
    dllmain.cpp
    Animations/Animations.cpp
    GUI/GUI.cpp
    Hook/Hook.cpp
    Input/Input.cpp
    Reach.cpp
    Hitbox.cpp
    AutoSprint.cpp
    Timer.cpp
    FullBright.cpp
    Modules/Visuals/RenderInfo/RenderInfo.cpp
    Modules/Visuals/Watermark/Watermark.cpp
    Modules/Visuals/MotionBlur/MotionBlur.cpp
    Modules/Visuals/Keystrokes/Keystrokes.cpp
    Modules/Visuals/CPSCounter/CPSCounter.cpp
    Modules/Visuals/UnlockFPS/UnlockFPS.cpp
)

# ImGui sources
set(IMGUI_SOURCES
    ImGui/imgui.cpp
    ImGui/imgui_draw.cpp
    ImGui/imgui_widgets.cpp
    ImGui/imgui_tables.cpp
)

# ImGui backend sources
set(IMGUI_BACKEND_SOURCES
    ImGui/backend/imgui_impl_dx11.cpp
    ImGui/backend/imgui_impl_win32.cpp
)

# MinHook library sources
set(MINHOOK_SOURCES
    minhook/buffer.c
    minhook/hook.c
    minhook/trampoline.c
    minhook/hde64.c
)

# All source files
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${IMGUI_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
    ${MINHOOK_SOURCES}
)

# Create shared library (DLL)
add_library(internal_hook SHARED ${ALL_SOURCES})

# Include directories
target_include_directories(internal_hook PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGui
    ${CMAKE_CURRENT_SOURCE_DIR}/ImGui/backend
    ${CMAKE_CURRENT_SOURCE_DIR}/minhook
)

# Link libraries
target_link_libraries(internal_hook PRIVATE
    d3d11
    dxgi
    d3dcompiler
    dwmapi
    imm32
    user32
    gdi32
    psapi
)

# Output DLL name and location
set_target_properties(internal_hook PROPERTIES
    PREFIX ""
    SUFFIX ".dll"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build"
)

# Set output name
set_target_properties(internal_hook PROPERTIES
    OUTPUT_NAME "internal_hook"
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(internal_hook PRIVATE
        _WINDOWS
        WIN32
        _DEBUG=0
    )
endif()

# Print configuration info
message(STATUS "=== AegleInternal Build Configuration ===")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "OUTPUT_DIRECTORY: ${CMAKE_BINARY_DIR}/build")
message(STATUS "=========================================")
