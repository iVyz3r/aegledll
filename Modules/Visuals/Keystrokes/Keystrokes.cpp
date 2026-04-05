#include "Keystrokes.hpp"
#include "../../../Utils/HudElement.hpp"
#include "Helper/HelperFunctions.hpp"
#include "../../../Animations/Animations.hpp"
#include "../../../ImGui/imgui.h"
#include <windows.h>
#include <cmath>
#include <cstdio>

// Static member initialization
bool Keystrokes::g_showKeystrokes = false;
float Keystrokes::g_keystrokesAnim = 0.0f;
ULONGLONG Keystrokes::g_keystrokesEnableTime = 0;
ULONGLONG Keystrokes::g_keystrokesDisableTime = 0;
HudElement* Keystrokes::g_keystrokesHud = nullptr;

float Keystrokes::g_keystrokesUIScale = 1.0f;
bool Keystrokes::g_keystrokesBlurEffect = false;
float Keystrokes::g_keystrokesRounding = 11.0f;
bool Keystrokes::g_keystrokesShowBg = false;
bool Keystrokes::g_keystrokesRectShadow = false;
float Keystrokes::g_keystrokesRectShadowOffset = 0.02f;
bool Keystrokes::g_keystrokesBorder = false;
float Keystrokes::g_keystrokesBorderWidth = 1.0f;
bool Keystrokes::g_keystrokesGlow = false;
float Keystrokes::g_keystrokesGlowAmount = 50.0f;
bool Keystrokes::g_keystrokesGlowEnabled = false;
float Keystrokes::g_keystrokesGlowEnabledAmount = 50.0f;
float Keystrokes::g_keystrokesGlowSpeed = 1.0f;
float Keystrokes::g_keystrokesKeySpacing = 1.63f;
float Keystrokes::g_keystrokesEdSpeed = 1.0f;
float Keystrokes::g_keystrokesTextScale = 1.0f;
float Keystrokes::g_keystrokesTextScale2 = 1.0f;
float Keystrokes::g_keystrokesTextXOffset = 0.5f;
float Keystrokes::g_keystrokesTextYOffset = 0.5f;
bool Keystrokes::g_keystrokesTextShadow = false;
float Keystrokes::g_keystrokesTextShadowOffset = 0.003f;
bool Keystrokes::g_keystrokesLMBRMB = true;
bool Keystrokes::g_keystrokesHideCPS = true;
std::string Keystrokes::g_keystrokesWText = "W";
std::string Keystrokes::g_keystrokesAText = "A";
std::string Keystrokes::g_keystrokesSText = "S";
std::string Keystrokes::g_keystrokesDText = "D";
std::string Keystrokes::g_keystrokesLMBText = "LMB";
std::string Keystrokes::g_keystrokesRMBText = "RMB";
std::string Keystrokes::g_keystrokesLMBCPSText = "{value} CPS";
std::string Keystrokes::g_keystrokesRMBCPSText = "{value} CPS";

ImVec4 Keystrokes::g_keystrokesBgColor = ImVec4(0.314f, 0.314f, 0.314f, 0.55f);
ImVec4 Keystrokes::g_keystrokesEnabledColor = ImVec4(0.0f, 1.0f, 0.4f, 1.0f);
ImVec4 Keystrokes::g_keystrokesTextColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
ImVec4 Keystrokes::g_keystrokesTextEnabledColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
ImVec4 Keystrokes::g_keystrokesRectShadowColor = ImVec4(0.0f, 0.0f, 0.0f, 0.55f);
ImVec4 Keystrokes::g_keystrokesTextShadowColor = ImVec4(0.0f, 0.0f, 0.0f, 0.55f);
ImVec4 Keystrokes::g_keystrokesBorderColor = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
ImVec4 Keystrokes::g_keystrokesGlowColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
ImVec4 Keystrokes::g_keystrokesGlowEnabledColor = ImVec4(0.941f, 0.941f, 1.0f, 1.0f);
ImVec4 Keystrokes::g_keystrokesEnabledShadowColor = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
ImVec4 Keystrokes::g_keystrokesDisabledShadowColor = ImVec4(0.0f, 0.0f, 0.0f, 0.55f);

std::vector<ImVec4> Keystrokes::g_keystrokesStates(7, ImVec4(0.314f, 0.314f, 0.314f, 0.55f));
std::vector<ImVec4> Keystrokes::g_keystrokesTextStates(7, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
std::vector<ImVec4> Keystrokes::g_keystrokesShadowStates(7, ImVec4(0.0f, 0.0f, 0.0f, 0.55f));
float Keystrokes::g_keystrokesGlowModifier[7] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

bool Keystrokes::g_keystrokesShowSpacebar = true;
float Keystrokes::g_keystrokesSpacebarWidth = 1.0f;
bool Keystrokes::g_keystrokesShowMouseButtons = true;
bool Keystrokes::g_keystrokesShowLMBRMB = true;

// CPS tracking for LMB and RMB
std::vector<ULONGLONG> Keystrokes::g_lmbClickTimes(10, 0);
std::vector<ULONGLONG> Keystrokes::g_rmbClickTimes(10, 0);
int Keystrokes::g_lmbClickIndex = 0;
int Keystrokes::g_rmbClickIndex = 0;
int Keystrokes::g_lmbCps = 0;
int Keystrokes::g_rmbCps = 0;
bool Keystrokes::g_prevLmbPressed = false;
bool Keystrokes::g_prevRmbPressed = false;


void Keystrokes::Initialize(HudElement* hud) {
    g_keystrokesHud = hud;
}

void Keystrokes::UpdateAnimation(ULONGLONG now) {
    // ⌨️ Keystrokes Animation - Fade in/out exponencial
    if (g_showKeystrokes && g_keystrokesEnableTime == 0) {
        g_keystrokesEnableTime = now;
        g_keystrokesDisableTime = 0;
    }
    if (!g_showKeystrokes && g_keystrokesDisableTime == 0 && g_keystrokesEnableTime > 0) {
        g_keystrokesDisableTime = now;
        g_keystrokesEnableTime = 0;
    }
    
    if (g_keystrokesEnableTime > 0) {
        float enableElapsed = (float)(now - g_keystrokesEnableTime) / 1000.0f;
        g_keystrokesAnim = fminf(1.0f, enableElapsed / 0.4f);
    }
    else if (g_keystrokesDisableTime > 0) {
        float disableElapsed = (float)(now - g_keystrokesDisableTime) / 1000.0f;
        float disableAnim = fminf(1.0f, disableElapsed / 0.3f);  // 300ms para desaparecer
        g_keystrokesAnim = 1.0f - disableAnim;
        if (disableAnim >= 1.0f) {
            g_keystrokesEnableTime = 0;
            g_keystrokesDisableTime = 0;
        }
    }
}

void Keystrokes::RenderArrayList(ImDrawList* draw, ImVec2 arrayListStart, float& yPos, ImVec2& arrayListEnd) {
    // Keystrokes module in array list
    if (g_showKeystrokes || (g_keystrokesDisableTime > 0 && g_keystrokesAnim > 0.01f)) {
        float keystrokesAlpha = g_keystrokesAnim * 255.0f;
        float slideOffset = -60.0f + (Animations::SmoothInertia(g_keystrokesAnim) * 60.0f);
        
        if (keystrokesAlpha > 1.0f) {
            char kBuf[64];
            sprintf_s(kBuf, "Keystrokes");
            float wK = ImGui::CalcTextSize(kBuf).x;
            float xPosK = arrayListStart.x + 300.0f - wK - 10;  // Adjusted from hardcoded 290
            draw->AddText(ImVec2(xPosK + slideOffset - 1, yPos + 1), IM_COL32(0, 0, 0, 220), kBuf); // Sombra
            draw->AddText(ImVec2(xPosK + slideOffset, yPos), IM_COL32(100, 200, 255, (int)keystrokesAlpha), kBuf);
            yPos += 18.0f;
            arrayListEnd.y = yPos;
        }
    }
}

void Keystrokes::RenderDisplay(float sw, float sh) {
    if (g_showKeystrokes || g_keystrokesAnim > 0.01f) {
        // Update color lerping based on key states
        bool handlerRes[7] = {
            (GetAsyncKeyState('W') & 0x8000) != 0,
            (GetAsyncKeyState('A') & 0x8000) != 0,
            (GetAsyncKeyState('S') & 0x8000) != 0,
            (GetAsyncKeyState('D') & 0x8000) != 0,
            (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0,
            (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0,
            (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0
        };

        // Update glow modifier based on key state
        for (int i = 0; i < 7; i++) {
            if (handlerRes[i]) {
                g_keystrokesStates[i] = LerpImVec4(g_keystrokesStates[i], g_keystrokesEnabledColor, 0.15f * g_keystrokesEdSpeed);
                g_keystrokesTextStates[i] = LerpImVec4(g_keystrokesTextStates[i], g_keystrokesTextEnabledColor, 0.15f * g_keystrokesEdSpeed);
                if (g_keystrokesTextShadow) {
                    g_keystrokesShadowStates[i] = LerpImVec4(g_keystrokesShadowStates[i], g_keystrokesEnabledShadowColor, 0.15f * g_keystrokesEdSpeed);
                }
                if (g_keystrokesGlowEnabled) {
                    g_keystrokesGlowModifier[i] = fminf(1.0f, g_keystrokesGlowModifier[i] + 0.2f * g_keystrokesGlowSpeed);
                }
            } else {
                g_keystrokesStates[i] = LerpImVec4(g_keystrokesStates[i], g_keystrokesBgColor, 0.15f * g_keystrokesEdSpeed);
                g_keystrokesTextStates[i] = LerpImVec4(g_keystrokesTextStates[i], g_keystrokesTextColor, 0.15f * g_keystrokesEdSpeed);
                if (g_keystrokesTextShadow) {
                    g_keystrokesShadowStates[i] = LerpImVec4(g_keystrokesShadowStates[i], g_keystrokesDisabledShadowColor, 0.15f * g_keystrokesEdSpeed);
                }
                if (g_keystrokesGlowEnabled) {
                    g_keystrokesGlowModifier[i] = fmaxf(0.0f, g_keystrokesGlowModifier[i] - 0.1f * g_keystrokesGlowSpeed);
                }
            }
        }

        float easedAnim = Animations::EaseOutExpo(g_keystrokesAnim);
        float keystrokesAlpha = easedAnim * 255.0f;
        
        // Keystrokes layout size calculation
        float scaledSize = 130.0f * g_keystrokesUIScale;
        float scaledSpacebarWidth = (scaledSize * 3.0f + g_keystrokesKeySpacing * 2.0f * g_keystrokesUIScale) * g_keystrokesSpacebarWidth;
        float keySpacing = g_keystrokesKeySpacing * g_keystrokesUIScale;
        float keyHeight = 38.0f * g_keystrokesUIScale;
        ImVec2 keystrokesSize = ImVec2(
            scaledSize, 
            scaledSize * 1.15f + 
            (g_keystrokesShowMouseButtons ? (scaledSize * 0.5f) : 0.0f) +
            (g_keystrokesShowSpacebar ? (keyHeight + keySpacing) : 0.0f)
        );
        g_keystrokesHud->size = keystrokesSize;
        
        // Initialize position on first draw
        if (g_keystrokesHud->pos.y == 0) {
            g_keystrokesHud->pos = ImVec2(30, sh - 250);
        }
        
        // Handle drag and clamp when menu open
        extern bool g_showMenu;
        if (g_showMenu) {
            g_keystrokesHud->HandleDrag(true);
            g_keystrokesHud->ClampToScreen();
        }
        
        // Position with smooth animation
        float animatedYOffset = 150.0f - (150.0f * easedAnim);
        ImVec2 finalPos = ImVec2(
            g_keystrokesHud->pos.x,
            g_keystrokesHud->pos.y + animatedYOffset
        );
        
        if (finalPos.x < -10000 || finalPos.x > 10000) finalPos.x = 30;
        if (finalPos.y < -10000 || finalPos.y > 10000) finalPos.y = sh - 250;
        
        ImDrawList* keystrokesDraw = ImGui::GetForegroundDrawList();
        if (keystrokesDraw && keystrokesAlpha > 1.0f) {
            float baseX = finalPos.x;
            float baseY = finalPos.y;
            float containerWidth = scaledSize;
            float padding = 6.0f * g_keystrokesUIScale;
            float rounding = g_keystrokesRounding * g_keystrokesUIScale;
            float textShadowOffset = g_keystrokesTextShadowOffset * g_keystrokesUIScale * 10.0f;
            
            // Apply blur effect to background if enabled
            if (g_keystrokesShowBg && g_keystrokesBlurEffect) {
                keystrokesDraw->AddRectFilled(finalPos, ImVec2(finalPos.x + keystrokesSize.x, finalPos.y + keystrokesSize.y), IM_COL32(0, 0, 0, 20), rounding);
            }
            
            // Draw background shadow if enabled
            if (g_keystrokesShowBg && g_keystrokesRectShadow) {
                float shadowOffset = g_keystrokesRectShadowOffset * g_keystrokesUIScale * 10.0f;
                keystrokesDraw->AddRectFilled(
                    ImVec2(finalPos.x + shadowOffset, finalPos.y + shadowOffset),
                    ImVec2(finalPos.x + keystrokesSize.x + shadowOffset, finalPos.y + keystrokesSize.y + shadowOffset),
                    ImGui::GetColorU32(g_keystrokesRectShadowColor),
                    rounding
                );
            }
            
            // Draw main background if enabled
            if (g_keystrokesShowBg) {
                keystrokesDraw->AddRectFilled(finalPos, ImVec2(finalPos.x + keystrokesSize.x, finalPos.y + keystrokesSize.y), ImGui::GetColorU32(g_keystrokesBgColor), rounding);
            }
            
            bool wPressed = handlerRes[0];
            bool aPressed = handlerRes[1];
            bool sPressed = handlerRes[2];
            bool dPressed = handlerRes[3];
            bool lmbPressed = handlerRes[4];
            bool rmbPressed = handlerRes[5];
            bool spacePressed = handlerRes[6];
            
            ImFont* keystrokesFont = ImGui::GetFont();
            if (keystrokesFont) {
                float asdWidth = (containerWidth - (2 * keySpacing)) / 3.0f;
                float wWidth = asdWidth;
                float wX = baseX + (containerWidth - wWidth) / 2.0f;
                ImVec2 wPos = ImVec2(wX, baseY + padding);

                // W Key - with lerped colors and glow
                ImU32 wBgCol = ImGui::GetColorU32(g_keystrokesStates[0]);
                
                // Glow for disabled state
                if (g_keystrokesGlow && g_keystrokesGlowModifier[0] > 0.05f) {
                    ImU32 glowCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowColor.x, g_keystrokesGlowColor.y, g_keystrokesGlowColor.z, g_keystrokesGlowColor.w * g_keystrokesGlowModifier[0] * (g_keystrokesGlowAmount / 100.0f)));
                    keystrokesDraw->AddRect(wPos, ImVec2(wPos.x + wWidth, wPos.y + keyHeight), glowCol, rounding);
                }
                
                // Glow for enabled state
                if (g_keystrokesGlowEnabled && wPressed && g_keystrokesGlowModifier[0] > 0.05f) {
                    ImU32 glowEnabledCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowEnabledColor.x, g_keystrokesGlowEnabledColor.y, g_keystrokesGlowEnabledColor.z, g_keystrokesGlowEnabledColor.w * g_keystrokesGlowModifier[0] * (g_keystrokesGlowEnabledAmount / 100.0f)));
                    keystrokesDraw->AddRect(wPos, ImVec2(wPos.x + wWidth, wPos.y + keyHeight), glowEnabledCol, rounding);
                }
                
                keystrokesDraw->AddRectFilled(wPos, ImVec2(wPos.x + wWidth, wPos.y + keyHeight), wBgCol, rounding);
                if (g_keystrokesBorder) {
                    keystrokesDraw->AddRect(wPos, ImVec2(wPos.x + wWidth, wPos.y + keyHeight), ImGui::GetColorU32(g_keystrokesBorderColor), rounding, 0, g_keystrokesBorderWidth);
                }
                
                float textPosX = wPos.x + (12.0f * g_keystrokesUIScale) + (g_keystrokesTextXOffset * g_keystrokesUIScale);
                float textPosY = wPos.y + (8.0f * g_keystrokesUIScale) + (g_keystrokesTextYOffset * g_keystrokesUIScale);
                float wasdTextScale = 16.0f * g_keystrokesUIScale * g_keystrokesTextScale;
                
                if (g_keystrokesTextShadow) {
                    keystrokesDraw->AddText(keystrokesFont, wasdTextScale, ImVec2(textPosX + textShadowOffset, textPosY + textShadowOffset), ImGui::GetColorU32(g_keystrokesShadowStates[0]), g_keystrokesWText.c_str());
                }
                keystrokesDraw->AddText(keystrokesFont, wasdTextScale, ImVec2(textPosX, textPosY), ImGui::GetColorU32(g_keystrokesTextStates[0]), g_keystrokesWText.c_str());
                
                float row2Y = baseY + padding + keyHeight + keySpacing;
                
                // A Key
                ImVec2 aPos = ImVec2(baseX, row2Y);
                ImU32 aBgCol = ImGui::GetColorU32(g_keystrokesStates[1]);
                
                if (g_keystrokesGlow && g_keystrokesGlowModifier[1] > 0.05f) {
                    ImU32 glowCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowColor.x, g_keystrokesGlowColor.y, g_keystrokesGlowColor.z, g_keystrokesGlowColor.w * g_keystrokesGlowModifier[1] * (g_keystrokesGlowAmount / 100.0f)));
                    keystrokesDraw->AddRect(aPos, ImVec2(aPos.x + asdWidth, aPos.y + keyHeight), glowCol, rounding);
                }
                
                if (g_keystrokesGlowEnabled && aPressed && g_keystrokesGlowModifier[1] > 0.05f) {
                    ImU32 glowEnabledCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowEnabledColor.x, g_keystrokesGlowEnabledColor.y, g_keystrokesGlowEnabledColor.z, g_keystrokesGlowEnabledColor.w * g_keystrokesGlowModifier[1] * (g_keystrokesGlowEnabledAmount / 100.0f)));
                    keystrokesDraw->AddRect(aPos, ImVec2(aPos.x + asdWidth, aPos.y + keyHeight), glowEnabledCol, rounding);
                }
                
                keystrokesDraw->AddRectFilled(aPos, ImVec2(aPos.x + asdWidth, aPos.y + keyHeight), aBgCol, rounding);
                if (g_keystrokesBorder) {
                    keystrokesDraw->AddRect(aPos, ImVec2(aPos.x + asdWidth, aPos.y + keyHeight), ImGui::GetColorU32(g_keystrokesBorderColor), rounding, 0, g_keystrokesBorderWidth);
                }
                
                float aTextPosX = aPos.x + (10.0f * g_keystrokesUIScale) + (g_keystrokesTextXOffset * g_keystrokesUIScale);
                float aTextPosY = aPos.y + (6.0f * g_keystrokesUIScale) + (g_keystrokesTextYOffset * g_keystrokesUIScale);
                float asdTextScale = 18.0f * g_keystrokesUIScale * g_keystrokesTextScale;
                
                if (g_keystrokesTextShadow) {
                    keystrokesDraw->AddText(keystrokesFont, asdTextScale, ImVec2(aTextPosX + textShadowOffset, aTextPosY + textShadowOffset), ImGui::GetColorU32(g_keystrokesShadowStates[1]), g_keystrokesAText.c_str());
                }
                keystrokesDraw->AddText(keystrokesFont, asdTextScale, ImVec2(aTextPosX, aTextPosY), ImGui::GetColorU32(g_keystrokesTextStates[1]), g_keystrokesAText.c_str());
                
                // S Key
                ImVec2 sPos = ImVec2(baseX + asdWidth + keySpacing, row2Y);
                ImU32 sBgCol = ImGui::GetColorU32(g_keystrokesStates[2]);
                
                if (g_keystrokesGlow && g_keystrokesGlowModifier[2] > 0.05f) {
                    ImU32 glowCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowColor.x, g_keystrokesGlowColor.y, g_keystrokesGlowColor.z, g_keystrokesGlowColor.w * g_keystrokesGlowModifier[2] * (g_keystrokesGlowAmount / 100.0f)));
                    keystrokesDraw->AddRect(sPos, ImVec2(sPos.x + asdWidth, sPos.y + keyHeight), glowCol, rounding);
                }
                
                if (g_keystrokesGlowEnabled && sPressed && g_keystrokesGlowModifier[2] > 0.05f) {
                    ImU32 glowEnabledCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowEnabledColor.x, g_keystrokesGlowEnabledColor.y, g_keystrokesGlowEnabledColor.z, g_keystrokesGlowEnabledColor.w * g_keystrokesGlowModifier[2] * (g_keystrokesGlowEnabledAmount / 100.0f)));
                    keystrokesDraw->AddRect(sPos, ImVec2(sPos.x + asdWidth, sPos.y + keyHeight), glowEnabledCol, rounding);
                }
                
                keystrokesDraw->AddRectFilled(sPos, ImVec2(sPos.x + asdWidth, sPos.y + keyHeight), sBgCol, rounding);
                if (g_keystrokesBorder) {
                    keystrokesDraw->AddRect(sPos, ImVec2(sPos.x + asdWidth, sPos.y + keyHeight), ImGui::GetColorU32(g_keystrokesBorderColor), rounding, 0, g_keystrokesBorderWidth);
                }
                
                float sTextPosX = sPos.x + (10.0f * g_keystrokesUIScale) + (g_keystrokesTextXOffset * g_keystrokesUIScale);
                float sTextPosY = sPos.y + (6.0f * g_keystrokesUIScale) + (g_keystrokesTextYOffset * g_keystrokesUIScale);
                
                if (g_keystrokesTextShadow) {
                    keystrokesDraw->AddText(keystrokesFont, asdTextScale, ImVec2(sTextPosX + textShadowOffset, sTextPosY + textShadowOffset), ImGui::GetColorU32(g_keystrokesShadowStates[2]), g_keystrokesSText.c_str());
                }
                keystrokesDraw->AddText(keystrokesFont, asdTextScale, ImVec2(sTextPosX, sTextPosY), ImGui::GetColorU32(g_keystrokesTextStates[2]), g_keystrokesSText.c_str());
                
                // D Key
                ImVec2 dPos = ImVec2(baseX + (asdWidth + keySpacing) * 2, row2Y);
                ImU32 dBgCol = ImGui::GetColorU32(g_keystrokesStates[3]);
                
                if (g_keystrokesGlow && g_keystrokesGlowModifier[3] > 0.05f) {
                    ImU32 glowCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowColor.x, g_keystrokesGlowColor.y, g_keystrokesGlowColor.z, g_keystrokesGlowColor.w * g_keystrokesGlowModifier[3] * (g_keystrokesGlowAmount / 100.0f)));
                    keystrokesDraw->AddRect(dPos, ImVec2(dPos.x + asdWidth, dPos.y + keyHeight), glowCol, rounding);
                }
                
                if (g_keystrokesGlowEnabled && dPressed && g_keystrokesGlowModifier[3] > 0.05f) {
                    ImU32 glowEnabledCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowEnabledColor.x, g_keystrokesGlowEnabledColor.y, g_keystrokesGlowEnabledColor.z, g_keystrokesGlowEnabledColor.w * g_keystrokesGlowModifier[3] * (g_keystrokesGlowEnabledAmount / 100.0f)));
                    keystrokesDraw->AddRect(dPos, ImVec2(dPos.x + asdWidth, dPos.y + keyHeight), glowEnabledCol, rounding);
                }
                
                keystrokesDraw->AddRectFilled(dPos, ImVec2(dPos.x + asdWidth, dPos.y + keyHeight), dBgCol, rounding);
                if (g_keystrokesBorder) {
                    keystrokesDraw->AddRect(dPos, ImVec2(dPos.x + asdWidth, dPos.y + keyHeight), ImGui::GetColorU32(g_keystrokesBorderColor), rounding, 0, g_keystrokesBorderWidth);
                }
                
                float dTextPosX = dPos.x + (10.0f * g_keystrokesUIScale) + (g_keystrokesTextXOffset * g_keystrokesUIScale);
                float dTextPosY = dPos.y + (6.0f * g_keystrokesUIScale) + (g_keystrokesTextYOffset * g_keystrokesUIScale);
                
                if (g_keystrokesTextShadow) {
                    keystrokesDraw->AddText(keystrokesFont, asdTextScale, ImVec2(dTextPosX + textShadowOffset, dTextPosY + textShadowOffset), ImGui::GetColorU32(g_keystrokesShadowStates[3]), g_keystrokesDText.c_str());
                }
                keystrokesDraw->AddText(keystrokesFont, asdTextScale, ImVec2(dTextPosX, dTextPosY), ImGui::GetColorU32(g_keystrokesTextStates[3]), g_keystrokesDText.c_str());
                
                // Row 3: LMB, RMB / Spacebar  
                float row3Y = row2Y + keyHeight + keySpacing;
                
                // Mouse button handling
                bool lmbPressed = handlerRes[4];
                bool rmbPressed = handlerRes[5];
                bool spacePressed = handlerRes[6];
                
                // Track CPS for LMB
                if (lmbPressed && !g_prevLmbPressed) {
                    g_lmbClickTimes[g_lmbClickIndex] = GetTickCount64();
                    g_lmbClickIndex = (g_lmbClickIndex + 1) % 10;
                    int count = 0;
                    for (int i = 0; i < 10; i++) {
                        if (g_lmbClickTimes[i] > 0 && (GetTickCount64() - g_lmbClickTimes[i]) < 1000) {
                            count++;
                        }
                    }
                    g_lmbCps = count;
                }
                g_prevLmbPressed = lmbPressed;
                
                // Track CPS for RMB
                if (rmbPressed && !g_prevRmbPressed) {
                    g_rmbClickTimes[g_rmbClickIndex] = GetTickCount64();
                    g_rmbClickIndex = (g_rmbClickIndex + 1) % 10;
                    int count = 0;
                    for (int i = 0; i < 10; i++) {
                        if (g_rmbClickTimes[i] > 0 && (GetTickCount64() - g_rmbClickTimes[i]) < 1000) {
                            count++;
                        }
                    }
                    g_rmbCps = count;
                }
                g_prevRmbPressed = rmbPressed;
                
                if (g_keystrokesShowMouseButtons) {
                    float lmbRmbWidth = (containerWidth - keySpacing) / 2.0f;
                    
                    // LMB
                    ImVec2 lmbPos = ImVec2(baseX, row3Y);
                    ImU32 lmbBgCol = ImGui::GetColorU32(g_keystrokesStates[4]);
                    
                    if (g_keystrokesGlow && g_keystrokesGlowModifier[4] > 0.05f) {
                        ImU32 glowCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowColor.x, g_keystrokesGlowColor.y, g_keystrokesGlowColor.z, g_keystrokesGlowColor.w * g_keystrokesGlowModifier[4] * (g_keystrokesGlowAmount / 100.0f)));
                        keystrokesDraw->AddRect(lmbPos, ImVec2(lmbPos.x + lmbRmbWidth, lmbPos.y + keyHeight), glowCol, rounding);
                    }
                    
                    if (g_keystrokesGlowEnabled && lmbPressed && g_keystrokesGlowModifier[4] > 0.05f) {
                        ImU32 glowEnabledCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowEnabledColor.x, g_keystrokesGlowEnabledColor.y, g_keystrokesGlowEnabledColor.z, g_keystrokesGlowEnabledColor.w * g_keystrokesGlowModifier[4] * (g_keystrokesGlowEnabledAmount / 100.0f)));
                        keystrokesDraw->AddRect(lmbPos, ImVec2(lmbPos.x + lmbRmbWidth, lmbPos.y + keyHeight), glowEnabledCol, rounding);
                    }
                    
                    keystrokesDraw->AddRectFilled(lmbPos, ImVec2(lmbPos.x + lmbRmbWidth, lmbPos.y + keyHeight), lmbBgCol, rounding);
                    if (g_keystrokesBorder) {
                        keystrokesDraw->AddRect(lmbPos, ImVec2(lmbPos.x + lmbRmbWidth, lmbPos.y + keyHeight), ImGui::GetColorU32(g_keystrokesBorderColor), rounding, 0, g_keystrokesBorderWidth);
                    }
                    
                    float lmbTextPosX = lmbPos.x + (15.0f * g_keystrokesUIScale) + (g_keystrokesTextXOffset * g_keystrokesUIScale);
                    float lmbTextPosY = lmbPos.y + (3.0f * g_keystrokesUIScale) + (g_keystrokesTextYOffset * g_keystrokesUIScale);
                    float mouseTextScale = 14.0f * g_keystrokesUIScale * g_keystrokesTextScale2;
                    
                    if (g_keystrokesTextShadow) {
                        keystrokesDraw->AddText(keystrokesFont, mouseTextScale, ImVec2(lmbTextPosX + textShadowOffset, lmbTextPosY + textShadowOffset), ImGui::GetColorU32(g_keystrokesShadowStates[4]), g_keystrokesLMBText.c_str());
                    }
                    keystrokesDraw->AddText(keystrokesFont, mouseTextScale, ImVec2(lmbTextPosX, lmbTextPosY), ImGui::GetColorU32(g_keystrokesTextStates[4]), g_keystrokesLMBText.c_str());
                    
                    // LMB CPS text
                    std::string lmbCpsStr = std::to_string(g_lmbCps) + " CPS";
                    float cpsMiniScale = 11.0f * g_keystrokesUIScale * g_keystrokesTextScale2;
                    if (g_keystrokesTextShadow) {
                        keystrokesDraw->AddText(keystrokesFont, cpsMiniScale, ImVec2(lmbTextPosX + textShadowOffset, lmbTextPosY + 19.0f * g_keystrokesUIScale + textShadowOffset), ImGui::GetColorU32(g_keystrokesShadowStates[4]), lmbCpsStr.c_str());
                    }
                    keystrokesDraw->AddText(keystrokesFont, cpsMiniScale, ImVec2(lmbTextPosX, lmbTextPosY + 19.0f * g_keystrokesUIScale), ImGui::GetColorU32(g_keystrokesTextStates[4]), lmbCpsStr.c_str());
                    
                    // RMB
                    if (g_keystrokesShowLMBRMB) {
                        ImVec2 rmbPos = ImVec2(baseX + lmbRmbWidth + keySpacing, row3Y);
                        ImU32 rmbBgCol = ImGui::GetColorU32(g_keystrokesStates[5]);
                        
                        if (g_keystrokesGlow && g_keystrokesGlowModifier[5] > 0.05f) {
                            ImU32 glowCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowColor.x, g_keystrokesGlowColor.y, g_keystrokesGlowColor.z, g_keystrokesGlowColor.w * g_keystrokesGlowModifier[5] * (g_keystrokesGlowAmount / 100.0f)));
                            keystrokesDraw->AddRect(rmbPos, ImVec2(rmbPos.x + lmbRmbWidth, rmbPos.y + keyHeight), glowCol, rounding);
                        }
                        
                        if (g_keystrokesGlowEnabled && rmbPressed && g_keystrokesGlowModifier[5] > 0.05f) {
                            ImU32 glowEnabledCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowEnabledColor.x, g_keystrokesGlowEnabledColor.y, g_keystrokesGlowEnabledColor.z, g_keystrokesGlowEnabledColor.w * g_keystrokesGlowModifier[5] * (g_keystrokesGlowEnabledAmount / 100.0f)));
                            keystrokesDraw->AddRect(rmbPos, ImVec2(rmbPos.x + lmbRmbWidth, rmbPos.y + keyHeight), glowEnabledCol, rounding);
                        }
                        
                        keystrokesDraw->AddRectFilled(rmbPos, ImVec2(rmbPos.x + lmbRmbWidth, rmbPos.y + keyHeight), rmbBgCol, rounding);
                        if (g_keystrokesBorder) {
                            keystrokesDraw->AddRect(rmbPos, ImVec2(rmbPos.x + lmbRmbWidth, rmbPos.y + keyHeight), ImGui::GetColorU32(g_keystrokesBorderColor), rounding, 0, g_keystrokesBorderWidth);
                        }
                        
                        float rmbTextPosX = rmbPos.x + (15.0f * g_keystrokesUIScale) + (g_keystrokesTextXOffset * g_keystrokesUIScale);
                        float rmbTextPosY = rmbPos.y + (3.0f * g_keystrokesUIScale) + (g_keystrokesTextYOffset * g_keystrokesUIScale);
                        
                        if (g_keystrokesTextShadow) {
                            keystrokesDraw->AddText(keystrokesFont, mouseTextScale, ImVec2(rmbTextPosX + textShadowOffset, rmbTextPosY + textShadowOffset), ImGui::GetColorU32(g_keystrokesShadowStates[5]), g_keystrokesRMBText.c_str());
                        }
                        keystrokesDraw->AddText(keystrokesFont, mouseTextScale, ImVec2(rmbTextPosX, rmbTextPosY), ImGui::GetColorU32(g_keystrokesTextStates[5]), g_keystrokesRMBText.c_str());
                        
                        // RMB CPS text
                        std::string rmbCpsStr = std::to_string(g_rmbCps) + " CPS";
                        if (g_keystrokesTextShadow) {
                            keystrokesDraw->AddText(keystrokesFont, cpsMiniScale, ImVec2(rmbTextPosX + textShadowOffset, rmbTextPosY + 19.0f * g_keystrokesUIScale + textShadowOffset), ImGui::GetColorU32(g_keystrokesShadowStates[5]), rmbCpsStr.c_str());
                        }
                        keystrokesDraw->AddText(keystrokesFont, cpsMiniScale, ImVec2(rmbTextPosX, rmbTextPosY + 19.0f * g_keystrokesUIScale), ImGui::GetColorU32(g_keystrokesTextStates[5]), rmbCpsStr.c_str());
                    }
                }
                
                // Row 4: Spacebar
                if (g_keystrokesShowSpacebar) {
                    float row4Y = row3Y + keyHeight + keySpacing;
                    float spacebarWidth = containerWidth;
                    float spacebarHeight = keyHeight * 0.7f;
                    
                    ImVec2 spacebarPos = ImVec2(baseX, row4Y);
                    ImU32 spacebarBgCol = ImGui::GetColorU32(g_keystrokesStates[6]);
                    
                    if (g_keystrokesGlow && g_keystrokesGlowModifier[6] > 0.05f) {
                        ImU32 glowCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowColor.x, g_keystrokesGlowColor.y, g_keystrokesGlowColor.z, g_keystrokesGlowColor.w * g_keystrokesGlowModifier[6] * (g_keystrokesGlowAmount / 100.0f)));
                        keystrokesDraw->AddRect(spacebarPos, ImVec2(spacebarPos.x + spacebarWidth, spacebarPos.y + spacebarHeight), glowCol, rounding);
                    }
                    
                    if (g_keystrokesGlowEnabled && spacePressed && g_keystrokesGlowModifier[6] > 0.05f) {
                        ImU32 glowEnabledCol = ImGui::GetColorU32(ImVec4(g_keystrokesGlowEnabledColor.x, g_keystrokesGlowEnabledColor.y, g_keystrokesGlowEnabledColor.z, g_keystrokesGlowEnabledColor.w * g_keystrokesGlowModifier[6] * (g_keystrokesGlowEnabledAmount / 100.0f)));
                        keystrokesDraw->AddRect(spacebarPos, ImVec2(spacebarPos.x + spacebarWidth, spacebarPos.y + spacebarHeight), glowEnabledCol, rounding);
                    }
                    
                    keystrokesDraw->AddRectFilled(spacebarPos, ImVec2(spacebarPos.x + spacebarWidth, spacebarPos.y + spacebarHeight), spacebarBgCol, rounding);
                    if (g_keystrokesBorder) {
                        keystrokesDraw->AddRect(spacebarPos, ImVec2(spacebarPos.x + spacebarWidth, spacebarPos.y + spacebarHeight), ImGui::GetColorU32(g_keystrokesBorderColor), rounding, 0, g_keystrokesBorderWidth);
                    }
                    
                    // Spacebar text (centered)
                    float spaceTextPosX = spacebarPos.x + (spacebarWidth / 2.0f) - (25.0f * g_keystrokesUIScale / 2.0f) + (g_keystrokesTextXOffset * g_keystrokesUIScale);
                    float spaceTextPosY = spacebarPos.y + (spacebarHeight / 2.0f) - (8.0f * g_keystrokesUIScale / 2.0f) + (g_keystrokesTextYOffset * g_keystrokesUIScale);
                    
                    if (g_keystrokesTextShadow) {
                        keystrokesDraw->AddText(keystrokesFont, 16.0f * g_keystrokesUIScale, ImVec2(spaceTextPosX + textShadowOffset, spaceTextPosY + textShadowOffset), ImGui::GetColorU32(g_keystrokesShadowStates[6]), "___");
                    }
                    keystrokesDraw->AddText(keystrokesFont, 16.0f * g_keystrokesUIScale, ImVec2(spaceTextPosX, spaceTextPosY), ImGui::GetColorU32(g_keystrokesTextStates[6]), "___");
                }
            }
        }
        
        // DEBUG: show hitbox only when menu is open
        if (g_showMenu) {
            ImDrawList* debugDraw = ImGui::GetForegroundDrawList();
            if (debugDraw) {
                debugDraw->AddRect(
                    finalPos,
                    ImVec2(finalPos.x + keystrokesSize.x, finalPos.y + keystrokesSize.y),
                    IM_COL32(0, 150, 255, 200),
                    0.0f,
                    0,
                    2.0f
                );
            }
        }
    }
}

void Keystrokes::RenderMenu() {
    // Show Keystrokes
    ImGui::Checkbox("Keystrokes", &g_showKeystrokes);
    
    if (g_showKeystrokes) {
        ImGui::Separator();
        ImGui::Text("Keystrokes Configuration");
        
        // UI Scale
        ImGui::SliderFloat("Keystrokes Scale", &g_keystrokesUIScale, 0.5f, 2.0f, "%.2f");
        
        // Visual Effects
        ImGui::Checkbox("Blur Effect##KS", &g_keystrokesBlurEffect);
        ImGui::SliderFloat("Keystrokes Rounding", &g_keystrokesRounding, 0.0f, 20.0f, "%.1f");
        
        // Background & Shadow
        ImGui::Checkbox("Show Background##KS", &g_keystrokesShowBg);
        if (g_keystrokesShowBg) {
            ImGui::Checkbox("Background Shadow##KS", &g_keystrokesRectShadow);
            if (g_keystrokesRectShadow) {
                ImGui::SliderFloat("Shadow Offset##KS", &g_keystrokesRectShadowOffset, 0.0f, 0.1f, "%.3f");
            }
        }
        
        // Border
        ImGui::Checkbox("Border##KS", &g_keystrokesBorder);
        if (g_keystrokesBorder) {
            ImGui::SliderFloat("Border Width##KS", &g_keystrokesBorderWidth, 0.5f, 4.0f, "%.1f");
        }
        
        // Glow Effects
        ImGui::Checkbox("Glow (Disabled)##KS", &g_keystrokesGlow);
        if (g_keystrokesGlow) {
            ImGui::SliderFloat("Glow Amount##KS", &g_keystrokesGlowAmount, 0.0f, 100.0f, "%.0f");
        }
        
        ImGui::Checkbox("Glow (Enabled)##KS", &g_keystrokesGlowEnabled);
        if (g_keystrokesGlowEnabled) {
            ImGui::SliderFloat("Enabled Glow Amount##KS", &g_keystrokesGlowEnabledAmount, 0.0f, 100.0f, "%.0f");
            ImGui::SliderFloat("Glow Speed##KS", &g_keystrokesGlowSpeed, 0.1f, 10.0f, "%.1f");
        }
        
        // Spacing & Speed
        ImGui::SliderFloat("Key Spacing##KS", &g_keystrokesKeySpacing, 0.5f, 5.0f, "%.2f");
        ImGui::SliderFloat("Highlight Speed##KS", &g_keystrokesEdSpeed, 0.1f, 10.0f, "%.1f");
        
        // Text Settings
        ImGui::SliderFloat("WASD Text Scale##KS", &g_keystrokesTextScale, 0.5f, 2.0f, "%.2f");
        ImGui::SliderFloat("Mouse Text Scale##KS", &g_keystrokesTextScale2, 0.5f, 2.0f, "%.2f");
        ImGui::SliderFloat("Text X Offset##KS", &g_keystrokesTextXOffset, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Text Y Offset##KS", &g_keystrokesTextYOffset, 0.0f, 1.0f, "%.2f");
        
        ImGui::Checkbox("Text Shadow##KS", &g_keystrokesTextShadow);
        if (g_keystrokesTextShadow) {
            ImGui::SliderFloat("Text Shadow Offset##KS", &g_keystrokesTextShadowOffset, 0.0f, 0.1f, "%.3f");
        }
        
        ImGui::Separator();
        ImGui::Text("Keystrokes Colors");
        
        // Background Colors
        float bgColorDisabled[4] = {g_keystrokesBgColor.x, g_keystrokesBgColor.y, g_keystrokesBgColor.z, g_keystrokesBgColor.w};
        if (ImGui::ColorEdit4("Background Disabled##KS", bgColorDisabled)) {
            g_keystrokesBgColor = ImVec4(bgColorDisabled[0], bgColorDisabled[1], bgColorDisabled[2], bgColorDisabled[3]);
        }
        ImGui::SliderFloat("Background Disabled Opacity##KS", &g_keystrokesBgColor.w, 0.0f, 1.0f, "%.2f");
        
        float bgColorEnabled[4] = {g_keystrokesEnabledColor.x, g_keystrokesEnabledColor.y, g_keystrokesEnabledColor.z, g_keystrokesEnabledColor.w};
        if (ImGui::ColorEdit4("Background Enabled##KS", bgColorEnabled)) {
            g_keystrokesEnabledColor = ImVec4(bgColorEnabled[0], bgColorEnabled[1], bgColorEnabled[2], bgColorEnabled[3]);
        }
        ImGui::SliderFloat("Background Enabled Opacity##KS", &g_keystrokesEnabledColor.w, 0.0f, 1.0f, "%.2f");
        
        // Text Colors
        float textColorDisabled[4] = {g_keystrokesTextColor.x, g_keystrokesTextColor.y, g_keystrokesTextColor.z, g_keystrokesTextColor.w};
        if (ImGui::ColorEdit4("Text Disabled##KS", textColorDisabled)) {
            g_keystrokesTextColor = ImVec4(textColorDisabled[0], textColorDisabled[1], textColorDisabled[2], textColorDisabled[3]);
        }
        ImGui::SliderFloat("Text Disabled Opacity##KS", &g_keystrokesTextColor.w, 0.0f, 1.0f, "%.2f");
        
        float textColorEnabled[4] = {g_keystrokesTextEnabledColor.x, g_keystrokesTextEnabledColor.y, g_keystrokesTextEnabledColor.z, g_keystrokesTextEnabledColor.w};
        if (ImGui::ColorEdit4("Text Enabled##KS", textColorEnabled)) {
            g_keystrokesTextEnabledColor = ImVec4(textColorEnabled[0], textColorEnabled[1], textColorEnabled[2], textColorEnabled[3]);
        }
        ImGui::SliderFloat("Text Enabled Opacity##KS", &g_keystrokesTextEnabledColor.w, 0.0f, 1.0f, "%.2f");
        
        ImGui::Separator();
        ImGui::Text("Advanced Effects");
        
        // Background Shadow Color
        if (g_keystrokesShowBg && g_keystrokesRectShadow) {
            float rectShadowCol[4] = {g_keystrokesRectShadowColor.x, g_keystrokesRectShadowColor.y, g_keystrokesRectShadowColor.z, g_keystrokesRectShadowColor.w};
            if (ImGui::ColorEdit4("Background Shadow Color##KS", rectShadowCol)) {
                g_keystrokesRectShadowColor = ImVec4(rectShadowCol[0], rectShadowCol[1], rectShadowCol[2], rectShadowCol[3]);
            }
            ImGui::SliderFloat("Background Shadow Opacity##KS", &g_keystrokesRectShadowColor.w, 0.0f, 1.0f, "%.2f");
        }
        
        // Text Shadow Color
        if (g_keystrokesTextShadow) {
            float textShadowCol[4] = {g_keystrokesTextShadowColor.x, g_keystrokesTextShadowColor.y, g_keystrokesTextShadowColor.z, g_keystrokesTextShadowColor.w};
            if (ImGui::ColorEdit4("Text Shadow Color##KS", textShadowCol)) {
                g_keystrokesTextShadowColor = ImVec4(textShadowCol[0], textShadowCol[1], textShadowCol[2], textShadowCol[3]);
            }
            ImGui::SliderFloat("Text Shadow Opacity##KS", &g_keystrokesTextShadowColor.w, 0.0f, 1.0f, "%.2f");
        }
        
        // Border Color
        if (g_keystrokesBorder) {
            float borderCol[4] = {g_keystrokesBorderColor.x, g_keystrokesBorderColor.y, g_keystrokesBorderColor.z, g_keystrokesBorderColor.w};
            if (ImGui::ColorEdit4("Border Color##KS", borderCol)) {
                g_keystrokesBorderColor = ImVec4(borderCol[0], borderCol[1], borderCol[2], borderCol[3]);
            }
            ImGui::SliderFloat("Border Opacity##KS", &g_keystrokesBorderColor.w, 0.0f, 1.0f, "%.2f");
        }
        
        // Glow Colors
        if (g_keystrokesGlow) {
            float glowCol[4] = {g_keystrokesGlowColor.x, g_keystrokesGlowColor.y, g_keystrokesGlowColor.z, g_keystrokesGlowColor.w};
            if (ImGui::ColorEdit4("Glow Color (Disabled)##KS", glowCol)) {
                g_keystrokesGlowColor = ImVec4(glowCol[0], glowCol[1], glowCol[2], glowCol[3]);
            }
            ImGui::SliderFloat("Glow Disabled Opacity##KS", &g_keystrokesGlowColor.w, 0.0f, 1.0f, "%.2f");
        }
        
        if (g_keystrokesGlowEnabled) {
            float glowEnabledCol[4] = {g_keystrokesGlowEnabledColor.x, g_keystrokesGlowEnabledColor.y, g_keystrokesGlowEnabledColor.z, g_keystrokesGlowEnabledColor.w};
            if (ImGui::ColorEdit4("Glow Color (Enabled)##KS", glowEnabledCol)) {
                g_keystrokesGlowEnabledColor = ImVec4(glowEnabledCol[0], glowEnabledCol[1], glowEnabledCol[2], glowEnabledCol[3]);
            }
            ImGui::SliderFloat("Glow Enabled Opacity##KS", &g_keystrokesGlowEnabledColor.w, 0.0f, 1.0f, "%.2f");
        }
        
        ImGui::Separator();
        ImGui::Text("Mouse Buttons and Spacebar");
        
        ImGui::Checkbox("Show Mouse Buttons##KS", &g_keystrokesShowMouseButtons);
        ImGui::Checkbox("Show LMB & RMB##KS", &g_keystrokesShowLMBRMB);
        ImGui::Checkbox("Show Spacebar##KS", &g_keystrokesShowSpacebar);
        ImGui::SliderFloat("Spacebar Width##KS", &g_keystrokesSpacebarWidth, 0.1f, 2.0f, "%.2f");
    }
}
