#include "Component.h"
#include <SDL2/SDL_timer.h>
#include <utility>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

// ==========================================
// Base Component Implementation
// ==========================================

Component::Component(std::string id, Point2D pos, ComponentType type, std::string label)
        : id(std::move(id)), position(pos), type(type), label(std::move(label)) {}

void Component::renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, float x, float y, SDL_Color color) {
    if (!font || text.empty()) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        SDL_FRect destRect = { x, y, static_cast<float>(surface->w), static_cast<float>(surface->h) };
        SDL_RenderCopyF(renderer, texture, nullptr, &destRect);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

void Component::renderLabel(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    Point2D screenPos = (position - cameraOffset) * zoom;

    // نمایش شناسه و مقدار (valueLabel) در پایین المان
    std::string displayText = id;
    if (!valueLabel.empty()) {
        displayText += " (" + valueLabel + ")";
    } else if (!label.empty() && label != id) {
        displayText += " (" + label + ")";
    }

    SDL_Color textColor = isSelected ? SDL_Color{255, 255, 0, 255} : SDL_Color{200, 220, 255, 255};
    // انتقال متن به پایین المان (45 پیکسل پایین‌تر از نقطه مبدأ)
    renderText(renderer, font, displayText, screenPos.x, screenPos.y + 45.0f * zoom, textColor);
}

void Component::renderPins(SDL_Renderer* renderer, Point2D cameraOffset, float zoom) {
    for (size_t i = 0; i < pins.size(); ++i) {
        Point2D pinWorldPos = getPinWorldPosition(i);
        Point2D pinScreenPos = (pinWorldPos - cameraOffset) * zoom;

        if (pins[i].state == LogicState::HIGH_STATE) {
            SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
        } else if (pins[i].state == LogicState::LOW_STATE) {
            SDL_SetRenderDrawColor(renderer, 50, 50, 255, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
        }

        SDL_FRect pinRect = { pinScreenPos.x - 4.0f * zoom, pinScreenPos.y - 4.0f * zoom, 8.0f * zoom, 8.0f * zoom };
        SDL_RenderFillRectF(renderer, &pinRect);
    }
}

// ==========================================
// Digital Logic Gates & Basic Components
// ==========================================

// --- Gate AND ---
GateAND::GateAND(const std::string& id, Point2D pos) : Component(id, pos, ComponentType::GATE_AND, "AND") {
    pins.push_back({0, "IN1", {0.0f, 10.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "IN2", {0.0f, 30.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({2, "OUT", {60.0f, 20.0f}, PinType::OUTPUT, LogicState::LOW_STATE, 0.0f});
}
void GateAND::evaluate() {
    bool in1 = (pins[0].state == LogicState::HIGH_STATE);
    bool in2 = (pins[1].state == LogicState::HIGH_STATE);
    pins[2].state = (in1 && in2) ? LogicState::HIGH_STATE : LogicState::LOW_STATE;
    pins[2].voltage = (pins[2].state == LogicState::HIGH_STATE) ? 5.0f : 0.0f;
}
void GateAND::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 200, 250, 255);
    drawTransformedLine(renderer, {10.0f, 0.0f}, {10.0f, 40.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 0.0f}, {35.0f, 0.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 40.0f}, {35.0f, 40.0f}, cameraOffset, zoom);
    for (float angle = -static_cast<float>(M_PI_2); angle <= static_cast<float>(M_PI_2); angle += 0.05f) {
        Point2D localP = { 35.0f + std::cos(angle) * 20.0f, 20.0f + std::sin(angle) * 20.0f };
        drawTransformedPoint(renderer, localP, cameraOffset, zoom);
    }
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Gate OR ---
GateOR::GateOR(const std::string& id, Point2D pos) : Component(id, pos, ComponentType::GATE_OR, "OR") {
    pins.push_back({0, "IN1", {0.0f, 10.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "IN2", {0.0f, 30.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({2, "OUT", {60.0f, 20.0f}, PinType::OUTPUT, LogicState::LOW_STATE, 0.0f});
}
void GateOR::evaluate() {
    bool in1 = (pins[0].state == LogicState::HIGH_STATE);
    bool in2 = (pins[1].state == LogicState::HIGH_STATE);
    pins[2].state = (in1 || in2) ? LogicState::HIGH_STATE : LogicState::LOW_STATE;
    pins[2].voltage = (pins[2].state == LogicState::HIGH_STATE) ? 5.0f : 0.0f;
}
void GateOR::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 200, 250, 255);
    for (float t = -1.0f; t <= 1.0f; t += 0.05f) {
        Point2D localP = { 10.0f + (1.0f - t*t) * 8.0f, 20.0f + t * 20.0f };
        drawTransformedPoint(renderer, localP, cameraOffset, zoom);
    }
    drawTransformedLine(renderer, {10.0f, 0.0f}, {35.0f, 0.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 40.0f}, {35.0f, 40.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {35.0f, 0.0f}, {60.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {35.0f, 40.0f}, {60.0f, 20.0f}, cameraOffset, zoom);
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Gate NOT ---
GateNOT::GateNOT(const std::string& id, Point2D pos) : Component(id, pos, ComponentType::GATE_NOT, "NOT") {
    pins.push_back({0, "IN", {0.0f, 20.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "OUT", {50.0f, 20.0f}, PinType::OUTPUT, LogicState::HIGH_STATE, 5.0f});
}
void GateNOT::evaluate() {
    bool in = (pins[0].state == LogicState::HIGH_STATE);
    pins[1].state = (!in) ? LogicState::HIGH_STATE : LogicState::LOW_STATE;
    pins[1].voltage = (pins[1].state == LogicState::HIGH_STATE) ? 5.0f : 0.0f;
}
void GateNOT::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 200, 250, 255);
    drawTransformedLine(renderer, {10.0f, 5.0f}, {10.0f, 35.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 5.0f}, {38.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 35.0f}, {38.0f, 20.0f}, cameraOffset, zoom);
    for (float a = 0; a < 2 * static_cast<float>(M_PI); a += 0.1f) {
        Point2D localP = { 42.0f + std::cos(a)*4.0f, 20.0f + std::sin(a)*4.0f };
        drawTransformedPoint(renderer, localP, cameraOffset, zoom);
    }
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Gate NAND ---
GateNAND::GateNAND(const std::string& id, Point2D pos) : Component(id, pos, ComponentType::GATE_NAND, "NAND") {
    pins.push_back({0, "IN1", {0.0f, 10.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "IN2", {0.0f, 30.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({2, "OUT", {60.0f, 20.0f}, PinType::OUTPUT, LogicState::HIGH_STATE, 5.0f});
}
void GateNAND::evaluate() {
    bool in1 = (pins[0].state == LogicState::HIGH_STATE);
    bool in2 = (pins[1].state == LogicState::HIGH_STATE);
    pins[2].state = (!(in1 && in2)) ? LogicState::HIGH_STATE : LogicState::LOW_STATE;
    pins[2].voltage = (pins[2].state == LogicState::HIGH_STATE) ? 5.0f : 0.0f;
}
void GateNAND::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 200, 250, 255);
    drawTransformedLine(renderer, {10.0f, 0.0f}, {10.0f, 40.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 0.0f}, {31.0f, 0.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 40.0f}, {31.0f, 40.0f}, cameraOffset, zoom);
    for (float angle = -static_cast<float>(M_PI_2); angle <= static_cast<float>(M_PI_2); angle += 0.05f) {
        Point2D localP = { 31.0f + std::cos(angle) * 20.0f, 20.0f + std::sin(angle) * 20.0f };
        drawTransformedPoint(renderer, localP, cameraOffset, zoom);
    }
    for (float a = 0; a < 2 * static_cast<float>(M_PI); a += 0.1f) {
        Point2D localP = { 54.0f + std::cos(a)*4.0f, 20.0f + std::sin(a)*4.0f };
        drawTransformedPoint(renderer, localP, cameraOffset, zoom);
    }
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Gate NOR ---
GateNOR::GateNOR(const std::string& id, Point2D pos) : Component(id, pos, ComponentType::GATE_NOR, "NOR") {
    pins.push_back({0, "IN1", {0.0f, 10.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "IN2", {0.0f, 30.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({2, "OUT", {60.0f, 20.0f}, PinType::OUTPUT, LogicState::HIGH_STATE, 5.0f});
}
void GateNOR::evaluate() {
    bool in1 = (pins[0].state == LogicState::HIGH_STATE);
    bool in2 = (pins[1].state == LogicState::HIGH_STATE);
    pins[2].state = (!(in1 || in2)) ? LogicState::HIGH_STATE : LogicState::LOW_STATE;
    pins[2].voltage = (pins[2].state == LogicState::HIGH_STATE) ? 5.0f : 0.0f;
}
void GateNOR::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 200, 250, 255);
    for (float t = -1.0f; t <= 1.0f; t += 0.05f) {
        Point2D localP = { 10.0f + (1.0f - t*t) * 8.0f, 20.0f + t * 20.0f };
        drawTransformedPoint(renderer, localP, cameraOffset, zoom);
    }
    drawTransformedLine(renderer, {10.0f, 0.0f}, {31.0f, 0.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 40.0f}, {31.0f, 40.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {31.0f, 0.0f}, {53.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {31.0f, 40.0f}, {53.0f, 20.0f}, cameraOffset, zoom);
    for (float a = 0; a < 2 * static_cast<float>(M_PI); a += 0.1f) {
        Point2D localP = { 54.0f + std::cos(a)*4.0f, 20.0f + std::sin(a)*4.0f };
        drawTransformedPoint(renderer, localP, cameraOffset, zoom);
    }
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Gate XOR ---
GateXOR::GateXOR(const std::string& id, Point2D pos) : Component(id, pos, ComponentType::GATE_XOR, "XOR") {
    pins.push_back({0, "IN1", {0.0f, 10.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "IN2", {0.0f, 30.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({2, "OUT", {60.0f, 20.0f}, PinType::OUTPUT, LogicState::LOW_STATE, 0.0f});
}
void GateXOR::evaluate() {
    bool in1 = (pins[0].state == LogicState::HIGH_STATE);
    bool in2 = (pins[1].state == LogicState::HIGH_STATE);
    pins[2].state = (in1 != in2) ? LogicState::HIGH_STATE : LogicState::LOW_STATE;
    pins[2].voltage = (pins[2].state == LogicState::HIGH_STATE) ? 5.0f : 0.0f;
}
void GateXOR::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 200, 250, 255);
    for (float t = -1.0f; t <= 1.0f; t += 0.05f) {
        Point2D localP = { 6.0f + (1.0f - t*t) * 8.0f, 20.0f + t * 20.0f };
        drawTransformedPoint(renderer, localP, cameraOffset, zoom);
    }
    for (float t = -1.0f; t <= 1.0f; t += 0.05f) {
        Point2D localP = { 10.0f + (1.0f - t*t) * 8.0f, 20.0f + t * 20.0f };
        drawTransformedPoint(renderer, localP, cameraOffset, zoom);
    }
    drawTransformedLine(renderer, {10.0f, 0.0f}, {35.0f, 0.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 40.0f}, {35.0f, 40.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {35.0f, 0.0f}, {60.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {35.0f, 40.0f}, {60.0f, 20.0f}, cameraOffset, zoom);
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Gate XNOR ---
GateXNOR::GateXNOR(const std::string& id, Point2D pos) : Component(id, pos, ComponentType::GATE_XNOR, "XNOR") {
    pins.push_back({0, "IN1", {0.0f, 10.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "IN2", {0.0f, 30.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({2, "OUT", {60.0f, 20.0f}, PinType::OUTPUT, LogicState::HIGH_STATE, 5.0f});
}
void GateXNOR::evaluate() {
    bool in1 = (pins[0].state == LogicState::HIGH_STATE);
    bool in2 = (pins[1].state == LogicState::HIGH_STATE);
    pins[2].state = (in1 == in2) ? LogicState::HIGH_STATE : LogicState::LOW_STATE;
    pins[2].voltage = (pins[2].state == LogicState::HIGH_STATE) ? 5.0f : 0.0f;
}
void GateXNOR::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 200, 250, 255);
    for (float t = -1.0f; t <= 1.0f; t += 0.05f) {
        Point2D localP = { 6.0f + (1.0f - t*t) * 8.0f, 20.0f + t * 20.0f };
        drawTransformedPoint(renderer, localP, cameraOffset, zoom);
    }
    for (float t = -1.0f; t <= 1.0f; t += 0.05f) {
        Point2D localP = { 10.0f + (1.0f - t*t) * 8.0f, 20.0f + t * 20.0f };
        drawTransformedPoint(renderer, localP, cameraOffset, zoom);
    }
    drawTransformedLine(renderer, {10.0f, 0.0f}, {31.0f, 0.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 40.0f}, {31.0f, 40.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {31.0f, 0.0f}, {53.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {31.0f, 40.0f}, {53.0f, 20.0f}, cameraOffset, zoom);
    for (float a = 0; a < 2 * static_cast<float>(M_PI); a += 0.1f) {
        Point2D localP = { 54.0f + std::cos(a)*4.0f, 20.0f + std::sin(a)*4.0f };
        drawTransformedPoint(renderer, localP, cameraOffset, zoom);
    }
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Switch ---
SwitchComponent::SwitchComponent(const std::string& id, Point2D pos) : Component(id, pos, ComponentType::SWITCH, "SW") {
    pins.push_back({0, "OUT", {40.0f, 20.0f}, PinType::OUTPUT, LogicState::LOW_STATE, 0.0f});
}
void SwitchComponent::evaluate() {
    pins[0].state = state ? LogicState::HIGH_STATE : LogicState::LOW_STATE;
    pins[0].voltage = state ? 5.0f : 0.0f;
}
void SwitchComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 200, 200, 200, 255);
    drawTransformedLine(renderer, {10.0f, 10.0f}, {30.0f, 10.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {30.0f, 10.0f}, {30.0f, 30.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {30.0f, 30.0f}, {10.0f, 30.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 30.0f}, {10.0f, 10.0f}, cameraOffset, zoom);
    if (state) SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    else SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    drawTransformedLine(renderer, {15.0f, 20.0f}, {state ? 28.0f : 12.0f, 20.0f}, cameraOffset, zoom);
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Constant 0 ---
Constant0Component::Constant0Component(const std::string& id, Point2D pos) : Component(id, pos, ComponentType::CONST_0, "GND") {
    pins.push_back({0, "OUT", {30.0f, 20.0f}, PinType::OUTPUT, LogicState::LOW_STATE, 0.0f});
}
void Constant0Component::evaluate() {
    pins[0].state = LogicState::LOW_STATE;
    pins[0].voltage = 0.0f;
}
void Constant0Component::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    Point2D sp = (position + transformLocalPoint({10.0f, 10.0f}) - cameraOffset) * zoom;
    renderText(renderer, font, "0", sp.x, sp.y, {0, 150, 255, 255});
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Constant 1 ---
Constant1Component::Constant1Component(const std::string& id, Point2D pos) : Component(id, pos, ComponentType::CONST_1, "VCC") {
    pins.push_back({0, "OUT", {30.0f, 20.0f}, PinType::OUTPUT, LogicState::HIGH_STATE, 5.0f});
}
void Constant1Component::evaluate() {
    pins[0].state = LogicState::HIGH_STATE;
    pins[0].voltage = 5.0f;
}
void Constant1Component::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    Point2D sp = (position + transformLocalPoint({10.0f, 10.0f}) - cameraOffset) * zoom;
    renderText(renderer, font, "1", sp.x, sp.y, {255, 50, 50, 255});
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- LED ---
LEDComponent::LEDComponent(const std::string& id, Point2D pos) : Component(id, pos, ComponentType::LED, "LED") {
    pins.push_back({0, "IN", {0.0f, 20.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
}
void LEDComponent::evaluate() {}
void LEDComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    bool lit = (pins[0].state == LogicState::HIGH_STATE || pins[0].voltage >= 2.0f);
    if (lit) SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    else SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (float r = 1.0f; r <= 12.0f; r += 2.0f) {
        for (float a = 0; a < 2 * static_cast<float>(M_PI); a += 0.2f) {
            Point2D localP = { 30.0f + std::cos(a)*r, 20.0f + std::sin(a)*r };
            drawTransformedPoint(renderer, localP, cameraOffset, zoom);
        }
    }
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Clock ---
ClockComponent::ClockComponent(const std::string& id, Point2D pos, float freq)
        : Component(id, pos, ComponentType::CLOCK, "CLK"), frequency(freq) {
    pins.push_back({0, "OUT", {40.0f, 20.0f}, PinType::OUTPUT, LogicState::LOW_STATE, 0.0f});
}
void ClockComponent::evaluate() {
    Uint32 currentTicks = SDL_GetTicks();
    float periodMs = (1.0f / frequency) * 1000.0f;
    bool state = (static_cast<int>(currentTicks / (periodMs / 2.0f)) % 2) == 0;
    pins[0].state = state ? LogicState::HIGH_STATE : LogicState::LOW_STATE;
    pins[0].voltage = state ? 5.0f : 0.0f;
}
void ClockComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 200, 255, 255);
    drawTransformedLine(renderer, {10.0f, 10.0f}, {30.0f, 10.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {30.0f, 10.0f}, {30.0f, 30.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {30.0f, 30.0f}, {10.0f, 30.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 30.0f}, {10.0f, 10.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {14.0f, 24.0f}, {18.0f, 24.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {18.0f, 24.0f}, {18.0f, 16.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {18.0f, 16.0f}, {22.0f, 16.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {22.0f, 16.0f}, {22.0f, 24.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {22.0f, 24.0f}, {26.0f, 24.0f}, cameraOffset, zoom);
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// ==========================================
// Measurement Instruments
// ==========================================

// --- Voltmeter ---
VoltmeterComponent::VoltmeterComponent(const std::string& id, Point2D pos) : Component(id, pos, ComponentType::VOLTMETER, "VM") {
    pins.push_back({0, "V+", {0.0f, 10.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "V-", {0.0f, 30.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
}
void VoltmeterComponent::evaluate() {}
void VoltmeterComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 40, 180, 220, 255);
    float radius = 18.0f;
    Point2D center = { width * 0.5f, height * 0.5f };
    for (int angle = 0; angle < 360; angle += 10) {
        float rad = angle * static_cast<float>(M_PI) / 180.0f;
        Point2D localP = { center.x + std::cos(rad) * radius, center.y + std::sin(rad) * radius };
        drawTransformedPoint(renderer, localP, cameraOffset, zoom);
    }
    float diffVolts = pins[0].voltage - pins[1].voltage;
    std::string valStr = std::to_string(diffVolts).substr(0, 4) + "V";
    Point2D textWorldPos = position + transformLocalPoint({ center.x - 12.0f, center.y - 6.0f });
    Point2D textScreenPos = (textWorldPos - cameraOffset) * zoom;
    renderText(renderer, font, valStr, textScreenPos.x, textScreenPos.y, {0, 255, 120, 255});
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Amperemeter ---
AmperemeterComponent::AmperemeterComponent(const std::string& id, Point2D pos)
        : Component(id, pos, ComponentType::AMPEREMETER, "AM") {
    pins.push_back({0, "A+", {0.0f, 20.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "A-", {60.0f, 20.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});
}
void AmperemeterComponent::evaluate() {
    pins[1].voltage = pins[0].voltage;
    measuredCurrent = pins[0].current;
}
void AmperemeterComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 255, 180, 255);
    float radius = 18.0f;
    Point2D center = { width * 0.5f, height * 0.5f };
    for (int a = 0; a < 360; a += 10) {
        float rad = a * static_cast<float>(M_PI) / 180.0f;
        drawTransformedPoint(renderer, {center.x + std::cos(rad)*radius, center.y + std::sin(rad)*radius}, cameraOffset, zoom);
    }
    Point2D textPos = (position + transformLocalPoint({center.x - 10.0f, center.y - 6.0f}) - cameraOffset) * zoom;
    std::string iStr = std::to_string(measuredCurrent * 1000.0f).substr(0, 4) + "mA";
    renderText(renderer, font, iStr, textPos.x, textPos.y, {255, 255, 0, 255});
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// ==========================================
// Analog Basic Passive Components & Sources
// ==========================================

// --- Ground ---
GroundComponent::GroundComponent(const std::string& id, Point2D pos)
        : Component(id, pos, ComponentType::GROUND, "GND") {
    width = 60.0f;
    height = 40.0f;
    pins.push_back({0, "GND", {30.0f, 0.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});
}
void GroundComponent::evaluate() {
    pins[0].voltage = 0.0f;
    pins[0].state = LogicState::LOW_STATE;
}
void GroundComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 200, 200, 200, 255);
    drawTransformedLine(renderer, {30.0f, 0.0f}, {30.0f, 15.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {15.0f, 15.0f}, {45.0f, 15.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {22.0f, 22.0f}, {38.0f, 22.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {27.0f, 29.0f}, {33.0f, 29.0f}, cameraOffset, zoom);
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Resistor ---
ResistorComponent::ResistorComponent(const std::string& id, Point2D pos, float r)
        : Component(id, pos, ComponentType::RESISTOR, "R"), resistance(r) {
    width = 60.0f;
    height = 40.0f;
    pins.push_back({0, "p1", {0.0f, 20.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "p2", {60.0f, 20.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});
}
void ResistorComponent::evaluate() {
    float current = (pins[0].voltage - pins[1].voltage) / resistance;
    pins[0].current = current;
    pins[1].current = -current;
}
void ResistorComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 220, 255, 255);
    // اتصال لبه پین 0 و 60 به بدنه زیگزاگ مقاومت
    drawTransformedLine(renderer, {0.0f, 20.0f}, {12.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {48.0f, 20.0f}, {60.0f, 20.0f}, cameraOffset, zoom);

    Point2D zigZag[] = {{12.0f, 20.0f}, {15.0f, 10.0f}, {21.0f, 30.0f}, {27.0f, 10.0f},
                        {33.0f, 30.0f}, {39.0f, 10.0f}, {45.0f, 30.0f}, {48.0f, 20.0f}};
    for (int i = 0; i < 7; ++i) {
        drawTransformedLine(renderer, zigZag[i], zigZag[i+1], cameraOffset, zoom);
    }
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Capacitor ---
CapacitorComponent::CapacitorComponent(const std::string& id, Point2D pos, float c)
        : Component(id, pos, ComponentType::CAPACITOR, "C"), capacitance(c) {
    width = 60.0f;
    height = 40.0f;
    pins.push_back({0, "p1", {0.0f, 20.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "p2", {60.0f, 20.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});
}
void CapacitorComponent::evaluate() {
    float dt = 0.001f;
    float vDiff = pins[0].voltage - pins[1].voltage;
    float current = capacitance * (vDiff - prevVoltageDiff) / dt;
    prevVoltageDiff = vDiff;
    pins[0].current = current;
    pins[1].current = -current;
}
void CapacitorComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 220, 255, 255);
    drawTransformedLine(renderer, {0.0f, 20.0f}, {26.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {34.0f, 20.0f}, {60.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {26.0f, 5.0f}, {26.0f, 35.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {34.0f, 5.0f}, {34.0f, 35.0f}, cameraOffset, zoom);
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Inductor ---
InductorComponent::InductorComponent(const std::string& id, Point2D pos, float l)
        : Component(id, pos, ComponentType::INDUCTOR, "L"), inductance(l) {
    width = 60.0f;
    height = 40.0f;
    pins.push_back({0, "p1", {0.0f, 20.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "p2", {60.0f, 20.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});
}
void InductorComponent::evaluate() {
    float dt = 0.001f;
    float vDiff = pins[0].voltage - pins[1].voltage;
    current += (vDiff / inductance) * dt;
    pins[0].current = current;
    pins[1].current = -current;
}
void InductorComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 220, 255, 255);
    drawTransformedLine(renderer, {0.0f, 20.0f}, {10.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {50.0f, 20.0f}, {60.0f, 20.0f}, cameraOffset, zoom);
    for (int i = 0; i < 4; ++i) {
        float cx = 15.0f + i * 10.0f;
        for (float a = 0; a < static_cast<float>(M_PI); a += 0.2f) {
            drawTransformedPoint(renderer, {cx + std::cos(a)*5.0f, 20.0f - std::sin(a)*8.0f}, cameraOffset, zoom);
        }
    }
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Diode ---
DiodeComponent::DiodeComponent(const std::string& id, Point2D pos)
        : Component(id, pos, ComponentType::DIODE, "D") {
    width = 60.0f;
    height = 40.0f;
    pins.push_back({0, "A", {0.0f, 20.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "K", {60.0f, 20.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});
}
void DiodeComponent::evaluate() {
    float vDiff = pins[0].voltage - pins[1].voltage;
    if (vDiff >= vForward) {
        pins[1].voltage = pins[0].voltage - vForward;
        pins[1].state = LogicState::HIGH_STATE;
    } else {
        pins[1].state = LogicState::LOW_STATE;
    }
}
void DiodeComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 220, 255, 255);
    drawTransformedLine(renderer, {0.0f, 20.0f}, {20.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {40.0f, 20.0f}, {60.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {20.0f, 10.0f}, {20.0f, 30.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {20.0f, 10.0f}, {40.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {20.0f, 30.0f}, {40.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {40.0f, 10.0f}, {40.0f, 30.0f}, cameraOffset, zoom);
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Zener Diode ---
ZenerDiodeComponent::ZenerDiodeComponent(const std::string& id, Point2D pos, float vz)
        : Component(id, pos, ComponentType::DIODE_ZENER, "DZ"), vZener(vz) {
    width = 60.0f;
    height = 40.0f;
    pins.push_back({0, "A", {0.0f, 20.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "K", {60.0f, 20.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});
}
void ZenerDiodeComponent::evaluate() {
    float vDiff = pins[0].voltage - pins[1].voltage;
    if (vDiff >= vForward) {
        pins[1].voltage = pins[0].voltage - vForward;
    } else if (-vDiff >= vZener) {
        pins[0].voltage = pins[1].voltage - vZener;
    }
}
void ZenerDiodeComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 220, 255, 255);
    drawTransformedLine(renderer, {0.0f, 20.0f}, {20.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {40.0f, 20.0f}, {60.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {20.0f, 10.0f}, {20.0f, 30.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {20.0f, 10.0f}, {40.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {20.0f, 30.0f}, {40.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {40.0f, 10.0f}, {40.0f, 30.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {40.0f, 10.0f}, {35.0f, 5.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {40.0f, 30.0f}, {45.0f, 35.0f}, cameraOffset, zoom);
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Voltage Source ---
VoltageSourceComponent::VoltageSourceComponent(const std::string& id, Point2D pos, float v)
        : Component(id, pos, ComponentType::VOLTAGE_SOURCE, "VS"), voltage(v) {
    pins.push_back({0, "V+", {60.0f, 20.0f}, PinType::OUTPUT, LogicState::HIGH_STATE, v});
    pins.push_back({1, "V-", {0.0f, 20.0f}, PinType::OUTPUT, LogicState::LOW_STATE, 0.0f});
}
void VoltageSourceComponent::evaluate() {
    pins[0].voltage = pins[1].voltage + voltage;
    pins[0].state = (pins[0].voltage > 2.5f) ? LogicState::HIGH_STATE : LogicState::LOW_STATE;
}
void VoltageSourceComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 255, 200, 50, 255);
    drawTransformedLine(renderer, {0.0f, 20.0f}, {15.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {45.0f, 20.0f}, {60.0f, 20.0f}, cameraOffset, zoom);
    for (int a = 0; a < 360; a += 10) {
        float rad = a * static_cast<float>(M_PI) / 180.0f;
        drawTransformedPoint(renderer, {30.0f + std::cos(rad)*15.0f, 20.0f + std::sin(rad)*15.0f}, cameraOffset, zoom);
    }
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Battery ---
BatteryComponent::BatteryComponent(const std::string& id, Point2D pos, float v)
        : Component(id, pos, ComponentType::BATTERY, "BAT"), voltage(v) {
    pins.push_back({0, "V+", {60.0f, 20.0f}, PinType::OUTPUT, LogicState::HIGH_STATE, v});
    pins.push_back({1, "V-", {0.0f, 20.0f}, PinType::OUTPUT, LogicState::LOW_STATE, 0.0f});
}
void BatteryComponent::evaluate() {
    pins[0].voltage = pins[1].voltage + voltage;
    pins[0].state = (pins[0].voltage > 2.5f) ? LogicState::HIGH_STATE : LogicState::LOW_STATE;
}
void BatteryComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 255, 200, 50, 255);
    drawTransformedLine(renderer, {0.0f, 20.0f}, {25.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {35.0f, 20.0f}, {60.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {35.0f, 5.0f}, {35.0f, 35.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {25.0f, 12.0f}, {25.0f, 28.0f}, cameraOffset, zoom);
    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// ==========================================
// Advanced Active Devices
// ==========================================

// --- BJT NPN Transistor ---
BJTNPNComponent::BJTNPNComponent(const std::string& id, Point2D pos)
        : Component(id, pos, ComponentType::TRANSISTOR_NPN, "Q_NPN") {
    pins.push_back({0, "B", {0.0f, 20.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});   // Base
    pins.push_back({1, "C", {40.0f, 0.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});  // Collector
    pins.push_back({2, "E", {40.0f, 40.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f}); // Emitter
}

void BJTNPNComponent::evaluate() {
    float vBE = pins[0].voltage - pins[2].voltage;
    if (vBE >= 0.7f) {
        pins[1].state = LogicState::HIGH_STATE;
        pins[2].voltage = pins[1].voltage - 0.2f;
    } else {
        pins[1].state = LogicState::LOW_STATE;
    }
}

void BJTNPNComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 220, 255, 255);
    drawTransformedLine(renderer, {0.0f, 20.0f}, {18.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {18.0f, 8.0f}, {18.0f, 32.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {18.0f, 12.0f}, {35.0f, 3.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {35.0f, 3.0f}, {40.0f, 3.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {40.0f, 3.0f}, {40.0f, 0.0f}, cameraOffset, zoom);

    drawTransformedLine(renderer, {18.0f, 28.0f}, {35.0f, 37.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {35.0f, 37.0f}, {40.0f, 37.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {40.0f, 37.0f}, {40.0f, 40.0f}, cameraOffset, zoom);

    drawTransformedLine(renderer, {35.0f, 37.0f}, {28.0f, 35.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {35.0f, 37.0f}, {32.0f, 29.0f}, cameraOffset, zoom);

    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- Operational Amplifier (Op-Amp) ---
OpAmpComponent::OpAmpComponent(const std::string& id, Point2D pos)
        : Component(id, pos, ComponentType::OPAMP, "U") {
    pins.push_back({0, "IN-", {0.0f, 10.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({1, "IN+", {0.0f, 30.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});
    pins.push_back({2, "OUT", {50.0f, 20.0f}, PinType::OUTPUT, LogicState::LOW_STATE, 0.0f});
}

void OpAmpComponent::evaluate() {
    float diff = pins[1].voltage - pins[0].voltage;
    float vOut = diff * 100000.0f;

    if (vOut > 15.0f) vOut = 15.0f;
    if (vOut < -15.0f) vOut = -15.0f;

    pins[2].voltage = vOut;
    pins[2].state = (vOut > 2.5f) ? LogicState::HIGH_STATE : LogicState::LOW_STATE;
}

void OpAmpComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 220, 255, 255);
    drawTransformedLine(renderer, {10.0f, 0.0f}, {10.0f, 40.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 0.0f}, {45.0f, 20.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {10.0f, 40.0f}, {45.0f, 20.0f}, cameraOffset, zoom);

    drawTransformedLine(renderer, {0.0f, 10.0f}, {10.0f, 10.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {0.0f, 30.0f}, {10.0f, 30.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {45.0f, 20.0f}, {50.0f, 20.0f}, cameraOffset, zoom);

    drawTransformedLine(renderer, {13.0f, 10.0f}, {19.0f, 10.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {13.0f, 30.0f}, {19.0f, 30.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {16.0f, 27.0f}, {16.0f, 33.0f}, cameraOffset, zoom);

    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}

// --- N-Channel MOSFET ---
MOSFETNChanComponent::MOSFETNChanComponent(const std::string& id, Point2D pos)
        : Component(id, pos, ComponentType::MOSFET_N, "Q_NMOS") {
    pins.push_back({0, "G", {0.0f, 30.0f}, PinType::INPUT, LogicState::LOW_STATE, 0.0f});   // Gate
    pins.push_back({1, "D", {40.0f, 0.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f});  // Drain
    pins.push_back({2, "S", {40.0f, 40.0f}, PinType::PASSIVE, LogicState::LOW_STATE, 0.0f}); // Source
}

void MOSFETNChanComponent::evaluate() {
    float vGS = pins[0].voltage - pins[2].voltage;
    float vTh = 2.0f;
    if (vGS >= vTh) {
        pins[1].voltage = pins[2].voltage;
        pins[1].state = LogicState::HIGH_STATE;
    } else {
        pins[1].state = LogicState::LOW_STATE;
    }
}

void MOSFETNChanComponent::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom) {
    SDL_SetRenderDrawColor(renderer, isSelected ? 255 : 0, 220, 255, 255);
    drawTransformedLine(renderer, {0.0f, 30.0f}, {14.0f, 30.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {14.0f, 10.0f}, {14.0f, 30.0f}, cameraOffset, zoom);

    drawTransformedLine(renderer, {18.0f, 8.0f}, {18.0f, 16.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {18.0f, 18.0f}, {18.0f, 24.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {18.0f, 26.0f}, {18.0f, 34.0f}, cameraOffset, zoom);

    drawTransformedLine(renderer, {18.0f, 10.0f}, {40.0f, 10.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {40.0f, 10.0f}, {40.0f, 0.0f}, cameraOffset, zoom);

    drawTransformedLine(renderer, {18.0f, 30.0f}, {40.0f, 30.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {40.0f, 30.0f}, {40.0f, 40.0f}, cameraOffset, zoom);

    drawTransformedLine(renderer, {18.0f, 21.0f}, {25.0f, 18.0f}, cameraOffset, zoom);
    drawTransformedLine(renderer, {18.0f, 21.0f}, {25.0f, 24.0f}, cameraOffset, zoom);

    renderLabel(renderer, font, cameraOffset, zoom);
    renderPins(renderer, cameraOffset, zoom);
}