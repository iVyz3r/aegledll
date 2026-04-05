#include "RenderInfo.hpp"
#include "../../../Animations/Animations.hpp"
#include "../../../Utils/HudElement.hpp"
#include "../../../ImGui/imgui.h"
#include <windows.h>
#include <cstdio>
#include <cmath>

// Static member initialization
bool RenderInfo::g_showRenderInfo = false;
float RenderInfo::g_fpsCounter = 0.0f;
float RenderInfo::g_frameTime = 0.0f;
ULONGLONG RenderInfo::g_renderInfoEnableTime = 0;
ULONGLONG RenderInfo::g_renderInfoDisableTime = 0;
float RenderInfo::g_renderInfoAnim = 0.0f;
HudElement* RenderInfo::g_renderInfoHud = nullptr;

// Forward declarations for helper functions

void RenderInfo::Initialize(HudElement* hud) {
    g_renderInfoHud = hud;
}

void RenderInfo::UpdateAnimation(ULONGLONG now) {
    // 📊 Render Info Animation - Exponencial suave
    if (g_showRenderInfo && g_renderInfoEnableTime == 0) {
        g_renderInfoEnableTime = now;
        g_renderInfoDisableTime = 0;
    }
    if (!g_showRenderInfo && g_renderInfoDisableTime == 0 && g_renderInfoEnableTime > 0) {
        g_renderInfoDisableTime = now;
        g_renderInfoEnableTime = 0;  // Resetear enable para que se use el else if
    }
    
    if (g_renderInfoEnableTime > 0) {
        float enableElapsed = (float)(now - g_renderInfoEnableTime) / 1000.0f;
        g_renderInfoAnim = fminf(1.0f, enableElapsed / 0.4f);
    }
    else if (g_renderInfoDisableTime > 0) {
        float disableElapsed = (float)(now - g_renderInfoDisableTime) / 1000.0f;
        float disableAnim = fminf(1.0f, disableElapsed / 0.3f);  // 300ms para desaparecer
        g_renderInfoAnim = 1.0f - disableAnim;
        if (disableAnim >= 1.0f) {
            g_renderInfoEnableTime = 0;
            g_renderInfoDisableTime = 0;
        }
    }
}

void RenderInfo::RenderArrayList(ImDrawList* draw, ImVec2 arrayListStart, float& yPos, ImVec2& arrayListEnd) {
    // Show Render Info module
    if (g_showRenderInfo || (g_renderInfoDisableTime > 0 && g_renderInfoAnim > 0.01f)) {
        float renderInfoAlpha = g_renderInfoAnim * 255.0f;
        float slideOffset = -60.0f + (Animations::SmoothInertia(g_renderInfoAnim) * 60.0f);
        
        if (renderInfoAlpha > 1.0f) {
            char riBuf[64];
            sprintf_s(riBuf, "Render Info");
            float wRI = ImGui::CalcTextSize(riBuf).x;
            float xPosRI = arrayListStart.x + 290.0f - wRI - 10;
            draw->AddText(ImVec2(xPosRI + slideOffset - 1, yPos + 1), IM_COL32(0, 0, 0, 220), riBuf); // Sombra
            draw->AddText(ImVec2(xPosRI + slideOffset, yPos), IM_COL32(0, 255, 200, (int)renderInfoAlpha), riBuf);
            yPos += 18.0f;
            arrayListEnd.y = yPos;
        }
    }
}

void RenderInfo::RenderWindow() {
    // --- 📊 RENDER INFO (TOP LEFT, BELOW WATERMARK) - DRAGGABLE ---
    if (g_showRenderInfo || g_renderInfoAnim > 0.01f) {
        if (!g_renderInfoHud) return;
        
        float easedAnim = Animations::EaseOutExpo(g_renderInfoAnim);
        float infoAlpha = 0.7f * easedAnim;
        
        // Render info dimensions
        ImVec2 renderInfoSize = ImVec2(220, 120);
        g_renderInfoHud->size = renderInfoSize;
        
        // Initialize position on first draw
        if (g_renderInfoHud->pos.x == 0 && g_renderInfoHud->pos.y == 0) {
            g_renderInfoHud->pos = ImVec2(10, 50);
        }
        
        // Handle drag and clamp when menu open
        extern bool g_showMenu;
        g_renderInfoHud->HandleDrag(g_showMenu);
        g_renderInfoHud->ClampToScreen();
        
        // Position with smooth animation
        float animatedXOffset = -150.0f + (160.0f * easedAnim);
        ImVec2 finalPos = ImVec2(
            g_renderInfoHud->pos.x + animatedXOffset,
            g_renderInfoHud->pos.y
        );
        
        // Limitar valores para seguridad
        if (finalPos.x < -10000 || finalPos.x > 10000) finalPos.x = 10;
        if (finalPos.y < -10000 || finalPos.y > 10000) finalPos.y = 50;
        
        ImGui::SetNextWindowPos(finalPos, ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(infoAlpha);
        ImGui::SetNextWindowSize(renderInfoSize, ImGuiCond_Always);
        
        // Allow drag only when menu open
        extern bool g_showMenu;
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs;
        if (!g_showMenu) {
            flags |= ImGuiWindowFlags_NoMove;
        }
        
        if (ImGui::Begin("##RenderInfo", nullptr, flags)) {
            ImGuiIO& io = ImGui::GetIO();
            // Calculate real FPS from delta time
            if (io.DeltaTime > 0.0f) {
                g_fpsCounter = 1.0f / io.DeltaTime;
            }
            
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.5f, easedAnim));
            ImGui::Text("FPS: %.0f", g_fpsCounter);
            ImGui::Text("Render: DX11");
            ImGui::Text("OS: Windows");
            ImGui::Text("Build: Release");
            ImGui::Text("Compiler: GCC");
            ImGui::PopStyleColor();
            
            ImGui::End();
        }
        
        // DEBUG: show hitbox only when menu is open
        if (g_showMenu) {
            ImDrawList* debugDraw = ImGui::GetForegroundDrawList();
            if (debugDraw) {
                debugDraw->AddRect(
                    finalPos,
                    ImVec2(finalPos.x + renderInfoSize.x, finalPos.y + renderInfoSize.y),
                    IM_COL32(0, 255, 100, 200),
                    0.0f,
                    0,
                    2.0f
                );
            }
        }
    }
}

void RenderInfo::RenderMenu() {
    // Show Render Info
    ImGui::Checkbox("Render Info", &g_showRenderInfo);
}
