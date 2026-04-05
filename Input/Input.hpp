#pragma once

#include <windows.h>
#include "../ImGui/imgui.h"

/// @brief Input class - Manages all keyboard and mouse input handling
class Input {
public:
    // Keyboard state arrays
    static bool g_keys[256];
    static bool g_keysPressed[256];
    static bool g_keysReleased[256];
    
    // Input methods
    static void UpdateKeyboard(bool menuOpen);
    static void UpdateMouse(HWND window, float screenWidth, float screenHeight, bool drawCursor);
    static void Update(HWND window, float screenWidth, float screenHeight, bool menuOpen, bool drawCursor);
    
    // Utility
    static ImGuiKey VKToImGuiKey(int vk);
};
