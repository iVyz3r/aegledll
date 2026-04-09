#include "Terminal.hpp"
#include "Modules/ModuleHeader.hpp"
#include "ImGui/imgui.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <windows.h>
#include <shellapi.h>
#include <filesystem>
#include <cstdlib>
#include <ctime>

// Static member initialization
std::vector<std::string> Terminal::outputLines;
char Terminal::inputBuffer[256] = {0};
bool Terminal::scrollToBottom = false;
std::string Terminal::configDir;

static bool EnsureDirectoryExists(const std::string& directoryPath) {
    try {
        std::filesystem::path path(directoryPath);
        if (std::filesystem::exists(path)) {
            return std::filesystem::is_directory(path);
        }
        return std::filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
}

void Terminal::Initialize() {
    try {
        // Set config directory to %LOCALAPPDATA%\Packages\Microsoft.MinecraftUWP_8wekyb3d8bbwe\LocalState\Aegle
        std::filesystem::path baseDir;
        char* localAppData = std::getenv("LOCALAPPDATA");
        if (localAppData && localAppData[0] != '\0') {
            baseDir = std::filesystem::path(localAppData) /
                "Packages" /
                "Microsoft.MinecraftUWP_8wekyb3d8bbwe" /
                "LocalState" /
                "Aegle";
        } else {
            baseDir = std::filesystem::current_path() / "Aegle";
        }

        if (!EnsureDirectoryExists(baseDir.string())) {
            baseDir = std::filesystem::current_path() / "Aegle";
            EnsureDirectoryExists(baseDir.string());
        }

        configDir = baseDir.string();
        if (!configDir.empty() && configDir.back() != '\\' && configDir.back() != '/') {
            configDir += "\\";
        }
    } catch (...) {
        configDir = (std::filesystem::current_path() / "configs").string();
        EnsureDirectoryExists(configDir);
        if (!configDir.empty() && configDir.back() != '\\' && configDir.back() != '/') {
            configDir += "\\";
        }
    }
}

void Terminal::RenderConsole() {
    ImGui::BeginChild("TerminalOutput", ImVec2(0, 250), true, ImGuiWindowFlags_HorizontalScrollbar);
    
    for (const auto& line : outputLines) {
        ImGui::TextUnformatted(line.c_str());
    }
    
    if (scrollToBottom) {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;
    }
    
    ImGui::EndChild();
    
    ImGui::Separator();
    
    // Input field
    if (ImGui::InputText("##TerminalInput", inputBuffer, sizeof(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (strlen(inputBuffer) > 0) {
            ExecuteCommand(std::string(inputBuffer));
            inputBuffer[0] = '\0';
            scrollToBottom = true;
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Execute")) {
        if (strlen(inputBuffer) > 0) {
            ExecuteCommand(std::string(inputBuffer));
            inputBuffer[0] = '\0';
            scrollToBottom = true;
        }
    }
}

void Terminal::ExecuteCommand(const std::string& command) {
    AddOutput("> " + command);
    
    if (command == ".help") {
        ShowHelp();
    } else if (command.substr(0, 13) == ".config save ") {
        std::string name = command.substr(13);
        if (SaveConfig(name)) {
            AddOutput("Config saved: " + name);
        } else {
            AddOutput("Failed to save config: " + name);
        }
    } else if (command.substr(0, 13) == ".config load ") {
        std::string name = command.substr(13);
        if (LoadConfig(name)) {
            AddOutput("Config loaded: " + name);
        } else {
            AddOutput("Failed to load config: " + name);
        }
    } else if (command.substr(0, 15) == ".config delete ") {
        std::string name = command.substr(15);
        if (DeleteConfig(name)) {
            AddOutput("Config deleted: " + name);
        } else {
            AddOutput("Failed to delete config: " + name);
        }
    } else if (command == ".config list") {
        auto configs = ListConfigs();
        if (configs.empty()) {
            AddOutput("No configs found.");
        } else {
            AddOutput("Available configs:");
            for (const auto& config : configs) {
                AddOutput("  " + config);
            }
        }
    } else if (command == ".config opendirectory") {
        if (OpenConfigDirectory()) {
            AddOutput("Opened config directory: " + configDir);
        } else {
            AddOutput("Failed to open config directory: " + configDir);
        }
    } else if (command == ".deattach") {
        AddOutput("Detaching DLL...");
        Detach();
    } else {
        AddOutput("Unknown command. Type .help for available commands.");
    }
}

void Terminal::AddOutput(const std::string& text) {
    outputLines.push_back(text);
    if (outputLines.size() > 1000) { // Limit output lines
        outputLines.erase(outputLines.begin());
    }
}

bool Terminal::SaveConfig(const std::string& name) {
    try {
        if (name.empty()) {
            AddOutput("Config name cannot be empty");
            return false;
        }

        nlohmann::json config = CollectCurrentConfig();
        std::filesystem::path dirPath = std::filesystem::path(configDir);
        std::filesystem::path filepath = dirPath / (name + ".json");
        AddOutput("Saving config to: " + filepath.string());

        if (!EnsureDirectoryExists(dirPath.string())) {
            AddOutput("Failed to create config directory: " + dirPath.string());
            return false;
        }

        std::ofstream file(filepath, std::ios::out | std::ios::trunc);
        if (file.is_open()) {
            file << config.dump(4);
            AddOutput("Config saved successfully");
            return true;
        }

        AddOutput("Failed to open file for writing: " + filepath.string());
    } catch (const std::exception& e) {
        AddOutput("Exception in SaveConfig: " + std::string(e.what()));
    } catch (...) {
        AddOutput("Unknown exception in SaveConfig");
    }
    return false;
}

bool Terminal::LoadConfig(const std::string& name) {
    try {
        std::filesystem::path filepath = std::filesystem::path(configDir) / (name + ".json");
        AddOutput("Loading config from: " + filepath.string());
        std::ifstream file(filepath);
        if (file.is_open()) {
            nlohmann::json config;
            file >> config;
            ApplyConfig(config);
            AddOutput("Config loaded successfully");
            return true;
        } else {
            AddOutput("Failed to open file for reading: " + filepath.string());
        }
    } catch (const std::exception& e) {
        AddOutput("Exception in LoadConfig: " + std::string(e.what()));
    } catch (...) {
        AddOutput("Unknown exception in LoadConfig");
    }
    return false;
}

bool Terminal::DeleteConfig(const std::string& name) {
    try {
        std::string filepath = configDir + name + ".json";
        AddOutput("Deleting config: " + filepath);
        std::filesystem::path path = filepath;
        if (std::filesystem::remove(path)) {
            AddOutput("Config deleted successfully");
            return true;
        } else {
            AddOutput("Config file not found");
        }
    } catch (const std::exception& e) {
        AddOutput("Exception in DeleteConfig: " + std::string(e.what()));
    } catch (...) {
        AddOutput("Unknown exception in DeleteConfig");
    }
    return false;
}

std::vector<std::string> Terminal::ListConfigs() {
    std::vector<std::string> configs;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(configDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                configs.push_back(entry.path().stem().string());
            }
        }
    } catch (...) {}
    return configs;
}

bool Terminal::OpenConfigDirectory() {
    try {
        if (configDir.empty()) {
            return false;
        }

        std::filesystem::path dirPath = std::filesystem::path(configDir);
        if (!std::filesystem::exists(dirPath)) {
            if (!EnsureDirectoryExists(dirPath.string())) {
                return false;
            }
        }

        HINSTANCE result = ShellExecuteA(NULL, "open", configDir.c_str(), NULL, NULL, SW_SHOWDEFAULT);
        return reinterpret_cast<intptr_t>(result) > 32;
    } catch (...) {
        return false;
    }
}

void Terminal::Detach() {
    // Clean up and unload DLL
    // This is a simplified version - in a real scenario, you'd need to properly clean up hooks, etc.
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule) {
        FreeLibrary(hModule);
    }
}

void Terminal::ShowHelp() {
    AddOutput("Available commands:");
    AddOutput("  .help                    - Show this help");
    AddOutput("  .config save <name>      - Save current config");
    AddOutput("  .config load <name>      - Load config");
    AddOutput("  .config delete <name>    - Delete config");
    AddOutput("  .config list             - List available configs");
    AddOutput("  .config opendirectory    - Open the config directory");
    AddOutput("  .deattach                - Detach DLL safely");
}

nlohmann::json Terminal::CollectCurrentConfig() {
    nlohmann::json config;

    // Combat modules
    config["Combat"]["Hitbox"]["enabled"] = Hitbox::g_hitboxEnabled;
    config["Combat"]["Hitbox"]["value"] = Hitbox::g_hitboxValue;

    config["Combat"]["Reach"]["enabled"] = Reach::IsEnabled();
    config["Combat"]["Reach"]["value"] = Reach::g_reachValue;

    // Movement modules
    config["Movement"]["AutoSprint"]["enabled"] = AutoSprint::g_autoSprintEnabled;
    config["Movement"]["Timer"]["enabled"] = Timer::g_timerEnabled;
    config["Movement"]["Timer"]["value"] = Timer::g_timerValue;

    // Visuals modules
    config["Visuals"]["FullBright"]["enabled"] = FullBright::g_fullBrightEnabled;
    config["Visuals"]["FullBright"]["value"] = FullBright::g_fullBrightValue;

    config["Visuals"]["RenderInfo"]["enabled"] = RenderInfo::g_showRenderInfo;
    if (RenderInfo::g_renderInfoHud) {
        config["Visuals"]["RenderInfo"]["position"]["x"] = RenderInfo::g_renderInfoHud->pos.x;
        config["Visuals"]["RenderInfo"]["position"]["y"] = RenderInfo::g_renderInfoHud->pos.y;
    }

    config["Visuals"]["Watermark"]["enabled"] = Watermark::g_showWatermark;
    if (Watermark::g_watermarkHud) {
        config["Visuals"]["Watermark"]["position"]["x"] = Watermark::g_watermarkHud->pos.x;
        config["Visuals"]["Watermark"]["position"]["y"] = Watermark::g_watermarkHud->pos.y;
    }

    config["Visuals"]["MotionBlur"]["enabled"] = MotionBlur::g_motionBlurEnabled;
    config["Visuals"]["Keystrokes"]["enabled"] = Keystrokes::g_showKeystrokes;
    if (Keystrokes::g_keystrokesHud) {
        config["Visuals"]["Keystrokes"]["position"]["x"] = Keystrokes::g_keystrokesHud->pos.x;
        config["Visuals"]["Keystrokes"]["position"]["y"] = Keystrokes::g_keystrokesHud->pos.y;
    }

    config["Visuals"]["CPSCounter"]["enabled"] = CPSCounter::g_showCpsCounter;
    if (CPSCounter::g_cpsHud) {
        config["Visuals"]["CPSCounter"]["position"]["x"] = CPSCounter::g_cpsHud->pos.x;
        config["Visuals"]["CPSCounter"]["position"]["y"] = CPSCounter::g_cpsHud->pos.y;
    }

    // Misc modules
    config["Misc"]["UnlockFPS"]["enabled"] = UnlockFPS::g_unlockFpsEnabled;
    config["Misc"]["UnlockFPS"]["fpsLimit"] = UnlockFPS::g_fpsLimit;

    config["version"] = "1.0";
    config["timestamp"] = std::time(nullptr);

    return config;
}

void Terminal::ApplyConfig(const nlohmann::json& config) {
    // Combat modules
    if (config.contains("Combat")) {
        if (config["Combat"].contains("Hitbox")) {
            if (config["Combat"]["Hitbox"].contains("enabled")) {
                Hitbox::g_hitboxEnabled = config["Combat"]["Hitbox"]["enabled"];
            }
            if (config["Combat"]["Hitbox"].contains("value")) {
                Hitbox::g_hitboxValue = config["Combat"]["Hitbox"]["value"];
            }
        }
        if (config["Combat"].contains("Reach")) {
            if (config["Combat"]["Reach"].contains("enabled")) {
                // Note: Reach enabled state is managed by pointer, not directly settable here
            }
            if (config["Combat"]["Reach"].contains("value")) {
                Reach::g_reachValue = config["Combat"]["Reach"]["value"];
                if (Reach::IsEnabled()) {
                    Reach::UpdateValue(Reach::g_reachValue);
                }
            }
        }
    }

    // Movement modules
    if (config.contains("Movement")) {
        if (config["Movement"].contains("AutoSprint")) {
            if (config["Movement"]["AutoSprint"].contains("enabled")) {
                AutoSprint::g_autoSprintEnabled = config["Movement"]["AutoSprint"]["enabled"];
            }
        }
        if (config["Movement"].contains("Timer")) {
            if (config["Movement"]["Timer"].contains("enabled")) {
                Timer::g_timerEnabled = config["Movement"]["Timer"]["enabled"];
            }
            if (config["Movement"]["Timer"].contains("value")) {
                Timer::g_timerValue = config["Movement"]["Timer"]["value"];
            }
        }
    }

    // Visuals modules
    if (config.contains("Visuals")) {
        if (config["Visuals"].contains("FullBright")) {
            if (config["Visuals"]["FullBright"].contains("enabled")) {
                FullBright::g_fullBrightEnabled = config["Visuals"]["FullBright"]["enabled"];
            }
            if (config["Visuals"]["FullBright"].contains("value")) {
                FullBright::g_fullBrightValue = config["Visuals"]["FullBright"]["value"];
            }
        }
        if (config["Visuals"].contains("RenderInfo")) {
            if (config["Visuals"]["RenderInfo"].contains("enabled")) {
                RenderInfo::g_showRenderInfo = config["Visuals"]["RenderInfo"]["enabled"];
            }
            if (config["Visuals"]["RenderInfo"].contains("position") && RenderInfo::g_renderInfoHud) {
                RenderInfo::g_renderInfoHud->pos.x = config["Visuals"]["RenderInfo"]["position"]["x"];
                RenderInfo::g_renderInfoHud->pos.y = config["Visuals"]["RenderInfo"]["position"]["y"];
            }
        }
        if (config["Visuals"].contains("Watermark")) {
            if (config["Visuals"]["Watermark"].contains("enabled")) {
                Watermark::g_showWatermark = config["Visuals"]["Watermark"]["enabled"];
            }
            if (config["Visuals"]["Watermark"].contains("position") && Watermark::g_watermarkHud) {
                Watermark::g_watermarkHud->pos.x = config["Visuals"]["Watermark"]["position"]["x"];
                Watermark::g_watermarkHud->pos.y = config["Visuals"]["Watermark"]["position"]["y"];
            }
        }
        if (config["Visuals"].contains("MotionBlur")) {
            if (config["Visuals"]["MotionBlur"].contains("enabled")) {
                MotionBlur::g_motionBlurEnabled = config["Visuals"]["MotionBlur"]["enabled"];
            }
        }
        if (config["Visuals"].contains("Keystrokes")) {
            if (config["Visuals"]["Keystrokes"].contains("enabled")) {
                Keystrokes::g_showKeystrokes = config["Visuals"]["Keystrokes"]["enabled"];
            }
            if (config["Visuals"]["Keystrokes"].contains("position") && Keystrokes::g_keystrokesHud) {
                Keystrokes::g_keystrokesHud->pos.x = config["Visuals"]["Keystrokes"]["position"]["x"];
                Keystrokes::g_keystrokesHud->pos.y = config["Visuals"]["Keystrokes"]["position"]["y"];
            }
        }
        if (config["Visuals"].contains("CPSCounter")) {
            if (config["Visuals"]["CPSCounter"].contains("enabled")) {
                CPSCounter::g_showCpsCounter = config["Visuals"]["CPSCounter"]["enabled"];
            }
            if (config["Visuals"]["CPSCounter"].contains("position") && CPSCounter::g_cpsHud) {
                CPSCounter::g_cpsHud->pos.x = config["Visuals"]["CPSCounter"]["position"]["x"];
                CPSCounter::g_cpsHud->pos.y = config["Visuals"]["CPSCounter"]["position"]["y"];
            }
        }
    }

    // Misc modules
    if (config.contains("Misc")) {
        if (config["Misc"].contains("UnlockFPS")) {
            if (config["Misc"]["UnlockFPS"].contains("enabled")) {
                UnlockFPS::g_unlockFpsEnabled = config["Misc"]["UnlockFPS"]["enabled"];
            }
            if (config["Misc"]["UnlockFPS"].contains("fpsLimit")) {
                UnlockFPS::g_fpsLimit = config["Misc"]["UnlockFPS"]["fpsLimit"];
            }
        }
    }

    AddOutput("Configuration applied successfully.");
}