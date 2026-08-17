#ifndef COMPONENT_H
#define COMPONENT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <cmath>
#include "Types.h"

class Component {
protected:
    std::string id;
    Point2D position;
    ComponentType type;
    std::string label;
    std::vector<Pin> pins;
    bool isSelected = false;

    float width = 60.0f;
    float height = 40.0f;

    Orientation orientation = Orientation::DEG_0;
    bool mirroredH = false;
    bool mirroredV = false;

    std::string valueLabel = ""; // اضافه شده برای نگهداری مقدار متنی المان‌ها

public:
    Component(std::string id, Point2D pos, ComponentType type, std::string label);
    virtual ~Component() = default;

    virtual void evaluate() = 0;
    virtual void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) = 0;

    // متدهای مربوط به مدیریت برچسب مقدار قطعات
    virtual void setValueLabel(const std::string& valLabel) { valueLabel = valLabel; }
    virtual const std::string& getValueLabel() const { return valueLabel; }

    void rotate() {
        switch (orientation) {
            case Orientation::DEG_0:   orientation = Orientation::DEG_90; break;
            case Orientation::DEG_90:  orientation = Orientation::DEG_180; break;
            case Orientation::DEG_180: orientation = Orientation::DEG_270; break;
            case Orientation::DEG_270: orientation = Orientation::DEG_0; break;
        }
    }

    void mirrorHorizontal() { mirroredH = !mirroredH; }
    void mirrorVertical() { mirroredV = !mirroredV; }

    Point2D transformLocalPoint(Point2D pt) const {
        Point2D center = { width * 0.5f, height * 0.5f };
        Point2D p = pt - center;

        if (mirroredH) p.x = -p.x;
        if (mirroredV) p.y = -p.y;

        Point2D rotated = p;
        switch (orientation) {
            case Orientation::DEG_0:   rotated = { p.x, p.y }; break;
            case Orientation::DEG_90:  rotated = { -p.y, p.x }; break;
            case Orientation::DEG_180: rotated = { -p.x, -p.y }; break;
            case Orientation::DEG_270: rotated = { p.y, -p.x }; break;
        }

        return center + rotated;
    }

    Point2D getPinWorldPosition(size_t pinIndex) const {
        if (pinIndex >= pins.size()) return position;
        Point2D localTransformed = transformLocalPoint(pins[pinIndex].relativePos);
        return position + localTransformed;
    }

    bool isInside(Point2D worldPos) const {
        return (worldPos.x >= position.x && worldPos.x <= position.x + width &&
                worldPos.y >= position.y && worldPos.y <= position.y + height);
    }

    void snapToGrid(float gridSize = 10.0f) {
        position.x = std::round(position.x / gridSize) * gridSize;
        position.y = std::round(position.y / gridSize) * gridSize;
    }

    std::string getId() const { return id; }
    Point2D getPosition() const { return position; }
    void setPosition(Point2D pos) { position = pos; }
    ComponentType getType() const { return type; }
    bool getSelected() const { return isSelected; }
    void setSelected(bool s) { isSelected = s; }
    const std::vector<Pin>& getPins() const { return pins; }
    std::vector<Pin>& getPinsMutable() { return pins; }

protected:
    void drawTransformedLine(SDL_Renderer* renderer, Point2D p1, Point2D p2, Point2D cameraOffset, float zoom) const {
        Point2D w1 = position + transformLocalPoint(p1);
        Point2D w2 = position + transformLocalPoint(p2);
        Point2D s1 = (w1 - cameraOffset) * zoom;
        Point2D s2 = (w2 - cameraOffset) * zoom;
        SDL_RenderDrawLineF(renderer, s1.x, s1.y, s2.x, s2.y);
    }

    void drawTransformedPoint(SDL_Renderer* renderer, Point2D p, Point2D cameraOffset, float zoom) const {
        Point2D w = position + transformLocalPoint(p);
        Point2D s = (w - cameraOffset) * zoom;
        SDL_RenderDrawPointF(renderer, s.x, s.y);
    }

    void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, float x, float y, SDL_Color color);
    void renderLabel(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom);
    void renderPins(SDL_Renderer* renderer, Point2D cameraOffset, float zoom);
};

// --- گیت‌ها و ابزارهای ورودی/خروجی ---
class GateAND : public Component {
public:
    GateAND(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

class GateOR : public Component {
public:
    GateOR(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

class GateNOT : public Component {
public:
    GateNOT(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

class GateNAND : public Component {
public:
    GateNAND(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

class GateNOR : public Component {
public:
    GateNOR(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

class GateXOR : public Component {
public:
    GateXOR(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

class GateXNOR : public Component {
public:
    GateXNOR(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

class SwitchComponent : public Component {
private:
    bool state = false;
public:
    SwitchComponent(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
    void toggle() { state = !state; }
};

class ClockComponent : public Component {
private:
    float frequency = 1.0f;
public:
    ClockComponent(const std::string& id, Point2D pos, float freq = 1.0f);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
    float getFrequency() const { return frequency; }
    void setFrequency(float f) { frequency = f; }
};

class Constant0Component : public Component {
public:
    Constant0Component(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

class Constant1Component : public Component {
public:
    Constant1Component(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

// --- LED با قابلیت انتخاب رنگ از طریق Properties ---
class LEDComponent : public Component {
public:
    enum class LEDColor { WHITE, BLUE, RED, GREEN };
private:
    LEDColor activeColor = LEDColor::WHITE;
public:
    LEDComponent(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;

    LEDColor getActiveColor() const { return activeColor; }
    void setActiveColor(LEDColor col) { activeColor = col; }
};

class VoltmeterComponent : public Component {
public:
    VoltmeterComponent(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

// --- قطعات آنالوگ ---
class GroundComponent : public Component {
public:
    GroundComponent(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

class ResistorComponent : public Component {
private:
    float resistance = 1000.0f;
public:
    ResistorComponent(const std::string& id, Point2D pos, float r = 1000.0f);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
    float getResistance() const { return resistance; }
    void setResistance(float r) { resistance = r; }
};

class CapacitorComponent : public Component {
private:
    float capacitance = 1e-6f;
    float prevVoltageDiff = 0.0f;
public:
    CapacitorComponent(const std::string& id, Point2D pos, float c = 1e-6f);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
    float getCapacitance() const { return capacitance; }
    void setCapacitance(float c) { capacitance = c; }
};

class InductorComponent : public Component {
private:
    float inductance = 1e-3f;
    float current = 0.0f;
public:
    InductorComponent(const std::string& id, Point2D pos, float l = 1e-3f);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
    float getInductance() const { return inductance; }
    void setInductance(float l) { inductance = l; }
};

class DiodeComponent : public Component {
private:
    float vForward = 0.7f;
public:
    DiodeComponent(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
    float getVForward() const { return vForward; }
    void setVForward(float vf) { vForward = vf; }
};

class ZenerDiodeComponent : public Component {
private:
    float vForward = 0.7f;
    float vZener = 5.1f;
public:
    ZenerDiodeComponent(const std::string& id, Point2D pos, float vz = 5.1f);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
    float getVZener() const { return vZener; }
    void setVZener(float vz) { vZener = vz; }
};

class VoltageSourceComponent : public Component {
private:
    float voltage = 5.0f;
public:
    VoltageSourceComponent(const std::string& id, Point2D pos, float v = 5.0f);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
    float getVoltage() const { return voltage; }
    void setVoltage(float v) { voltage = v; }
};

class BatteryComponent : public Component {
private:
    float voltage = 12.0f;
public:
    BatteryComponent(const std::string& id, Point2D pos, float v = 12.0f);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
    float getVoltage() const { return voltage; }
    void setVoltage(float v) { voltage = v; }
};

class AmperemeterComponent : public Component {
private:
    float measuredCurrent = 0.0f;
public:
    AmperemeterComponent(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

// --- قطعات فعال پیشرفته ---
class BJTNPNComponent : public Component {
public:
    BJTNPNComponent(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

class OpAmpComponent : public Component {
public:
    OpAmpComponent(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

class MOSFETNChanComponent : public Component {
public:
    MOSFETNChanComponent(const std::string& id, Point2D pos);
    void evaluate() override;
    void render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) override;
};

#endif // COMPONENT_H