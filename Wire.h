#ifndef WIRE_H
#define WIRE_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include "Types.h"
#include "Component.h"

class Wire {
private:
    Component* startComp = nullptr;
    int startPinIdx = -1;
    Component* endComp = nullptr;
    int endPinIdx = -1;
    bool selected = false;

public:
    Wire() = default;
    Wire(Component* startC, int startP, Component* endC, int endP);

    Wire(const Wire&) = default;
    Wire& operator=(const Wire&) = default;
    Wire(Wire&&) noexcept = default;
    Wire& operator=(Wire&&) noexcept = default;
    ~Wire() = default;

    Component* getStartComponent() const;
    Component* getEndComponent() const;
    int getStartPinIndex() const;
    int getEndPinIndex() const;

    bool isSelected() const { return selected; }
    void setSelected(bool sel) { selected = sel; }

    std::vector<Point2D> calculateRoutePoints() const;
    bool connectsToComponent(const Component* comp) const {
        return startComp == comp || endComp == comp;
    }

    bool isPointNearWire(Point2D p, float threshold = 6.0f) const;

    void update();
    void render(SDL_Renderer* renderer, Point2D offset, float zoom, SimState simState);

    std::string serialize() const;
};

#endif // WIRE_H