#include "AdvancedUI.h"
#include <cctype>
#include <algorithm>

const unsigned char* getCharBitmap(char c) {
    c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));

    static const unsigned char fontA[] = {0x7E, 0x11, 0x11, 0x11, 0x7E};
    static const unsigned char fontB[] = {0x7F, 0x49, 0x49, 0x49, 0x36};
    static const unsigned char fontC[] = {0x3E, 0x41, 0x41, 0x41, 0x22};
    static const unsigned char fontD[] = {0x7F, 0x41, 0x41, 0x22, 0x1C};
    static const unsigned char fontE[] = {0x7F, 0x49, 0x49, 0x49, 0x41};
    static const unsigned char fontF[] = {0x7F, 0x09, 0x09, 0x09, 0x06};
    static const unsigned char fontG[] = {0x3E, 0x41, 0x49, 0x49, 0x7A};
    static const unsigned char fontH[] = {0x7F, 0x08, 0x08, 0x08, 0x7F};
    static const unsigned char fontI[] = {0x00, 0x41, 0x7F, 0x41, 0x00};
    static const unsigned char fontJ[] = {0x20, 0x40, 0x41, 0x3F, 0x01};
    static const unsigned char fontK[] = {0x7F, 0x08, 0x14, 0x22, 0x41};
    static const unsigned char fontL[] = {0x7F, 0x40, 0x40, 0x40, 0x40};
    static const unsigned char fontM[] = {0x7F, 0x02, 0x04, 0x02, 0x7F};
    static const unsigned char fontN[] = {0x7F, 0x04, 0x08, 0x10, 0x7F};
    static const unsigned char fontO[] = {0x3E, 0x41, 0x41, 0x41, 0x3E};
    static const unsigned char fontP[] = {0x7F, 0x09, 0x09, 0x09, 0x06};
    static const unsigned char fontQ[] = {0x3E, 0x41, 0x51, 0x21, 0x5E};
    static const unsigned char fontR[] = {0x7F, 0x09, 0x19, 0x29, 0x46};
    static const unsigned char fontS[] = {0x26, 0x49, 0x49, 0x49, 0x32};
    static const unsigned char fontT[] = {0x01, 0x01, 0x7F, 0x01, 0x01};
    static const unsigned char fontU[] = {0x3F, 0x40, 0x40, 0x40, 0x3F};
    static const unsigned char fontV[] = {0x1F, 0x20, 0x40, 0x20, 0x1F};
    static const unsigned char fontW[] = {0x3F, 0x40, 0x38, 0x40, 0x3F};
    static const unsigned char fontX[] = {0x63, 0x14, 0x08, 0x14, 0x63};
    static const unsigned char fontY[] = {0x07, 0x08, 0x70, 0x08, 0x07};
    static const unsigned char fontZ[] = {0x61, 0x51, 0x49, 0x45, 0x43};

    static const unsigned char font0[] = {0x3E, 0x51, 0x49, 0x45, 0x3E};
    static const unsigned char font1[] = {0x00, 0x42, 0x7F, 0x40, 0x00};
    static const unsigned char font2[] = {0x42, 0x61, 0x51, 0x49, 0x46};
    static const unsigned char font3[] = {0x21, 0x41, 0x49, 0x4D, 0x33};
    static const unsigned char font4[] = {0x18, 0x14, 0x12, 0x7F, 0x10};
    static const unsigned char font5[] = {0x27, 0x45, 0x45, 0x45, 0x39};
    static const unsigned char font6[] = {0x3C, 0x4A, 0x49, 0x49, 0x30};
    static const unsigned char font7[] = {0x01, 0x71, 0x09, 0x05, 0x03};
    static const unsigned char font8[] = {0x36, 0x49, 0x49, 0x49, 0x36};
    static const unsigned char font9[] = {0x06, 0x49, 0x49, 0x29, 0x1E};

    static const unsigned char fontColon[]  = {0x00, 0x36, 0x36, 0x00, 0x00};
    static const unsigned char fontLParen[] = {0x00, 0x1C, 0x22, 0x41, 0x00};
    static const unsigned char fontRParen[] = {0x00, 0x41, 0x22, 0x1C, 0x00};
    static const unsigned char fontDash[]   = {0x08, 0x08, 0x08, 0x08, 0x08};
    static const unsigned char fontPlus[]   = {0x08, 0x08, 0x3E, 0x08, 0x08};
    static const unsigned char fontSpace[]  = {0x00, 0x00, 0x00, 0x00, 0x00};

    switch (c) {
        case 'A': return fontA; case 'B': return fontB; case 'C': return fontC;
        case 'D': return fontD; case 'E': return fontE; case 'F': return fontF;
        case 'G': return fontG; case 'H': return fontH; case 'I': return fontI;
        case 'J': return fontJ; case 'K': return fontK; case 'L': return fontL;
        case 'M': return fontM; case 'N': return fontN; case 'O': return fontO;
        case 'P': return fontP; case 'Q': return fontQ; case 'R': return fontR;
        case 'S': return fontS; case 'T': return fontT; case 'U': return fontU;
        case 'V': return fontV; case 'W': return fontW; case 'X': return fontX;
        case 'Y': return fontY; case 'Z': return fontZ;

        case '0': return font0; case '1': return font1; case '2': return font2;
        case '3': return font3; case '4': return font4; case '5': return font5;
        case '6': return font6; case '7': return font7; case '8': return font8;
        case '9': return font9;

        case ':': return fontColon; case '(': return fontLParen;
        case ')': return fontRParen; case '-': return fontDash;
        case '+': return fontPlus;
        default:  return fontSpace;
    }
}

void renderText(SDL_Renderer* renderer, const std::string& text, float x, float y, float scale) {
    SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
    float curX = x;

    for (char c : text) {
        const unsigned char* bitmap = getCharBitmap(c);
        for (int col = 0; col < 5; ++col) {
            unsigned char bits = bitmap[col];
            for (int row = 0; row < 7; ++row) {
                if (bits & (1 << row)) {
                    SDL_FRect r = { curX + col * scale, y + row * scale, scale, scale };
                    SDL_RenderFillRectF(renderer, &r);
                }
            }
        }
        curX += 6 * scale;
    }
}

AdvancedSidebar::AdvancedSidebar() {
    std::vector<std::pair<ComponentType, std::string>> menuData = {
            // --- گیت‌های منطقی ---
            {ComponentType::GATE_AND,       "AND GATE"},
            {ComponentType::GATE_OR,        "OR GATE"},
            {ComponentType::GATE_NOT,       "NOT GATE"},
            {ComponentType::GATE_NAND,      "NAND GATE"},
            {ComponentType::GATE_NOR,       "NOR GATE"},
            {ComponentType::GATE_XOR,       "XOR GATE"},
            {ComponentType::GATE_XNOR,      "XNOR GATE"},

            // --- ورودی‌ها و سیگنال‌ها ---
            {ComponentType::SWITCH,         "SWITCH"},
            {ComponentType::CONST_0,        "GND (0V LOGIC)"},
            {ComponentType::CONST_1,        "VCC (5V LOGIC)"},
            {ComponentType::CLOCK,          "CLOCK PULSE"},

            // --- قطعات پسیو و آنالوگ ---
            {ComponentType::GROUND,         "GROUND (EARTH)"},
            {ComponentType::RESISTOR,       "RESISTOR"},
            {ComponentType::CAPACITOR,      "CAPACITOR"},
            {ComponentType::INDUCTOR,       "INDUCTOR"},
            {ComponentType::DIODE,          "DIODE"},
            {ComponentType::DIODE_ZENER,    "ZENER DIODE"},
            {ComponentType::VOLTAGE_SOURCE, "VOLTAGE SOURCE"},
            {ComponentType::BATTERY,        "BATTERY"},

            // --- خروجی‌ها و ابزار سنجش ---
            {ComponentType::LED,            "LED LIGHT"},
            {ComponentType::VOLTMETER,      "VOLTMETER"},
            {ComponentType::AMPEREMETER,    "AMPEREMETER"}
    };

    float startY = 60.0f;
    float itemHeight = 36.0f;
    float spacing = 6.0f;

    for (size_t i = 0; i < menuData.size(); ++i) {
        float y = startY + i * (itemHeight + spacing);
        SDL_Color itemColor = {60, 130, 220, 255};

        ComponentType t = menuData[i].first;
        if (t == ComponentType::SWITCH || t == ComponentType::CONST_0 ||
            t == ComponentType::CONST_1 || t == ComponentType::CLOCK) {
            itemColor = {40, 180, 100, 255};
        }
        else if (t == ComponentType::RESISTOR || t == ComponentType::CAPACITOR ||
                 t == ComponentType::INDUCTOR || t == ComponentType::DIODE ||
                 t == ComponentType::DIODE_ZENER || t == ComponentType::GROUND ||
                 t == ComponentType::VOLTAGE_SOURCE || t == ComponentType::BATTERY) {
            itemColor = {200, 130, 40, 255};
        }
        else if (t == ComponentType::LED || t == ComponentType::VOLTMETER || t == ComponentType::AMPEREMETER) {
            itemColor = {220, 60, 60, 255};
        }

        items.push_back({
                                menuData[i].first,
                                menuData[i].second,
                                "GENERAL",
                                {10.0f, y, width - 20.0f, itemHeight},
                                itemColor
                        });
    }
}

void AdvancedSidebar::handleScroll(int wheelY) {
    scrollOffset += wheelY * 20.0f;
    if (scrollOffset > 0.0f) scrollOffset = 0.0f;
}

void AdvancedSidebar::navigateUp() {
    if (items.empty()) return;
    if (selectedIndex == -1) {
        selectedIndex = static_cast<int>(items.size()) - 1;
    } else {
        selectedIndex--;
        if (selectedIndex < 0) {
            selectedIndex = static_cast<int>(items.size()) - 1;
        }
    }
    scrollToSelected();
}

void AdvancedSidebar::navigateDown() {
    if (items.empty()) return;
    if (selectedIndex == -1) {
        selectedIndex = 0;
    } else {
        selectedIndex++;
        if (selectedIndex >= static_cast<int>(items.size())) {
            selectedIndex = 0;
        }
    }
    scrollToSelected();
}

void AdvancedSidebar::scrollToSelected() {
    if (selectedIndex < 0 || selectedIndex >= static_cast<int>(items.size())) return;

    float itemTop = items[selectedIndex].rect.y + scrollOffset;
    float itemBottom = itemTop + items[selectedIndex].rect.h;

    // محدوده قابل دیدن لیست در سایدبار (از y = 45 تا y = 610)
    float viewTop = 45.0f;
    float viewBottom = 610.0f;

    if (itemTop < viewTop) {
        scrollOffset += (viewTop - itemTop);
        if (scrollOffset > 0.0f) scrollOffset = 0.0f;
    } else if (itemBottom > viewBottom) {
        scrollOffset -= (itemBottom - viewBottom);
    }
}

ComponentType* AdvancedSidebar::getSelectedType() {
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(items.size())) {
        return &items[selectedIndex].type;
    }
    return nullptr;
}

void AdvancedSidebar::render(SDL_Renderer* renderer, SimState simState) {
    SDL_SetRenderDrawColor(renderer, 28, 32, 38, 255);
    SDL_FRect bg = {0, 0, width, 720};
    SDL_RenderFillRectF(renderer, &bg);

    SDL_SetRenderDrawColor(renderer, 50, 55, 65, 255);
    SDL_RenderDrawLineF(renderer, width, 0, width, 720);

    renderText(renderer, "COMPONENTS", 15, 15, 1.8f);
    SDL_SetRenderDrawColor(renderer, 70, 130, 240, 255);
    SDL_FRect headerLine = {15, 40, width - 30.0f, 2};
    SDL_RenderFillRectF(renderer, &headerLine);

    for (size_t i = 0; i < items.size(); ++i) {
        auto item = items[i];
        float currentY = item.rect.y + scrollOffset;

        if (currentY + item.rect.h < 45 || currentY > 610) continue;

        bool isSelected = (static_cast<int>(i) == selectedIndex);

        SDL_SetRenderDrawColor(renderer, isSelected ? 50 : 38, isSelected ? 60 : 42, isSelected ? 75 : 50, 255);
        SDL_FRect drawRect = {item.rect.x, currentY, item.rect.w, item.rect.h};
        SDL_RenderFillRectF(renderer, &drawRect);

        SDL_SetRenderDrawColor(renderer, isSelected ? 70 : 55, isSelected ? 140 : 60, isSelected ? 240 : 70, 255);
        SDL_RenderDrawRectF(renderer, &drawRect);

        SDL_SetRenderDrawColor(renderer, item.color.r, item.color.g, item.color.b, 255);
        SDL_FRect iconRect = {drawRect.x + 6, drawRect.y + 8, 18, 18};
        SDL_RenderFillRectF(renderer, &iconRect);

        renderText(renderer, item.name, drawRect.x + 30, drawRect.y + 10, 1.15f);
    }

    SDL_SetRenderDrawColor(renderer, 20, 22, 26, 255);
    SDL_FRect simPanel = {10, 630, width - 20.0f, 80};
    SDL_RenderFillRectF(renderer, &simPanel);

    renderText(renderer, "STATUS:", 20, 642, 1.3f);
    if (simState == SimState::RUNNING) {
        renderText(renderer, "RUNNING", 20, 668, 1.4f);
    } else if (simState == SimState::PAUSED) {
        renderText(renderer, "PAUSED", 20, 668, 1.4f);
    } else {
        renderText(renderer, "STOPPED", 20, 668, 1.4f);
    }
}

ComponentType* AdvancedSidebar::handleClick(float x, float y) {
    if (x > width || y < 45 || y > 620) return nullptr;
    for (size_t i = 0; i < items.size(); ++i) {
        float itemY = items[i].rect.y + scrollOffset;
        if (x >= items[i].rect.x && x <= items[i].rect.x + items[i].rect.w &&
            y >= itemY && y <= itemY + items[i].rect.h) {
            selectedIndex = static_cast<int>(i);
            return &items[i].type;
        }
    }
    return nullptr;
}