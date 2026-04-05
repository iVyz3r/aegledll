#include "Input.hpp"

// Static member definitions
bool Input::g_keys[256] = {};
bool Input::g_keysPressed[256] = {};
bool Input::g_keysReleased[256] = {};

// Virtual key to ImGui key mapping
ImGuiKey Input::VKToImGuiKey(int vk) {
    switch (vk) {
        case VK_TAB:            return ImGuiKey_Tab;
        case VK_LEFT:           return ImGuiKey_LeftArrow;
        case VK_RIGHT:          return ImGuiKey_RightArrow;
        case VK_UP:             return ImGuiKey_UpArrow;
        case VK_DOWN:           return ImGuiKey_DownArrow;
        case VK_PRIOR:          return ImGuiKey_PageUp;
        case VK_NEXT:           return ImGuiKey_PageDown;
        case VK_HOME:           return ImGuiKey_Home;
        case VK_END:            return ImGuiKey_End;
        case VK_INSERT:         return ImGuiKey_Insert;
        case VK_DELETE:         return ImGuiKey_Delete;
        case VK_BACK:           return ImGuiKey_Backspace;
        case VK_SPACE:          return ImGuiKey_Space;
        case VK_RETURN:         return ImGuiKey_Enter;
        case VK_ESCAPE:         return ImGuiKey_Escape;
        case VK_SHIFT:          return ImGuiKey_LeftShift;
        case VK_LSHIFT:         return ImGuiKey_LeftShift;
        case VK_RSHIFT:         return ImGuiKey_RightShift;
        case VK_CONTROL:        return ImGuiKey_LeftCtrl;
        case VK_LCONTROL:       return ImGuiKey_LeftCtrl;
        case VK_RCONTROL:       return ImGuiKey_RightCtrl;
        case VK_MENU:           return ImGuiKey_LeftAlt;
        case VK_LMENU:          return ImGuiKey_LeftAlt;
        case VK_RMENU:          return ImGuiKey_RightAlt;
        case VK_LWIN:           return ImGuiKey_LeftSuper;
        case VK_RWIN:           return ImGuiKey_RightSuper;
        case VK_CAPITAL:        return ImGuiKey_CapsLock;
        case VK_SCROLL:         return ImGuiKey_ScrollLock;
        case VK_NUMLOCK:        return ImGuiKey_NumLock;
        case VK_F1:             return ImGuiKey_F1;
        case VK_F2:             return ImGuiKey_F2;
        case VK_F3:             return ImGuiKey_F3;
        case VK_F4:             return ImGuiKey_F4;
        case VK_F5:             return ImGuiKey_F5;
        case VK_F6:             return ImGuiKey_F6;
        case VK_F7:             return ImGuiKey_F7;
        case VK_F8:             return ImGuiKey_F8;
        case VK_F9:             return ImGuiKey_F9;
        case VK_F10:            return ImGuiKey_F10;
        case VK_F11:            return ImGuiKey_F11;
        case VK_F12:            return ImGuiKey_F12;
        default:                return ImGuiKey_None;
    }
}

void Input::UpdateKeyboard(bool menuOpen) {
    ImGuiIO& io = ImGui::GetIO();

    // Update keyboard state
    for (int i = 0; i < 256; i++) {
        // Ignore F13-F24
        if (i >= VK_F13 && i <= VK_F24)
            continue;

        bool down = (GetAsyncKeyState(i) & 0x8000) != 0;

        g_keysPressed[i]  = down && !g_keys[i];
        g_keysReleased[i] = !down && g_keys[i];
        g_keys[i]         = down;

        // Send special keys to ImGui
        ImGuiKey imgui_key = VKToImGuiKey(i);
        if (imgui_key != ImGuiKey_None) {
            if (g_keysPressed[i])
                io.AddKeyEvent(imgui_key, true);
            if (g_keysReleased[i])
                io.AddKeyEvent(imgui_key, false);
        }
    }

    // Update modifiers
    io.KeyCtrl  = g_keys[VK_CONTROL];
    io.KeyShift = g_keys[VK_SHIFT];
    io.KeyAlt   = g_keys[VK_MENU];
    io.KeySuper = g_keys[VK_LWIN] || g_keys[VK_RWIN];

    // Add text characters
    if (menuOpen) {
        BYTE keyboardState[256];
        GetKeyboardState(keyboardState);

        WCHAR buffer[4];

        for (int vk = 0; vk < 256; vk++) {
            if (vk >= VK_F13 && vk <= VK_F24)
                continue;

            if (g_keysPressed[vk]) {
                UINT scanCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
                int result = ToUnicode(vk, scanCode, keyboardState, buffer, 4, 0);

                if (result > 0) {
                    for (int i = 0; i < result; i++)
                        io.AddInputCharacter((unsigned short)buffer[i]);
                }
            }
        }
    }
}

void Input::UpdateMouse(HWND window, float screenWidth, float screenHeight, bool drawCursor) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(screenWidth, screenHeight);

    // Manual input handling - UWP compatible
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(window, &p);
    io.MousePos = ImVec2((float)p.x, (float)p.y);

    // Mouse buttons - direct clean read
    io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    io.MouseDown[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    io.MouseDown[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;

    // ImGui cursor ONLY drawn when menu open
    io.MouseDrawCursor = drawCursor;
}

void Input::Update(HWND window, float screenWidth, float screenHeight, bool menuOpen, bool drawCursor) {
    UpdateKeyboard(menuOpen);
    UpdateMouse(window, screenWidth, screenHeight, drawCursor);
}
