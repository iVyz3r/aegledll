#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <nlohmann/json.hpp>

class Terminal {
public:
    static void Initialize();
    static void RenderConsole();

    // Command handling
    static void ExecuteCommand(const std::string& command);
    static void AddOutput(const std::string& text);

    // Config management
    static bool SaveConfig(const std::string& name);
    static bool LoadConfig(const std::string& name);
    static bool DeleteConfig(const std::string& name);
    static std::vector<std::string> ListConfigs();
    static bool OpenConfigDirectory();

    // Detach functionality
    static void Detach();

private:
    static std::vector<std::string> outputLines;
    static char inputBuffer[256];
    static bool scrollToBottom;
    static std::string configDir;

    // Helper functions
    static void ShowHelp();
    static nlohmann::json CollectCurrentConfig();
    static void ApplyConfig(const nlohmann::json& config);
};