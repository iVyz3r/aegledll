#include "UnlockFPS.hpp"
#include "Animations/Animations.hpp"
#include "ImGui/imgui.h"
#include <windows.h>
#include <cstdio>
#include <cmath>

// Static member initialization
bool UnlockFPS::g_unlockFpsEnabled = false;
float UnlockFPS::g_fpsLimit = 60.0f;
ULONGLONG UnlockFPS::g_unlockFpsEnableTime = 0;
ULONGLONG UnlockFPS::g_unlockFpsDisableTime = 0;

// Forward declarations for helper functions
extern bool g_vsync;

// Constants for animation
#define FADE_IN_TIME 0.3f
#define FADE_OUT_TIME 0.3f
#define SLIDE_TIME 0.4f

void UnlockFPS::Initialize() {
    // Initialize if needed
}

void UnlockFPS::UpdateFPS() {
    // 🔒 UNLOCK FPS - Disable VSync completely for UWP
    g_vsync = false;
    
    if (g_unlockFpsEnabled && g_fpsLimit > 0.0f) {
        static LARGE_INTEGER lastFramePC = { 0 };
        static LARGE_INTEGER frequency = { 0 };
        
        // Get frequency once
        if (frequency.QuadPart == 0) {
            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&lastFramePC);
        }
        
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        
        // Calculate frame time in microseconds
        double targetFrameTimeUs = 1000000.0 / g_fpsLimit;  // microseconds
        double elapsedUs = (double)(now.QuadPart - lastFramePC.QuadPart) * 1000000.0 / frequency.QuadPart;
        
        // Sleep if faster than target frametime
        if (elapsedUs < targetFrameTimeUs) {
            double remainingUs = targetFrameTimeUs - elapsedUs;
            
            // Sleep a bit less to avoid overshoot
            if (remainingUs > 500.0) {  // > 0.5ms
                Sleep((DWORD)((remainingUs - 500.0) / 1000.0));  // Convert to ms
            }
            
            // Busy-wait for precision
            LARGE_INTEGER waitStart;
            QueryPerformanceCounter(&waitStart);
            while (true) {
                LARGE_INTEGER waitNow;
                QueryPerformanceCounter(&waitNow);
                double waitElapsedUs = (double)(waitNow.QuadPart - waitStart.QuadPart) * 1000000.0 / frequency.QuadPart;
                if (waitElapsedUs >= remainingUs) break;
            }
        }
        
        lastFramePC = now;
    }
}

void UnlockFPS::RenderArrayList(ImDrawList* draw, ImVec2 arrayListStart, float& yPos, ImVec2& arrayListEnd) {
    if (g_unlockFpsEnabled || g_unlockFpsDisableTime > 0) {
        ULONGLONG now = GetTickCount64();
        float timeSinceEnable = (float)(now - g_unlockFpsEnableTime) / 1000.0f;
        float timeSinceDisable = (float)(now - g_unlockFpsDisableTime) / 1000.0f;
        
        float unlockFpsAlpha = 255.0f;
        float slideOffset = 0.0f;
        
        if (g_unlockFpsEnabled) {
            unlockFpsAlpha = Animations::SmoothInertia(fminf(1.0f, timeSinceEnable / FADE_IN_TIME)) * 255.0f;
            // Slide in animation
            float slideProgress = fminf(1.0f, timeSinceEnable / SLIDE_TIME);
            slideOffset = Animations::SmoothInertia(slideProgress) * 60.0f - 60.0f;
        } else if (timeSinceDisable < FADE_OUT_TIME) {
            unlockFpsAlpha = Animations::SmoothInertia(1.0f - (timeSinceDisable / FADE_OUT_TIME)) * 255.0f;
        } else {
            g_unlockFpsDisableTime = 0;
        }
        
        if (unlockFpsAlpha > 1.0f && draw) {
            char uBuf[64];
            sprintf_s(uBuf, sizeof(uBuf), "Unlock FPS - %.0f", g_fpsLimit);
            ImVec2 textSize = ImGui::CalcTextSize(uBuf);
            float xPosU = arrayListStart.x + 300.0f - textSize.x - 10.0f;
            
            draw->AddText(ImVec2(xPosU + slideOffset - 1, yPos + 1), IM_COL32(0, 0, 0, 220), uBuf);
            draw->AddText(ImVec2(xPosU + slideOffset, yPos), IM_COL32(100, 255, 100, (int)unlockFpsAlpha), uBuf);
            yPos += 18.0f;
            arrayListEnd.y = yPos;
        }
    }
}

void UnlockFPS::RenderMenu() {
    // Show Unlock FPS toggle
    if (ImGui::Checkbox("Unlock FPS", &g_unlockFpsEnabled)) {
        if (g_unlockFpsEnabled) {
            g_unlockFpsEnableTime = GetTickCount64();
            g_unlockFpsDisableTime = 0;
        } else {
            g_unlockFpsDisableTime = GetTickCount64();
            g_unlockFpsEnableTime = 0;
        }
    }
    
    // FPS slider (if enabled)
    if (g_unlockFpsEnabled) {
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.40f, 0.18f, 0.28f, 0.8f));
        ImGui::SliderFloat("FPS Limit", &g_fpsLimit, 5.0f, 240.0f, "%.0f FPS");
        ImGui::PopStyleColor();
    }
}
