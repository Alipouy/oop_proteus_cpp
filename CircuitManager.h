#ifndef CIRCUIT_MANAGER_H
#define CIRCUIT_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include <memory>
#include "Types.h"
#include "Component.h"
#include "Wire.h"

class CircuitManager {
private:
    std::vector<std::unique_ptr<Component>> components;
    std::vector<Wire> wires;

    std::vector<std::string> historyStates;
    int historyIndex = -1;

    void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, float x, float y, SDL_Color color);
    void loadFromMemoryStream(const std::string& stateStr);

public:
    CircuitManager() = default;

    void addComponent(std::unique_ptr<Component> comp);
    void addWire(Component* src, int srcPin, Component* dest, int destPin);

    void deleteComponent(Component* comp);
    void deleteWire(Wire* wire);
    void deleteSelectedItems();
    void clearSelections();

    const std::vector<std::unique_ptr<Component>>& getComponents() const { return components; }
    const std::vector<Wire>& getWires() const { return wires; }

    std::string generateUniqueId(ComponentType type);
    void updateLogic();
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom, SimState simState);

    Component* findComponentAt(const Point2D& worldPos);
    Wire* findWireAt(const Point2D& worldPos);

    // به‌روزرسانی امضای توابع برای دریافت پارامتر zoom جهت دقت بالا در بزرگنمایی‌های مختلف
    bool findPinAt(const Point2D& worldPos, Component*& outComp, int& outPinIdx, float zoom = 1.0f);
    std::string inspectProbeAt(const Point2D& worldPos, float zoom = 1.0f);

    void saveStateHistory();
    void undo();
    void redo();
    void saveToFile(const std::string& filename);
    void loadFromFile(const std::string& filename);

    // به‌روزرسانی متد اکسپورت برای پشتیبانی از A4 و A3
    void exportScreenshot(SDL_Renderer* renderer, const std::string& filename, bool isA3 = false);

    bool hasShortCircuit();
    bool hasFloatingPins();
};

#endif // CIRCUIT_MANAGER_H