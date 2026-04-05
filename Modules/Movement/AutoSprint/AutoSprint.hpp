#pragma once

#include <cstdint>
#include <windows.h>

// Forward declarations
class ImDrawList;
struct ImVec2;

// AutoSprint Module
class AutoSprint {
public:
    static bool g_autoSprintEnabled;
    static uintptr_t g_autoSprintAddr;
    static void* g_autoSprintCave;
    static BYTE g_autoSprintBackup[11];
    static ULONGLONG g_autoSprintEnableTime;
    static ULONGLONG g_autoSprintDisableTime;

    // Initialize autosprint module
    static void Initialize(uintptr_t gameBase);

    // Enable/Disable autosprint
    static void Enable();
    static void Disable();

    // Render autosprint in array list
    static void RenderArrayList(ImDrawList* draw, ImVec2 arrayListStart, float& yPos, ImVec2& arrayListEnd);

    // Render autosprint UI in menu
    static void RenderMenu();

    // Check if autosprint is enabled
    static bool IsEnabled() { return g_autoSprintEnabled; }
};
