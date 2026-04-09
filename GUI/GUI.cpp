#include "GUI.hpp"
#include "../Animations/Animations.hpp"
#include "../Modules/Terminal/Terminal.hpp"
#include <windows.h>
#include <cmath>

// Static member initialization
bool GUI::g_showMenu = false;
float GUI::g_menuAnim = 0.0f;

int GUI::g_currentTab = 0;
int GUI::g_previousTab = 0;
ULONGLONG GUI::g_tabChangeTime = 0;
float GUI::g_tabAnim = 0.0f;

// Forward declarations for helper functions

void GUI::ApplyTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // 🔹 Bordes y redondeo
    style.WindowRounding = 10.0f;
    style.FrameRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 6.0f;
    
    // 🔹 Espaciado
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(8, 5);
    style.ItemSpacing = ImVec2(8, 6);
    
    // 🔹 Colores
    ImVec4* colors = style.Colors;
    
    // Fondo
    colors[ImGuiCol_WindowBg]         = ImVec4(0.09f, 0.07f, 0.10f, 1.00f);
    
    // Texto
    colors[ImGuiCol_Text]             = ImVec4(0.95f, 0.88f, 0.92f, 1.00f);
    colors[ImGuiCol_TextDisabled]     = ImVec4(0.55f, 0.45f, 0.50f, 1.00f);
    
    // Frames (inputs, sliders)
    colors[ImGuiCol_FrameBg]          = ImVec4(0.18f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.30f, 0.15f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgActive]    = ImVec4(0.35f, 0.18f, 0.25f, 1.00f);
    
    // Botones
    colors[ImGuiCol_Button]           = ImVec4(0.85f, 0.35f, 0.55f, 1.00f);
    colors[ImGuiCol_ButtonHovered]    = ImVec4(0.95f, 0.45f, 0.65f, 1.00f);
    colors[ImGuiCol_ButtonActive]     = ImVec4(0.75f, 0.25f, 0.45f, 1.00f);
    
    // Headers
    colors[ImGuiCol_Header]           = ImVec4(0.80f, 0.30f, 0.50f, 1.00f);
    colors[ImGuiCol_HeaderHovered]    = ImVec4(0.90f, 0.40f, 0.60f, 1.00f);
    colors[ImGuiCol_HeaderActive]     = ImVec4(0.70f, 0.20f, 0.40f, 1.00f);
    
    // Tabs
    colors[ImGuiCol_Tab]              = ImVec4(0.20f, 0.12f, 0.18f, 1.00f);
    colors[ImGuiCol_TabHovered]       = ImVec4(0.85f, 0.35f, 0.55f, 1.00f);
    colors[ImGuiCol_TabActive]        = ImVec4(0.70f, 0.30f, 0.50f, 1.00f);
    
    // Titles
    colors[ImGuiCol_TitleBg]          = ImVec4(0.15f, 0.10f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgActive]    = ImVec4(0.30f, 0.15f, 0.22f, 1.00f);
    
    // Scrollbar
    colors[ImGuiCol_ScrollbarBg]      = ImVec4(0.10f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]    = ImVec4(0.75f, 0.25f, 0.45f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.85f, 0.35f, 0.55f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.95f, 0.45f, 0.65f, 1.00f);
    
    // Checkmark
    colors[ImGuiCol_CheckMark]        = ImVec4(1.00f, 0.55f, 0.75f, 1.00f);
    
    // Slider
    colors[ImGuiCol_SliderGrab]       = ImVec4(0.95f, 0.45f, 0.65f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.55f, 0.75f, 1.00f);
    
    // Otros elementos
    colors[ImGuiCol_Separator]        = ImVec4(0.75f, 0.25f, 0.45f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.85f, 0.35f, 0.55f, 1.00f);
    colors[ImGuiCol_SeparatorActive]  = ImVec4(0.95f, 0.45f, 0.65f, 1.00f);
    style.FrameBorderSize = 1.0f;
    style.WindowBorderSize = 1.0f;
}

// External vars from dllmain.cpp
extern bool g_showMenu;
extern ULONGLONG g_notifStart;
extern bool g_firstTabOpen;
extern bool g_keys[256];
extern int g_currentTab;
extern int g_previousTab;
extern ULONGLONG g_tabChangeTime;
extern float g_tabAnim;

// Module includes for RenderMenu
#include "../Modules/ModuleHeader.hpp"

void GUI::UpdateAnimation(ULONGLONG now, float dt) {
    // 📉 MENU ANIMATION - Velocidades optimizadas para elegancia
    if (GUI::g_showMenu) {
        GUI::g_menuAnim += 2.8f * dt;  // Apertura elegante
    } else {
        GUI::g_menuAnim -= 3.8f * dt;
    }
    if (GUI::g_menuAnim > 1.0f) GUI::g_menuAnim = 1.0f;
    if (GUI::g_menuAnim < 0.0f) GUI::g_menuAnim = 0.0f;
    
    // Tab change animation
    if (GUI::g_tabChangeTime > 0) {
        float tabChangeElapsed = (float)(now - GUI::g_tabChangeTime) / 1000.0f;
        GUI::g_tabAnim = fminf(1.0f, tabChangeElapsed / 0.25f);
    }
}

void GUI::RenderMenu(float screenWidth, float screenHeight) {
    if (GUI::g_menuAnim <= 0.001f) return;
    
    float e = Animations::SmoothInertia(GUI::g_menuAnim);
    
    // Background with animation
    float bgAlpha = Animations::SmoothInertia(e) * 140.0f;
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), ImVec2(screenWidth, screenHeight), IM_COL32(0, 0, 0, (int)bgAlpha));

    // Render menu content when visible
    if (e > 0.12f) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, e);
        
        // Improved scale with easing
        float sc = 0.85f + (0.15f * Animations::SmoothInertia(e));
        
        ImVec2 bS = ImVec2(750, 500);
        ImGui::SetNextWindowSize(ImVec2(bS.x * sc, bS.y * sc), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(screenWidth / 2, screenHeight / 2), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

        if (ImGui::Begin("Aegleseeker Config", &g_showMenu, ImGuiWindowFlags_NoCollapse)) {
            // Estilos mejorados para la interfaz
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
            ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.95f, 0.45f, 0.65f, 0.9f));
            ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 0.55f, 0.75f, 1.0f));
            
            // Scrollable menu content
            ImGui::BeginChild("MenuScroll", ImVec2(0, 0), false);
            
            if (ImGui::BeginTabBar("Tabs")) {
                // Detect tab change
                int prevTab = g_currentTab;
                
                if (ImGui::BeginTabItem("Combat")) {
                    g_currentTab = 0;
                    if (g_firstTabOpen) {
                        g_firstTabOpen = false;
                        g_tabAnim = 1.0f;
                    } else if (g_currentTab != prevTab) {
                        g_tabChangeTime = GetTickCount64();
                        g_tabAnim = 0.0f;
                        g_previousTab = prevTab;
                    }
                    
                    float alphaFade = (g_tabChangeTime > 0) ? Animations::SmoothInertia(g_tabAnim) : 1.0f;
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alphaFade);
                    
                    Reach::RenderMenu();
                    ImGui::Separator();
                    Hitbox::RenderMenu();
                    
                    ImGui::PopStyleVar();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Movement")) {
                    g_currentTab = 1;
                    if (g_currentTab != prevTab) {
                        g_tabChangeTime = GetTickCount64();
                        g_tabAnim = 0.0f;
                        g_previousTab = prevTab;
                    }
                    
                    float alphaFade = (g_tabChangeTime > 0) ? Animations::SmoothInertia(g_tabAnim) : 1.0f;
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alphaFade);
                    
                    AutoSprint::RenderMenu();
                    ImGui::Separator();
                    Timer::RenderMenu();
                    
                    ImGui::PopStyleVar();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Visuals")) {
                    g_currentTab = 2;
                    if (g_currentTab != prevTab) {
                        g_tabChangeTime = GetTickCount64();
                        g_tabAnim = 0.0f;
                        g_previousTab = prevTab;
                    }
                    
                    float alphaFade = (g_tabChangeTime > 0) ? Animations::SmoothInertia(g_tabAnim) : 1.0f;
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alphaFade);
                    
                    FullBright::RenderMenu();
                    ImGui::Separator();
                    RenderInfo::RenderMenu();
                    Watermark::RenderMenu();
                    ImGui::Separator();
                    MotionBlur::RenderMenu();
                    Keystrokes::RenderMenu();
                    ImGui::Separator();
                    CPSCounter::RenderMenu();
                    
                    ImGui::PopStyleVar();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Misc")) {
                    g_currentTab = 3;
                    if (g_currentTab != prevTab) {
                        g_tabChangeTime = GetTickCount64();
                        g_tabAnim = 0.0f;
                        g_previousTab = prevTab;
                    }
                    
                    float alphaFade = (g_tabChangeTime > 0) ? Animations::SmoothInertia(g_tabAnim) : 1.0f;
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alphaFade);
                    
                    UnlockFPS::RenderMenu();
                    
                    ImGui::PopStyleVar();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Terminal")) {
                    g_currentTab = 4;
                    if (g_currentTab != prevTab) {
                        g_tabChangeTime = GetTickCount64();
                        g_tabAnim = 0.0f;
                        g_previousTab = prevTab;
                    }
                    
                    float alphaFade = (g_tabChangeTime > 0) ? Animations::SmoothInertia(g_tabAnim) : 1.0f;
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alphaFade);
                    
                    Terminal::RenderConsole();
                    
                    ImGui::PopStyleVar();
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            
            ImGui::EndChild();
            
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
            ImGui::PopStyleVar();
            ImGui::PopStyleVar();
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }
}

void GUI::RenderNotification(float screenWidth, float screenHeight) {
    float tNotif = (float)(GetTickCount64() - g_notifStart) / 1000.0f;
    if (tNotif < 5.0f) {
        // Entry: 0-0.6s, Display: 0.6-4.0s, Exit: 4.0-5.0s
        float nT = tNotif < 0.6f ? tNotif / 0.6f : (tNotif > 4.0f ? 1.0f - (tNotif - 4.0f) / 1.0f : 1.0f);
        float nE = Animations::SmoothInertia(nT) * 255.0f;
        ImVec2 nS = ImVec2(320, 70);
        ImGui::SetNextWindowPos(ImVec2(screenWidth - ((nS.x + 20.0f) * nE / 255.0f), screenHeight - nS.y - 20.0f));
        ImGui::SetNextWindowSize(nS);
        ImGui::Begin("##Notif", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
        ImGui::TextColored(ImVec4(0, 0.8f, 1, nE / 255.0f), "Aegleseeker DLL | build: 2026-03");
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1, 0.6f, 0.2f, nE / 255.0f), "gh: https://github.com/iVyz3r/aegleseeker");
        ImGui::TextColored(ImVec4(1, 1, 1, nE / 255.0f), "Press Insert to Open Menu");
        ImGui::End();
    }
}
