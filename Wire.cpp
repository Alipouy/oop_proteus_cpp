#include "Wire.h"
#include <cmath>
#include <algorithm>

Wire::Wire(Component* startC, int startP, Component* endC, int endP)
        : startComp(startC), startPinIdx(startP), endComp(endC), endPinIdx(endP) {}

Component* Wire::getStartComponent() const { return startComp; }
Component* Wire::getEndComponent() const { return endComp; }
int Wire::getStartPinIndex() const { return startPinIdx; }
int Wire::getEndPinIndex() const { return endPinIdx; }

std::vector<Point2D> Wire::calculateRoutePoints() const {
    std::vector<Point2D> pts;
    if (!startComp || !endComp) return pts;

    Point2D p1 = startComp->getPinWorldPosition(startPinIdx);
    Point2D p2 = endComp->getPinWorldPosition(endPinIdx);

    // مسیردهی Manhattan (Orthogonal) استاندارد و پایدار
    pts.push_back(p1);

    // شکست هموژن برای جلوگیری از تداخل و پوشش درست فاصله بین پین‌ها
    float midX = p1.x + (p2.x - p1.x) * 0.5f;
    pts.push_back({midX, p1.y});
    pts.push_back({midX, p2.y});

    pts.push_back(p2);

    return pts;
}

bool Wire::isPointNearWire(Point2D p, float threshold) const {
    auto pts = calculateRoutePoints();
    if (pts.size() < 2) return false;

    for (size_t i = 0; i < pts.size() - 1; ++i) {
        Point2D a = pts[i];
        Point2D b = pts[i+1];

        float minX = std::min(a.x, b.x) - threshold;
        float maxX = std::max(a.x, b.x) + threshold;
        float minY = std::min(a.y, b.y) - threshold;
        float maxY = std::max(a.y, b.y) + threshold;

        if (p.x >= minX && p.x <= maxX && p.y >= minY && p.y <= maxY) {
            if (std::abs(a.x - b.x) < 0.001f) {
                if (std::abs(p.x - a.x) <= threshold) return true;
            }
            else if (std::abs(a.y - b.y) < 0.001f) {
                if (std::abs(p.y - a.y) <= threshold) return true;
            }
        }
    }
    return false;
}

void Wire::render(SDL_Renderer* renderer, Point2D offset, float zoom, SimState simState) {
    if (!startComp || !endComp) return;

    auto pts = calculateRoutePoints();
    if (pts.size() < 2) return;

    // اعمال قوانین رنگ‌بندی سیم‌ها بر اساس وضعیت شبیه‌سازی
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // زرد برای حالت انتخاب‌شده
    } else if (simState != SimState::STOPPED) {
        LogicState state = startComp->getPins()[startPinIdx].state;
        if (state == LogicState::HIGH_STATE) {
            SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255); // قرمز برای HIGH (1)
        } else if (state == LogicState::LOW_STATE) {
            SDL_SetRenderDrawColor(renderer, 50, 100, 255, 255); // آبی برای LOW (0)
        } else {
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255); // خاکستری برای Undefined
        }
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 180, 80, 255); // سبز استاندارد پیش‌فرض
    }

    // رسم بخش‌های مختلف سیم با در نظر گرفتن دوربین و زوم
    for (size_t i = 0; i < pts.size() - 1; ++i) {
        Point2D s1 = (pts[i] - offset) * zoom;
        Point2D s2 = (pts[i+1] - offset) * zoom;
        SDL_RenderDrawLineF(renderer, s1.x, s1.y, s2.x, s2.y);
    }
}

void Wire::update() {
    if (!startComp || !endComp) return;

    auto& startPins = startComp->getPinsMutable();
    auto& endPins = endComp->getPinsMutable();

    if (startPinIdx >= 0 && startPinIdx < static_cast<int>(startPins.size()) &&
        endPinIdx >= 0 && endPinIdx < static_cast<int>(endPins.size())) {

        auto& startPin = startPins[startPinIdx];
        auto& endPin = endPins[endPinIdx];

        // همگام‌سازی دوطرفه و مستقیم ولتاژ و وضعیت منطقی بین دو سر سیم
        endPin.voltage = startPin.voltage;
        endPin.state = startPin.state;
    }
}

std::string Wire::serialize() const {
    if (!startComp || !endComp) return "";
    return startComp->getId() + " " + std::to_string(startPinIdx) + " " +
           endComp->getId() + " " + std::to_string(endPinIdx);
}