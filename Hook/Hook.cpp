#include "Hook.hpp"
#include "../minhook/MinHook.h"

// Static member definitions
HRESULT(STDMETHODCALLTYPE* Hook::oPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) = NULL;
HRESULT(STDMETHODCALLTYPE* Hook::oResizeBuffers)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) = NULL;
BOOL(WINAPI* Hook::oSetCursorPos)(int x, int y) = NULL;
BOOL(WINAPI* Hook::oClipCursor)(const RECT* lpRect) = NULL;

// External references from dllmain.cpp
extern bool g_showMenu;
extern HRESULT STDMETHODCALLTYPE hkPresent_Impl(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
extern void CleanupRenderTarget();

// Helper to get VTable address
void* GetVTableAddress(int index) {
    DXGI_SWAP_CHAIN_DESC sd = { 0 };
    sd.BufferCount = 1; 
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; 
    sd.OutputWindow = GetForegroundWindow();
    sd.SampleDesc.Count = 1; 
    sd.Windowed = TRUE; 
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    
    ID3D11Device* d; 
    ID3D11DeviceContext* c; 
    IDXGISwapChain* s;
    D3D_FEATURE_LEVEL fl = D3D_FEATURE_LEVEL_11_0;
    
    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &fl, 1, D3D11_SDK_VERSION, &sd, &s, &d, NULL, &c))) 
        return 0;
    
    void* a = (*(void***)s)[index];
    s->Release(); 
    d->Release(); 
    c->Release(); 
    return a;
}

void Hook::Initialize() {
    Sleep(2000);
    MH_Initialize();
    
    // Get VTable addresses
    void* pPres = GetVTableAddress(8);
    void* pRes = GetVTableAddress(13);
    void* pSetCP = (void*)GetProcAddress(GetModuleHandleA("user32.dll"), "SetCursorPos");
    void* pClipC = (void*)GetProcAddress(GetModuleHandleA("user32.dll"), "ClipCursor");
    
    // Create hooks
    MH_CreateHook(pPres, (LPVOID)Hook::hkPresent, (LPVOID*)&Hook::oPresent);
    MH_CreateHook(pRes, (LPVOID)Hook::hkResizeBuffers, (LPVOID*)&Hook::oResizeBuffers);
    MH_CreateHook(pSetCP, (LPVOID)Hook::hkSetCursorPos, (LPVOID*)&Hook::oSetCursorPos);
    MH_CreateHook(pClipC, (LPVOID)Hook::hkClipCursor, (LPVOID*)&Hook::oClipCursor);
    
    // Enable all hooks
    MH_EnableHook(MH_ALL_HOOKS);
}

HRESULT STDMETHODCALLTYPE Hook::hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    return hkPresent_Impl(pSwapChain, SyncInterval, Flags);
}

HRESULT STDMETHODCALLTYPE Hook::hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
    CleanupRenderTarget();
    // Reset window dimensions
    extern float g_lastW, g_lastH;
    g_lastW = 0;
    g_lastH = 0;
    return Hook::oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}

BOOL WINAPI Hook::hkSetCursorPos(int x, int y) {
    if (g_showMenu) return TRUE;
    return Hook::oSetCursorPos(x, y);
}

BOOL WINAPI Hook::hkClipCursor(const RECT* lpRect) {
    if (g_showMenu) return Hook::oClipCursor(NULL);
    return Hook::oClipCursor(lpRect);
}
