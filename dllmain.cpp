#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <psapi.h>

#include "minhook/MinHook.h"
#include "ImGui/imgui.h"
#include "ImGui/backend/imgui_impl_dx11.h"
#include "ImGui/backend/imgui_impl_win32.h"
#include "Modules/ModuleHeader.hpp"
#include "Animations/Animations.hpp"
#include "GUI/GUI.hpp"
#include "Hook/Hook.hpp"
#include "Input/Input.hpp"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "psapi.lib");

// TODO: Refactor into multiple files and classes for better organization and maintainability

// Variable declarations and other code...

// Entity motion packet structure

// Declarations and interfaces
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ImGui configuration flags
ImGuiConfigFlags g_imguiConfigFlags = ImGuiConfigFlags_None;

// Global variables
WNDPROC oWndProc = NULL;

ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView = NULL;
HWND g_window = NULL;

bool g_showMenu = false;
float g_menuAnim = 0.0f;
ULONGLONG g_lastTime = 0, g_lastToggle = 0, g_notifStart = 0;
bool g_vsync = false;
float g_lastW = 0, g_lastH = 0;

// Module animations
ULONGLONG g_hitboxEnableTime = 0, g_hitboxDisableTime = 0;
ULONGLONG g_autoSprintEnableTime = 0, g_autoSprintDisableTime = 0;
ULONGLONG g_fullBrightEnableTime = 0, g_fullBrightDisableTime = 0;
ULONGLONG g_timerEnableTime = 0, g_timerDisableTime = 0;
ULONGLONG g_unlockFpsEnableTime = 0, g_unlockFpsDisableTime = 0;

// Tab animation
int g_currentTab = 0;
int g_previousTab = 0;
ULONGLONG g_tabChangeTime = 0;
float g_tabAnim = 0.0f;

// Keyboard input system
// Keyboard and input state - delegated to Input class

bool g_firstTabOpen = true;

// Memory access helpers
uintptr_t g_gameBase = 0;

// Utility declarations
bool IsWindowsCursorVisible() {
    CURSORINFO ci = { 0 };
    ci.cbSize = sizeof(CURSORINFO);
    if (GetCursorInfo(&ci)) {
        return (ci.flags & CURSOR_SHOWING) != 0;
    }
    return false;
}

bool IsInWorld() {
    // Check world or menu state for cursor visibility
    return !IsWindowsCursorVisible();
}

// Motion blur data - now handled by MotionBlur class

// HUD elements

// Unlock FPS data moved to UnlockFPS.hpp/cpp

// Keystrokes data - now handled by Keystrokes class

// CPS counter data
#define MAX_CPS_HISTORY 100
ULONGLONG g_lmbClickTimes[MAX_CPS_HISTORY] = {};
ULONGLONG g_rmbClickTimes[MAX_CPS_HISTORY] = {};
int g_lmbClickIndex = 0;
int g_rmbClickIndex = 0;
int g_lmbCps = 0;
int g_rmbCps = 0;

bool g_prevLmbPressed = false;
bool g_prevRmbPressed = false;

// Keystrokes config - now handled by Keystrokes class

// CPS Counter variables moved to CPSCounter.hpp/cpp

// Keystrokes mouse buttons config
bool g_keystrokesShowMouseButtons = true;
bool g_keystrokesShowLMBRMB = true;
bool g_keystrokesShowSpacebar = true;
float g_keystrokesSpacebarWidth = 0.5f;
float g_keystrokesSpacebarHeight = 0.09f;

// Keystrokes shadow colors
ImVec4 g_keystrokesDisabledShadowColor = ImVec4(0.0f, 0.0f, 0.0f, 0.55f);
ImVec4 g_keystrokesEnabledShadowColor = ImVec4(0.0f, 0.0f, 0.0f, 0.55f);

// LMB/RMB format variables
std::string g_keystrokesLMBFormatText = "{value} CPS";
std::string g_keystrokesRMBFormatText = "{value} CPS";

// HUD elements
HudElement g_watermarkHud = { ImVec2(10, 10), ImVec2(400, 80) };
HudElement g_renderInfoHud = { ImVec2(10, 100), ImVec2(220, 120) };
HudElement g_arrayListHud = { ImVec2(0, 10), ImVec2(300, 400) };
HudElement g_keystrokesHud = { ImVec2(30, 0), ImVec2(140, 150) };
HudElement g_cpsHud = { ImVec2(500, 400), ImVec2(80, 30) };  // Will be initialized in CPSCounter

// Input blocking
static HHOOK g_keyboardHook = nullptr;

// Keyboard hook
LRESULT CALLBACK KeyboardBlockHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0) {
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }
    
    // Allow all input when menu is open
    if (g_showMenu) {
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }
    
    // Block input when menu is closed
    if (nCode == HC_ACTION) {
        PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;
        if (pKey) {
            // Allow INSERT key
            if (pKey->vkCode == VK_INSERT) {
                return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
            }
            // Block other keys
            return 1;
        }
    }
    
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

void BlockGameInput() {
    // Install hook if needed
    if (!g_keyboardHook) {
        HMODULE hMod = GetModuleHandleA("internal_hook");
        if (!hMod) hMod = GetModuleHandleA(nullptr);
        g_keyboardHook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyboardBlockHookProc, hMod, 0);
    }
}

void UnblockGameInput() {
    // Remove keyboard hook
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
}

// Easing animations now handled by Animations class

// Chroma color function moved to Watermark.cpp

// ImVec4 lerp helper
ImVec4 LerpImVec4(ImVec4 a, ImVec4 b, float t) {
    return ImVec4(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t
    );
}

// CPS format processor moved to CPSCounter.cpp

// Keystrokes format processor
std::string ProcessKeystrokesFormat(const std::string& format, int value) {
    std::string result = format;
    std::string formatUpper = format;
    for (char& c : formatUpper) c = std::toupper(c);
    
    size_t pos = formatUpper.find("{VALUE}");
    if (pos != std::string::npos) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%d", value);
        result.replace(pos, 7, buffer);
    }
    
    return result;
}

// ImGui resize fix
// Sync ImGui + DX11
void SyncImGuiAndDX11(IDXGISwapChain* pSwapChain, float& width, float& height)
{
    DXGI_SWAP_CHAIN_DESC sd;
    pSwapChain->GetDesc(&sd);

    // Fallback for full screen edge cases
    if (width <= 0 || height <= 0)
    {
        RECT rect;
        GetClientRect(sd.OutputWindow, &rect);
        width  = (float)(rect.right - rect.left);
        height = (float)(rect.bottom - rect.top);
    }

    // Real viewport in pixels
    D3D11_VIEWPORT vp;
    vp.Width    = width;
    vp.Height   = height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    pContext->RSSetViewports(1, &vp);

    // ImGui DisplaySize matches backbuffer size
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(width, height);
    
    // DisplayFramebufferScale = 1.0f (backbuffer es la fuente de verdad)
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
}

void* AllocateNear(uintptr_t reference, size_t size) {
    intptr_t deltas[] = { 0x1000000, 0x2000000, 0x4000000, -0x1000000, -0x2000000, -0x4000000 };
    for (int i = 0; i < 6; i++) {
        uintptr_t target = reference + deltas[i];
        void* addr = VirtualAlloc((void*)target, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (addr) {
            // Check if within 32-bit range
            int64_t jmpRel = (uintptr_t)addr - (reference + 5);
            if (jmpRel >= -0x80000000LL && jmpRel <= 0x7FFFFFFFLL) {
                return addr;
            }
            VirtualFree(addr, 0, MEM_RELEASE);
        }
    }
    return NULL; // No near address found
}

uintptr_t PatternScan(uintptr_t start, size_t size, const BYTE* pattern, size_t patternSize) {
    for (size_t i = 0; i <= size - patternSize; ++i) {
        if (memcmp((void*)(start + i), pattern, patternSize) == 0) {
            return start + i;
        }
    }
    return 0;
}

// --- Motion Blur Infrastructure (delegated to MotionBlur class) ---

void ImageWithOpacity(ID3D11ShaderResourceView* srv, ImVec2 size, float opacity) {
    if (!srv || opacity <= 0.0f) {
        return;
    }

    opacity = opacity > 1.0f ? 1.0f : (opacity < 0.0f ? 0.0f : opacity);
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    ImVec2 pos = { 0, 0 };
    ImU32 col = IM_COL32(255, 255, 255, static_cast<int>(opacity * 255.0f));
    draw_list->AddImage((ImTextureID)srv, pos, ImVec2(pos.x + size.x, pos.y + size.y), ImVec2(0, 0), ImVec2(1, 1), col);
}


// Hitbox logic

void InitializeBackbufferStorage(int maxFrames) {
    MotionBlur::InitializeBackbufferStorage(maxFrames);
}

ID3D11ShaderResourceView* CopyBackbufferToSRV(IDXGISwapChain* pSwapChain, ID3D11Device* pDevice, ID3D11DeviceContext* pContext) {
    return MotionBlur::CopyBackbufferToSRV(pDevice, pContext, pSwapChain);
}

void CleanupBackbufferStorage() {
    MotionBlur::CleanupBackbufferStorage();
}

// Hitbox logic
// --- 🏃 AUTOSPRINT ---
// --- 🖱️ INPUT HOOKS DELEGADOS A Hook.cpp ---

LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (g_showMenu) {
        // Don't pass to ImGui - manual input handling
        // Only blocks input when menu is open
        switch (uMsg) {
            case WM_MOUSEMOVE: case WM_LBUTTONDOWN: case WM_LBUTTONUP:
            case WM_RBUTTONDOWN: case WM_RBUTTONUP: case WM_MBUTTONDOWN:
            case WM_MBUTTONUP: case WM_MOUSEWHEEL: case WM_INPUT: 
                return 1;
        }
    }
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

void CleanupRenderTarget() {
    if (mainRenderTargetView) { 
        mainRenderTargetView->Release(); 
        mainRenderTargetView = NULL; 
    }
}

// --- 🎨 RENDER PRINCIPAL ---
HRESULT STDMETHODCALLTYPE hkPresent_Impl(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    // Force VSync OFF when disabled
    if (!g_vsync)
        SyncInterval = 0;
        
    if (!pDevice) {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice))) {
            pDevice->GetImmediateContext(&pContext);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            g_window = sd.OutputWindow;
            if (!g_window) g_window = GetForegroundWindow();

            ImGui::CreateContext();
            ImGui_ImplWin32_Init(g_window);
            ImGui_ImplDX11_Init(pDevice, pContext);
            
            // Cargar fuente Segoe UI del sistema
            ImGuiIO& io = ImGui::GetIO();
            io.IniFilename = NULL;  // No generar .ini
            // Arial
            io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 16.0f);
            ImGui_ImplDX11_CreateDeviceObjects();
            
            // 🎨 APLICAR TEMA
            GUI::ApplyTheme();
            
            // Inicializar watermark para que aparezca por defecto
            Watermark::g_watermarkEnableTime = GetTickCount64();
            Watermark::g_watermarkAnim = 1.0f;
            
            oWndProc = (WNDPROC)SetWindowLongPtr(g_window, GWLP_WNDPROC, (LONG_PTR)hkWndProc);
            
            g_gameBase = (uintptr_t)GetModuleHandleA(NULL);
            Reach::Initialize(g_gameBase);
            Hitbox::Initialize(g_gameBase);
            Timer::Initialize(g_gameBase);
            RenderInfo::Initialize(&g_renderInfoHud);
            Watermark::Initialize(&g_watermarkHud);
            Keystrokes::Initialize(&g_keystrokesHud);
            CPSCounter::Initialize(&g_cpsHud);
            Terminal::Initialize();

            // Scan for AutoSprint pattern
            HMODULE hModule = GetModuleHandleA(NULL);
            MODULEINFO mi;
            GetModuleInformation(GetCurrentProcess(), hModule, &mi, sizeof(mi));
            if (!AutoSprint::g_autoSprintAddr) {
                BYTE pattern[] = {0x0F, 0xB6, 0x41, 0x63, 0x48, 0x8D, 0x2D, 0x39, 0xE0, 0xC3, 0x00};
                AutoSprint::g_autoSprintAddr = PatternScan(g_gameBase, mi.SizeOfImage, pattern, sizeof(pattern));
            }

            // Scan for FullBright pattern
            if (!FullBright::g_fullBrightAddr) {
                BYTE pattern[] = {0xF3, 0x0F, 0x10, 0x80, 0xA0, 0x01, 0x00, 0x00};
                FullBright::g_fullBrightAddr = PatternScan(g_gameBase, mi.SizeOfImage, pattern, sizeof(pattern));
            }

            g_notifStart = GetTickCount64();
            g_lastTime = GetTickCount64();
        }
    }

    ID3D11Texture2D* pBackBuffer = NULL;
    float sw = 0, sh = 0;
    if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer))) {
        D3D11_TEXTURE2D_DESC desc;
        pBackBuffer->GetDesc(&desc);
        sw = (float)desc.Width; 
        sh = (float)desc.Height;
        if (!mainRenderTargetView) pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
        
        // Motion blur - capture frame
        if (MotionBlur::g_motionBlurEnabled) {
            // Calculate max frames by blur type
            int maxFrames = 1;
            if (MotionBlur::g_blurType == "Time Aware Blur") {
                maxFrames = (int)round(MotionBlur::g_maxHistoryFrames);  // Usar g_maxHistoryFrames para Time Aware
            } else if (MotionBlur::g_blurType == "Real Motion Blur") {
                maxFrames = 8;
            } else {
                maxFrames = (int)round(MotionBlur::g_blurIntensity);  // Usar g_blurIntensity para otros modos
            }
            
            if (maxFrames <= 0) maxFrames = 4;
            if (maxFrames > 16) maxFrames = 16;
            
            InitializeBackbufferStorage(maxFrames);
            
            // Copy backbuffer to SRV
            ID3D11ShaderResourceView* srv = CopyBackbufferToSRV(pSwapChain, pDevice, pContext);
            if (srv) {
                // Remove oldest frame if exceeding max frames
                if ((int)MotionBlur::g_previousFrames.size() >= maxFrames) {
                    if (MotionBlur::g_previousFrames[0]) MotionBlur::g_previousFrames[0]->Release();
                    MotionBlur::g_previousFrames.erase(MotionBlur::g_previousFrames.begin());
                    MotionBlur::g_frameTimestamps.erase(MotionBlur::g_frameTimestamps.begin());
                }
                
                MotionBlur::g_previousFrames.push_back(srv);
                MotionBlur::g_frameTimestamps.push_back((float)GetTickCount64() / 1000.0f);
            }
        }
        
        pBackBuffer->Release();
    }

    if (sw <= 0) return Hook::oPresent(pSwapChain, 0, Flags);  // VSync disabled

    // Insert key - open menu
    if ((GetAsyncKeyState(VK_INSERT) & 0x8000) && (GetTickCount64() - g_lastToggle) > 400) {
        g_showMenu = !g_showMenu;
        GUI::g_showMenu = g_showMenu;  // Sincronizar con GUI
        g_lastToggle = GetTickCount64();
        
        if (g_showMenu) {
            // 🎮 BLOQUEAR INPUT DEL JUEGO
            BlockGameInput();
            
            Hook::oClipCursor(NULL);
            // Si estamos en un mundo, ocultar cursor SO (mostrar cursor ImGui)
            if (IsInWorld()) {
                while (ShowCursor(FALSE) >= 0);
            } else {
                // Si NO estamos en un mundo, ocultar cursor ImGui pero mantener SO visible
                // OS cursor visible by default
            }
        } else {
            // 🎮 DESBLOQUEAR INPUT DEL JUEGO
            UnblockGameInput();
            
            // Menu closed - ensure cursor visible
            while (ShowCursor(TRUE) < 0);
        }
    }

    // Motion blur cleanup
    static bool wasMotionBlurEnabled = false;
    if (!MotionBlur::g_motionBlurEnabled && wasMotionBlurEnabled) {
        CleanupBackbufferStorage();
    }
    wasMotionBlurEnabled = MotionBlur::g_motionBlurEnabled;

    // 📊 ANIMATION UPDATE - Simple y elegante
    ULONGLONG now = GetTickCount64();
    float dt = (float)(now - g_lastTime) / 1000.0f;
    g_lastTime = now;
    
    // 📉 MENU ANIMATION - Delegado a GUI
    GUI::UpdateAnimation(now, dt);
    float easedMenuAnim = Animations::SmoothInertia(GUI::g_menuAnim);
    // 📊 Render Info Animation - handled by RenderInfo class
    RenderInfo::UpdateAnimation(now);
    
    // Motion Blur Animation - handled by MotionBlur class
    MotionBlur::UpdateAnimation(now);

    // ⌨️ Keystrokes Animation - handled by Keystrokes class
    Keystrokes::UpdateAnimation(now);
    
    // ⌨️ CPS COUNTER - LMB y RMB
    bool lmbPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    bool rmbPressed = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    
    // LMB CPS Counter
    if (lmbPressed && !g_prevLmbPressed) {
        // Save click timestamp
        g_lmbClickTimes[g_lmbClickIndex] = now;
        g_lmbClickIndex = (g_lmbClickIndex + 1) % MAX_CPS_HISTORY;
        
        // Count clicks in last 1000ms
        int count = 0;
        for (int i = 0; i < MAX_CPS_HISTORY; i++) {
            if (g_lmbClickTimes[i] > 0 && (now - g_lmbClickTimes[i]) < 1000) {
                count++;
            }
        }
        g_lmbCps = count;
    }
    g_prevLmbPressed = lmbPressed;
    
    // RMB CPS Counter
    if (rmbPressed && !g_prevRmbPressed) {
        // Save click timestamp
        g_rmbClickTimes[g_rmbClickIndex] = now;
        g_rmbClickIndex = (g_rmbClickIndex + 1) % MAX_CPS_HISTORY;
        
        // Count clicks in last 1000ms
        int count = 0;
        for (int i = 0; i < MAX_CPS_HISTORY; i++) {
            if (g_rmbClickTimes[i] > 0 && (now - g_rmbClickTimes[i]) < 1000) {
                count++;
            }
        }
        g_rmbCps = count;
    }
    g_prevRmbPressed = rmbPressed;
    
    // CPS Counter Animation - Fade in/out exponencial
    CPSCounter::UpdateAnimation(now);
    
    // Watermark Animation (using Watermark class)
    Watermark::UpdateAnimation(now);

    ImGui_ImplDX11_NewFrame();
    // Don't use ImGui_ImplWin32_NewFrame - manual input handling
    
    // Update input system (keyboard and mouse) - delegated to Input class
    bool drawImGuiCursor = (g_showMenu && IsInWorld());
    Input::Update(g_window, sw, sh, g_showMenu, drawImGuiCursor);

    ImGui::NewFrame();

    // Motion blur - apply to background at frame start
    if (MotionBlur::g_motionBlurEnabled && !g_showMenu && MotionBlur::g_previousFrames.size() > 0 && MotionBlur::g_motionBlurAnim > 0.01f) {
        float currentTime = (float)GetTickCount64() / 1000.0f;
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        ImDrawList* blurDraw = ImGui::GetBackgroundDrawList();
        
        // Average Pixel Blur: Promedio de frames con falloff
        if (MotionBlur::g_blurType == "Average Pixel Blur") {
            float alpha = 0.25f;
            float bleedFactor = 0.95f;
            for (const auto& frame : MotionBlur::g_previousFrames) {
                if (frame) {
                    ImU32 col = IM_COL32(255, 255, 255, (int)(alpha * MotionBlur::g_motionBlurAnim * 255.0f));
                    blurDraw->AddImage((ImTextureID)frame, ImVec2(0, 0), screenSize, ImVec2(0, 0), ImVec2(1, 1), col);
                    alpha *= bleedFactor;
                }
            }
        } 
        // Ghost frames: More visible
        else if (MotionBlur::g_blurType == "Ghost Frames") {
            float alpha = 0.30f;
            float bleedFactor = 0.80f;
            for (const auto& frame : MotionBlur::g_previousFrames) {
                if (frame) {
                    ImU32 col = IM_COL32(255, 255, 255, (int)(alpha * MotionBlur::g_motionBlurAnim * 255.0f));
                    blurDraw->AddImage((ImTextureID)frame, ImVec2(0, 0), screenSize, ImVec2(0, 0), ImVec2(1, 1), col);
                    alpha *= bleedFactor;
                }
            }
        }
        // Time Aware Blur: Decay exponencial
        else if (MotionBlur::g_blurType == "Time Aware Blur") {
            float T = MotionBlur::g_blurTimeConstant;
            std::vector<float> weights;
            float totalWeight = 0.0f;
            
            for (size_t i = 0; i < MotionBlur::g_previousFrames.size(); i++) {
                float age = currentTime - MotionBlur::g_frameTimestamps[i];
                float weight = expf(-age / T);
                weights.push_back(weight);
                totalWeight += weight;
            }
            
            if (totalWeight > 0.0f) {
                for (float& w : weights) {
                    w /= totalWeight;
                }
            }
            
            for (size_t i = 0; i < MotionBlur::g_previousFrames.size(); i++) {
                if (MotionBlur::g_previousFrames[i] && weights[i] > 0.001f) {
                    ImU32 col = IM_COL32(255, 255, 255, (int)(weights[i] * MotionBlur::g_motionBlurAnim * 255.0f));
                    blurDraw->AddImage((ImTextureID)MotionBlur::g_previousFrames[i], ImVec2(0, 0), screenSize, ImVec2(0, 0), ImVec2(1, 1), col);
                }
            }
        }
        // Real Motion Blur: Trails suaves
        else if (MotionBlur::g_blurType == "Real Motion Blur") {
            float alpha = 0.35f;
            float bleedFactor = 0.85f;
            for (const auto& frame : MotionBlur::g_previousFrames) {
                if (frame) {
                    ImU32 col = IM_COL32(255, 255, 255, (int)(alpha * MotionBlur::g_motionBlurAnim * 255.0f));
                    blurDraw->AddImage((ImTextureID)frame, ImVec2(0, 0), screenSize, ImVec2(0, 0), ImVec2(1, 1), col);
                    alpha *= bleedFactor;
                }
            }
        }
        // V4 (Onix-style)
        else if (MotionBlur::g_blurType == "V4") {
            float alpha = 0.35f;
            float bleedFactor = 0.85f;
            for (const auto& frame : MotionBlur::g_previousFrames) {
                if (frame) {
                    ImU32 col = IM_COL32(255, 255, 255, (int)(alpha * MotionBlur::g_motionBlurAnim * 255.0f));
                    blurDraw->AddImage((ImTextureID)frame, ImVec2(0, 0), screenSize, ImVec2(0, 0), ImVec2(1, 1), col);
                    alpha *= bleedFactor;
                }
            }
        }
    }

    // ArrayList - always visible top right
    // Handle drag for ArrayList
    g_arrayListHud.HandleDrag(g_showMenu);
    g_arrayListHud.ClampToScreen();

    // Initialize arrayList position to top-right on first frame
    if (g_arrayListHud.pos.x == 0 && g_arrayListHud.pos.y == 10) {
        g_arrayListHud.pos.x = sw - 250;  // Right side with padding
    }

    ImDrawList* draw = ImGui::GetForegroundDrawList();
    ImVec2 arrayListStart = g_arrayListHud.pos;
    ImVec2 arrayListEnd = arrayListStart;
    float yPos = arrayListStart.y;
    char rBuf[64], hBuf[64];
    const float FADE_OUT_TIME = 0.15f;
    const float FADE_IN_TIME = 0.12f;
    const float SLIDE_TIME = 0.25f;

    // Reach module (always visible)
    Reach::RenderArrayList(draw, arrayListStart, yPos, arrayListEnd);

    // Hitbox module (using Hitbox class)
    Hitbox::RenderArrayList(draw, arrayListStart, yPos, arrayListEnd);

    // AutoSprint module (using AutoSprint class)
    AutoSprint::RenderArrayList(draw, arrayListStart, yPos, arrayListEnd);

    // FullBright module (using FullBright class)
    FullBright::RenderArrayList(draw, arrayListStart, yPos, arrayListEnd);

    // Timer module (using Timer class)
        Timer::RenderArrayList(draw, arrayListStart, yPos, arrayListEnd);

        // Unlock FPS module (using UnlockFPS class)
        UnlockFPS::RenderArrayList(draw, arrayListStart, yPos, arrayListEnd);

        // Watermark module (using Watermark class)
        Watermark::RenderArrayList(draw, arrayListStart, yPos, arrayListEnd);
        
        // Show Render Info module (using RenderInfo class)
        RenderInfo::RenderArrayList(draw, arrayListStart, yPos, arrayListEnd);

        // Motion Blur module (using MotionBlur class)
        MotionBlur::RenderArrayList(draw, arrayListStart, yPos, arrayListEnd);

        // Keystrokes module (using Keystrokes class)
        Keystrokes::RenderArrayList(draw, arrayListStart, yPos, arrayListEnd);

        // CPS Counter module (using CPSCounter class)
        CPSCounter::RenderArrayList(draw, arrayListStart, yPos, arrayListEnd);

    // Debug: Draw ArrayList collision area
    if (g_showMenu) {
        g_arrayListHud.size.y = arrayListEnd.y - arrayListStart.y;
        draw->AddRect(
            arrayListStart,
            ImVec2(arrayListStart.x + g_arrayListHud.size.x, arrayListEnd.y),
            IM_COL32(255, 255, 255, 80)
        );
    }
        
    // Initial notification - delegado a GUI
    GUI::RenderNotification(sw, sh);

    // Menu drawing - delegado a GUI
    if (GUI::g_menuAnim > 0.001f) {
        GUI::RenderMenu(sw, sh);
    }
    
    // Watermark display (using Watermark class)
    Watermark::RenderDisplay();
    
    // Keystrokes display (using Keystrokes class)
    Keystrokes::RenderDisplay(sw, sh);
    
    // Render Info window (using RenderInfo class)
    RenderInfo::RenderWindow();

    // CPS counter display
    CPSCounter::RenderDisplay(sw, sh, g_lmbCps, g_rmbCps);

    ImGui::Render();
    
    pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    
    // 🔒 UNLOCK FPS - Frame rate limiting (using UnlockFPS class)
    UnlockFPS::UpdateFPS();
    
    // Forzar VSync OFF para UWP - SyncInterval = 0 desabilita VSync completamente
    return Hook::oPresent(pSwapChain, 0, Flags);
}

// Hook initialization thread function
DWORD WINAPI HookInitThread(LPVOID lpReserved) {
    Hook::Initialize();
    return 0;
}

// Hook initialization delegated to Hook::Initialize()

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hMod);
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HookInitThread, 0, 0, 0);
    }
    return TRUE;
}
