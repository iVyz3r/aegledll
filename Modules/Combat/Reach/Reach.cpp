#include "Reach.hpp"
#include "../../../ImGui/imgui.h"
#include <windows.h>
#include <cstdio>

// Static member initialization
float* Reach::g_reachPtr = nullptr;
float Reach::g_reachValue = 3.0f;
ULONGLONG Reach::g_reachEnableTime = 0;

void Reach::Initialize(uintptr_t gameBase) {
    g_reachPtr = (float*)(gameBase + 0xB52A70);
    g_reachEnableTime = GetTickCount64();
}

void Reach::UpdateValue(float newValue) {
    if (!g_reachPtr) return;
    
    g_reachValue = newValue;
    DWORD old;
    VirtualProtect(g_reachPtr, 4, PAGE_EXECUTE_READWRITE, &old);
    *g_reachPtr = newValue;
    VirtualProtect(g_reachPtr, 4, old, &old);
}

void Reach::RenderArrayList(ImDrawList* draw, ImVec2 arrayListStart, float& yPos, ImVec2& arrayListEnd) {
    // Reach module (always visible)
    char rBuf[64];
    float reachAlpha = 255.0f;
    
    sprintf_s(rBuf, "Reach - %.2f", g_reachValue);
    float wR = ImGui::CalcTextSize(rBuf).x;
    float xPos = arrayListStart.x + 290.0f - wR - 10;
    
    draw->AddText(ImVec2(xPos - 1, yPos + 1), IM_COL32(0, 0, 0, 220), rBuf); // Shadow
    draw->AddText(ImVec2(xPos, yPos), IM_COL32(0, 200, 255, (int)reachAlpha), rBuf);
    
    yPos += 18.0f;
    arrayListEnd.y = yPos;
}

void Reach::RenderMenu() {
    // Reach Slider
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.40f, 0.18f, 0.28f, 0.8f));
    ImGui::Text("Reach");
    if (ImGui::SliderFloat("Reach Distance", &g_reachValue, 3.0f, 15.0f, "%.2f")) {
        UpdateValue(g_reachValue);
    }
    ImGui::PopStyleColor();
}
