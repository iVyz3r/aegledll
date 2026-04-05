#pragma once

#include <cstdint>
#include <windows.h>

// Forward declarations
class ImDrawList;
struct ImVec2;
class HudElement;

// RenderInfo Module
class RenderInfo {
public:
    static bool g_showRenderInfo;
    static float g_fpsCounter;
    static float g_frameTime;
    static ULONGLONG g_renderInfoEnableTime;
    static ULONGLONG g_renderInfoDisableTime;
    static float g_renderInfoAnim;
    static HudElement* g_renderInfoHud;

    // Initialize renderinfo module
    static void Initialize(HudElement* hud);

    // Update animation state
    static void UpdateAnimation(ULONGLONG now);

    // Render renderinfo in array list
    static void RenderArrayList(ImDrawList* draw, ImVec2 arrayListStart, float& yPos, ImVec2& arrayListEnd);

    // Render renderinfo window
    static void RenderWindow();

    // Render renderinfo UI in menu
    static void RenderMenu();

    // Check if renderinfo is enabled
    static bool IsEnabled() { return g_showRenderInfo; }
};
