#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

struct ContextMenuItem {
    std::string text;
    int id;
};

class ContextMenu {
private:
    bool active = false;
    float posX = 0.0f, posY = 0.0f;
    std::vector<ContextMenuItem> items;
    float width = 160.0f;
    float itemHeight = 30.0f;

public:
    ContextMenu() {
        // گزینه‌های کامل مشابه پروتئوس
        items.push_back({"Properties", 1});
        items.push_back({"Rotate (R)", 2});
        items.push_back({"Mirror Horiz (H)", 3});
        items.push_back({"Mirror Vert (V)", 4});
        items.push_back({"Delete (Del)", 5});
    }

    void show(float x, float y) {
        active = true;
        posX = x;
        posY = y;
    }

    void hide() {
        active = false;
    }

    bool isActive() const {
        return active;
    }

    void render(SDL_Renderer* renderer, TTF_Font* font) {
        if (!active) return;

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        float mx = static_cast<float>(mouseX);
        float my = static_cast<float>(mouseY);

        float totalHeight = items.size() * itemHeight;
        SDL_FRect bgRect = {posX, posY, width, totalHeight};

        // پس‌زمینه منو (خاکستری تیره)
        SDL_SetRenderDrawColor(renderer, 33, 37, 43, 245);
        SDL_RenderFillRectF(renderer, &bgRect);

        // کادر آبی دور منو
        SDL_SetRenderDrawColor(renderer, 70, 130, 240, 255);
        SDL_RenderDrawRectF(renderer, &bgRect);

        // رسم گزینه‌ها و افکت Hover
        for (size_t i = 0; i < items.size(); ++i) {
            float itemY = posY + (i * itemHeight);
            SDL_FRect itemRect = {posX, itemY, width, itemHeight};

            // افکت Hover در صورت قرارگیری ماوس روی گزینه
            if (mx >= posX && mx <= posX + width && my >= itemY && my <= itemY + itemHeight) {
                SDL_SetRenderDrawColor(renderer, 55, 90, 160, 255);
                SDL_RenderFillRectF(renderer, &itemRect);
            }

            // خط جداکننده بین اقلام منو
            if (i > 0) {
                SDL_SetRenderDrawColor(renderer, 50, 55, 65, 255);
                SDL_RenderDrawLineF(renderer, posX, itemY, posX + width, itemY);
            }

            if (font) {
                SDL_Surface* surf = TTF_RenderText_Blended(font, items[i].text.c_str(), {230, 230, 230, 255});
                if (surf) {
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                    if (tex) {
                        SDL_FRect dest = {posX + 10.0f, itemY + 6.0f, static_cast<float>(surf->w), static_cast<float>(surf->h)};
                        SDL_RenderCopyF(renderer, tex, nullptr, &dest);
                        SDL_DestroyTexture(tex);
                    }
                    SDL_FreeSurface(surf);
                }
            }
        }
    }

    int handleClick(float x, float y) {
        if (!active) return -1;

        float totalHeight = items.size() * itemHeight;
        if (x >= posX && x <= posX + width && y >= posY && y <= posY + totalHeight) {
            int index = static_cast<int>((y - posY) / itemHeight);
            if (index >= 0 && index < static_cast<int>(items.size())) {
                return items[index].id;
            }
        }
        return -1;
    }
};