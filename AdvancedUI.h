#ifndef ADVANCED_UI_H
#define ADVANCED_UI_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include "Types.h"

const unsigned char* getCharBitmap(char c);
void renderText(SDL_Renderer* renderer, const std::string& text, float x, float y, float scale = 1.5f);

struct AdvancedMenuItem {
    ComponentType type;
    std::string name;
    std::string category;
    SDL_FRect rect;
    SDL_Color color;
};

class AdvancedSidebar {
private:
    std::vector<AdvancedMenuItem> items;
    float width = 230.0f;
    int selectedIndex = -1;
    float scrollOffset = 0.0f;

    void scrollToSelected();

public:
    AdvancedSidebar();

    float getWidth() const { return width; }
    int getSelectedIndex() const { return selectedIndex; }
    void clearSelection() { selectedIndex = -1; }

    void navigateUp();
    void navigateDown();
    ComponentType* getSelectedType();

    void render(SDL_Renderer* renderer, SimState simState);
    ComponentType* handleClick(float x, float y);
    void handleScroll(int wheelY);
};

#endif // ADVANCED_UI_H