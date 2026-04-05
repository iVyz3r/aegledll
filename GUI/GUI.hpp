#pragma once

#include <windows.h>
#include "../ImGui/imgui.h"

/// @brief GUI class - Handles all UI logic and rendering
class GUI {
public:
    // Menu state
    static bool g_showMenu;
    static float g_menuAnim;
    
    // Tab state
    static int g_currentTab;
    static int g_previousTab;
    static ULONGLONG g_tabChangeTime;
    static float g_tabAnim;
    
    // Style and theme
    static void ApplyTheme();
    
    // Menu animation update
    static void UpdateAnimation(ULONGLONG now, float dt);
    
    // Menu rendering
    static void RenderMenu(float screenWidth, float screenHeight);
    
    // Notification
    static void RenderNotification(float screenWidth, float screenHeight);
};
