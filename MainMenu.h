#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

struct RecentProject {
    std::string name;
    std::string filePath;
    std::string lastModified;
};

enum class MenuResult {
    NONE,
    NEW_PROJECT,
    OPEN_PROJECT,
    OPEN_RECENT,
    EXIT
};

struct ProjectSettings {
    int width = 1920;
    int height = 1080;
    std::string projectName = "Untitled_Circuit";
};

class MainMenu {
private:
    bool showCanvasConfigModal = false;
    ProjectSettings tempSettings;
    int selectedPreset = 0; // 0: Custom, 1: A4, 2: A3, 3: HD, 4: 4K
    std::vector<RecentProject> recentProjects;
    MenuResult currentResult = MenuResult::NONE;
    std::string selectedRecentPath = "";

    void renderButton(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_FRect rect, bool hovered);

public:
    MainMenu();

    // بارگذاری لیست پروژه‌های اخیر (مثلاً از فایل یا ریجستری محلی)
    void loadRecentProjects();

    // مدیریت رویدادها (کلیک موس، کیبورد)
    MenuResult handleEvent(const SDL_Event& event, int windowWidth, int windowHeight);

    // رندر کردن منو و پنجره تنظیمات بوم
    void render(SDL_Renderer* renderer, TTF_Font* font, int windowWidth, int windowHeight);

    ProjectSettings getProjectSettings() const { return tempSettings; }
    std::string getSelectedRecentPath() const { return selectedRecentPath; }
};

#endif