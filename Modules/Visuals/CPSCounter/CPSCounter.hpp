#pragma once

#include <string>
#include <vector>
#include <windows.h>

// Forward declarations
class ImDrawList;
struct ImVec2;
struct ImVec4;
class HudElement;

class CPSCounter {
public:
    // Configuration
    static bool g_showCpsCounter;
    static std::string g_cpsCounterFormat;
    static float g_cpsTextScale;
    static ImVec4 g_cpsTextColor;
    
    // Animation
    static float g_cpsCounterAnim;
    static ULONGLONG g_cpsCounterEnableTime;
    static ULONGLONG g_cpsCounterDisableTime;
    
    // Display options
    static int g_cpsCounterAlignment;
    static bool g_cpsCounterShadow;
    static float g_cpsCounterShadowOffset;
    static ImVec4 g_cpsCounterShadowColor;
    static float g_cpsCounterX;
    static float g_cpsCounterY;
    static bool g_cpsCounterFirstRender;
    
    // HUD Element
    static HudElement* g_cpsHud;
    
    // Methods
    static void Initialize(HudElement* hudElement);
    static void UpdateAnimation(ULONGLONG now);
    static void RenderArrayList(class ImDrawList* draw, ImVec2 arrayListStart, float& yPos, ImVec2& arrayListEnd);
    static void RenderDisplay(int screenWidth, int screenHeight, int g_lmbCps, int g_rmbCps);
    static void RenderMenu();
    
    // Helper
    static std::string ProcessCPSCounterFormat(const std::string& format, int lmb, int rmb);
};
