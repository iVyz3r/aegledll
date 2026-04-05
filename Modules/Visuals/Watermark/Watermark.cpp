#include "Watermark.hpp"
#include "../../../Animations/Animations.hpp"
#include "../../../Utils/HudElement.hpp"
#include "../../../ImGui/imgui.h"
#include <windows.h>
#include <cmath>
#include <cstdio>

// Static member initialization
bool Watermark::g_showWatermark = true;
ULONGLONG Watermark::g_watermarkEnableTime = 0;
ULONGLONG Watermark::g_watermarkDisableTime = 0;
float Watermark::g_watermarkAnim = 1.0f;
HudElement* Watermark::g_watermarkHud = nullptr;

// Forward declarations for helper functions

// Local helper function for chroma color cycling
ImVec4 GetChromaColor(float time) {
    // Chroma color cycle
    ImVec4 colors[3] = {
        ImVec4(0x87/255.f, 0xF8/255.f, 0xFF/255.f, 1.0f),
        ImVec4(0x87/255.f, 0x97/255.f, 0xFF/255.f, 1.0f),
        ImVec4(0xE9/255.f, 0x87/255.f, 0xFF/255.f, 1.0f)
    };
    
    float cycleTime = fmodf(time * 2.0f, 3.0f);
    int colorIdx = (int)cycleTime;
    float blend = cycleTime - colorIdx;
    
    int nextIdx = (colorIdx + 1) % 3;
    ImVec4& c1 = colors[colorIdx];
    ImVec4& c2 = colors[nextIdx];
    
    return ImVec4(
        c1.x + (c2.x - c1.x) * blend,
        c1.y + (c2.y - c1.y) * blend,
        c1.z + (c2.z - c1.z) * blend,
        1.0f
    );
}

void Watermark::Initialize(HudElement* hud) {
    g_watermarkHud = hud;
}

void Watermark::UpdateAnimation(ULONGLONG now) {
    // 💧 Watermark Animation - Fade in/out exponencial
    if (g_showWatermark && g_watermarkEnableTime == 0) {
        g_watermarkEnableTime = now;
        g_watermarkDisableTime = 0;
    }
    if (!g_showWatermark && g_watermarkDisableTime == 0 && g_watermarkEnableTime > 0) {
        g_watermarkDisableTime = now;
        g_watermarkEnableTime = 0;  // Resetear enable para que se use el else if
    }
    
    if (g_watermarkEnableTime > 0) {
        float enableElapsed = (float)(now - g_watermarkEnableTime) / 1000.0f;
        g_watermarkAnim = fminf(1.0f, enableElapsed / 0.4f);
    }
    else if (g_watermarkDisableTime > 0) {
        float disableElapsed = (float)(now - g_watermarkDisableTime) / 1000.0f;
        float disableAnim = fminf(1.0f, disableElapsed / 0.3f);  // 300ms para desaparecer
        g_watermarkAnim = 1.0f - disableAnim;
        if (disableAnim >= 1.0f) {
            g_watermarkEnableTime = 0;
            g_watermarkDisableTime = 0;
        }
    }
}

void Watermark::RenderArrayList(ImDrawList* draw, ImVec2 arrayListStart, float& yPos, ImVec2& arrayListEnd) {
    // Watermark module in array list
    if (g_showWatermark || (g_watermarkDisableTime > 0 && g_watermarkAnim > 0.01f)) {
        float watermarkAlpha = g_watermarkAnim * 255.0f;
        float slideOffset = -60.0f + (Animations::SmoothInertia(g_watermarkAnim) * 60.0f);
        
        if (watermarkAlpha > 1.0f) {
            char wBuf[64];
            sprintf_s(wBuf, "Watermark");
            float wW = ImGui::CalcTextSize(wBuf).x;
            float xPosW = arrayListStart.x + 290.0f - wW - 10;  // 290.0f is typical array list width
            draw->AddText(ImVec2(xPosW + slideOffset - 1, yPos + 1), IM_COL32(0, 0, 0, 220), wBuf); // Sombra
            draw->AddText(ImVec2(xPosW + slideOffset, yPos), IM_COL32(100, 255, 200, (int)watermarkAlpha), wBuf);
            yPos += 18.0f;
            arrayListEnd.y = yPos;
        }
    }
}

void Watermark::RenderDisplay() {
    // Watermark display
    if (g_showWatermark || g_watermarkAnim > 0.01f) {
        if (!g_watermarkHud) return;
        
        extern bool g_showMenu;
        g_watermarkHud->HandleDrag(g_showMenu);
        g_watermarkHud->ClampToScreen();

        float easedWatermarkAnim = Animations::EaseOutExpo(g_watermarkAnim);
        float watermarkAlpha = easedWatermarkAnim;
        
        ImVec4 chromaColor = GetChromaColor((float)GetTickCount64() / 1000.0f);
        chromaColor.w = watermarkAlpha;
        
        ImDrawList* watermarkDraw = ImGui::GetForegroundDrawList();
        ImVec2 textPos = g_watermarkHud->pos;
        
        // Glow blur (multiple layers with decreasing alpha)
        ImVec4 glowColor = chromaColor;
        float fontSize = 32.0f;  // 2x bigger than default 16pt
        ImFont* font = ImGui::GetFont();
        for (int i = 3; i >= 1; --i) {
            glowColor.w = (chromaColor.w * 0.4f) / i;
            watermarkDraw->AddText(font, fontSize, ImVec2(textPos.x + i, textPos.y), ImGui::GetColorU32(glowColor), "Aegleseeker");
            watermarkDraw->AddText(font, fontSize, ImVec2(textPos.x - i, textPos.y), ImGui::GetColorU32(glowColor), "Aegleseeker");
            watermarkDraw->AddText(font, fontSize, ImVec2(textPos.x, textPos.y + i), ImGui::GetColorU32(glowColor), "Aegleseeker");
            watermarkDraw->AddText(font, fontSize, ImVec2(textPos.x, textPos.y - i), ImGui::GetColorU32(glowColor), "Aegleseeker");
        }
        
        // Main text with chroma color - 2x bigger
        watermarkDraw->AddText(font, fontSize, textPos, ImGui::GetColorU32(chromaColor), "Aegleseeker");

        // Update hitbox size based on actual fontSize (32pt = 2x scale of default 16pt)
        float fontScale = fontSize / 16.0f;  // Calculate scale factor
        ImVec2 textSize = ImGui::CalcTextSize("Aegleseeker");
        g_watermarkHud->size = ImVec2(textSize.x * fontScale + 20, textSize.y * fontScale + 10);

        // DEBUG: show hitbox only when menu is open
        if (g_showMenu) {
            watermarkDraw->AddRect(
                g_watermarkHud->pos,
                ImVec2(g_watermarkHud->pos.x + g_watermarkHud->size.x, g_watermarkHud->pos.y + g_watermarkHud->size.y),
                IM_COL32(255, 100, 100, 80)
            );
        }
    }
}

void Watermark::RenderMenu() {
    // Show Watermark
    ImGui::Checkbox("Watermark", &g_showWatermark);
}
