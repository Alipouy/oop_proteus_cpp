#include "CircuitManager.h"
#include "ComponentFactory.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <iomanip>

void CircuitManager::addComponent(std::unique_ptr<Component> comp) {
    if (!comp) return;
    components.push_back(std::move(comp));
    saveStateHistory();
}

void CircuitManager::addWire(Component* srcComp, int srcPinIdx, Component* destComp, int destPinIdx) {
    if (!srcComp || !destComp) return;
    wires.emplace_back(srcComp, srcPinIdx, destComp, destPinIdx);
    saveStateHistory();
}

void CircuitManager::deleteComponent(Component* comp) {
    if (!comp) return;

    wires.erase(std::remove_if(wires.begin(), wires.end(), [comp](const Wire& w) {
        return w.getStartComponent() == comp || w.getEndComponent() == comp;
    }), wires.end());

    components.erase(std::remove_if(components.begin(), components.end(), [comp](const std::unique_ptr<Component>& c) {
        return c.get() == comp;
    }), components.end());

    saveStateHistory();
}

void CircuitManager::deleteWire(Wire* wire) {
    if (!wire) return;
    wires.erase(std::remove_if(wires.begin(), wires.end(), [wire](const Wire& w) {
        return &w == wire;
    }), wires.end());

    saveStateHistory();
}

void CircuitManager::deleteSelectedItems() {
    bool changed = false;

    auto wireIt = wires.begin();
    while (wireIt != wires.end()) {
        if (wireIt->isSelected()) {
            wireIt = wires.erase(wireIt);
            changed = true;
        } else {
            ++wireIt;
        }
    }

    auto compIt = components.begin();
    while (compIt != components.end()) {
        if ((*compIt)->getSelected()) {
            Component* cPtr = compIt->get();
            wires.erase(std::remove_if(wires.begin(), wires.end(), [cPtr](const Wire& w) {
                return w.getStartComponent() == cPtr || w.getEndComponent() == cPtr;
            }), wires.end());

            compIt = components.erase(compIt);
            changed = true;
        } else {
            ++compIt;
        }
    }

    if (changed) saveStateHistory();
}

void CircuitManager::clearSelections() {
    for (auto& c : components) c->setSelected(false);
    for (auto& w : wires) w.setSelected(false);
}

void CircuitManager::updateLogic() {
    for (auto& comp : components) comp->evaluate();
    for (auto& wire : wires) wire.update();
}

void CircuitManager::renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, float x, float y, SDL_Color color) {
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

void CircuitManager::render(SDL_Renderer* renderer, TTF_Font* font, Point2D cameraOffset, float zoom, SimState simState) {
    SDL_SetRenderDrawColor(renderer, 24, 24, 24, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 38, 38, 38, 255);
    float gridSize = 20.0f * zoom;

    float startX = 230.0f + fmodf(-cameraOffset.x * zoom, gridSize);
    for (float x = startX; x < 1280; x += gridSize) {
        SDL_RenderDrawLineF(renderer, x, 0, x, 720);
    }

    float startY = fmodf(-cameraOffset.y * zoom, gridSize);
    for (float y = startY; y < 720; y += gridSize) {
        SDL_RenderDrawLineF(renderer, 230, y, 1280, y);
    }

    for (auto& wire : wires) {
        wire.render(renderer, cameraOffset, zoom, simState);
    }

    for (auto& comp : components) {
        comp->render(renderer, font, cameraOffset, zoom);
    }
}

bool CircuitManager::findPinAt(const Point2D& worldPos, Component*& outComp, int& outPinIdx, float zoom) {
    // تنظیم آستانه کلیک متناسب با میزان زوم برای دقت بالا در بزرگنمایی‌های مختلف
    float threshold = 12.0f / (zoom > 0.0f ? zoom : 1.0f);
    for (auto& comp : components) {
        const auto& pins = comp->getPins();
        for (size_t i = 0; i < pins.size(); ++i) {
            // بررسی اینکه پین از هر نوعی (از جمله PASSIVE) که باشد قابل انتخاب باشد
            Point2D pinPos = comp->getPinWorldPosition(static_cast<int>(i));
            float dx = worldPos.x - pinPos.x;
            float dy = worldPos.y - pinPos.y;
            if (std::sqrt(dx * dx + dy * dy) <= threshold) {
                outComp = comp.get();
                outPinIdx = static_cast<int>(i);
                return true;
            }
        }
    }
    return false;
}

Component* CircuitManager::findComponentAt(const Point2D& worldPos) {
    for (auto& comp : components) {
        if (comp->isInside(worldPos)) return comp.get();
    }
    return nullptr;
}

Wire* CircuitManager::findWireAt(const Point2D& worldPos) {
    for (auto& wire : wires) {
        if (wire.isPointNearWire(worldPos)) return &wire;
    }
    return nullptr;
}

std::string CircuitManager::inspectProbeAt(const Point2D& worldPos, float zoom) {
    Component* comp = nullptr;
    int pinIdx = -1;
    if (findPinAt(worldPos, comp, pinIdx, zoom)) {
        float v = comp->getPins()[pinIdx].voltage;
        return "[PROBE]: " + std::to_string(v).substr(0, 4) + " V";
    }
    return "";
}

std::string CircuitManager::generateUniqueId(ComponentType type) {
    static int idCounter = 0;
    std::string prefix = "U";
    switch (type) {
        case ComponentType::SWITCH:         prefix = "SW"; break;
        case ComponentType::CLOCK:          prefix = "CLK"; break;
        case ComponentType::CONST_0:        prefix = "GND_LOGIC"; break;
        case ComponentType::CONST_1:        prefix = "VCC_LOGIC"; break;
        case ComponentType::RESISTOR:       prefix = "R"; break;
        case ComponentType::CAPACITOR:      prefix = "C"; break;
        case ComponentType::INDUCTOR:       prefix = "L"; break;
        case ComponentType::DIODE:          prefix = "D"; break;
        case ComponentType::DIODE_ZENER:    prefix = "DZ"; break;
        case ComponentType::LED:            prefix = "LED"; break;
        case ComponentType::VOLTMETER:      prefix = "VM"; break;
        case ComponentType::AMPEREMETER:    prefix = "AM"; break;
        case ComponentType::GROUND:         prefix = "GND"; break;
        case ComponentType::VOLTAGE_SOURCE: prefix = "VS"; break;
        case ComponentType::BATTERY:        prefix = "BAT"; break;
        default:                            prefix = "U"; break;
    }
    return prefix + std::to_string(++idCounter);
}

void CircuitManager::saveStateHistory() {
    std::ostringstream oss;
    for (const auto& c : components) {
        oss << static_cast<int>(c->getType()) << " " << c->getId() << " " << c->getPosition().x << " " << c->getPosition().y;

        if (c->getType() == ComponentType::RESISTOR) {
            oss << " " << dynamic_cast<ResistorComponent*>(c.get())->getResistance();
        } else if (c->getType() == ComponentType::CAPACITOR) {
            oss << " " << dynamic_cast<CapacitorComponent*>(c.get())->getCapacitance();
        } else if (c->getType() == ComponentType::INDUCTOR) {
            oss << " " << dynamic_cast<InductorComponent*>(c.get())->getInductance();
        } else if (c->getType() == ComponentType::VOLTAGE_SOURCE) {
            oss << " " << dynamic_cast<VoltageSourceComponent*>(c.get())->getVoltage();
        } else if (c->getType() == ComponentType::BATTERY) {
            oss << " " << dynamic_cast<BatteryComponent*>(c.get())->getVoltage();
        } else if (c->getType() == ComponentType::CLOCK) {
            oss << " " << dynamic_cast<ClockComponent*>(c.get())->getFrequency();
        } else if (c->getType() == ComponentType::LED) {
            oss << " " << static_cast<int>(dynamic_cast<LEDComponent*>(c.get())->getActiveColor());
        } else if (c->getType() == ComponentType::DIODE_ZENER) {
            oss << " " << dynamic_cast<ZenerDiodeComponent*>(c.get())->getVZener();
        }

        oss << " \"" << c->getValueLabel() << "\"\n";
    }
    oss << "WIRES\n";
    for (const auto& w : wires) {
        oss << w.serialize() << "\n";
    }

    if (historyIndex < static_cast<int>(historyStates.size()) - 1) {
        historyStates.erase(historyStates.begin() + historyIndex + 1, historyStates.end());
    }
    historyStates.push_back(oss.str());
    historyIndex = static_cast<int>(historyStates.size()) - 1;
}

void CircuitManager::loadFromMemoryStream(const std::string& stateStr) {
    components.clear();
    wires.clear();

    std::stringstream file(stateStr);
    std::string line;
    bool loadingWires = false;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        if (line == "WIRES") {
            loadingWires = true;
            continue;
        }

        std::stringstream ss(line);

        if (!loadingWires) {
            int typeInt;
            std::string id;
            float x, y;

            if (ss >> typeInt >> id >> x >> y) {
                ComponentType type = static_cast<ComponentType>(typeInt);
                auto newComp = ComponentFactory::createComponent(type, id, {x, y});
                if (newComp) {
                    if (type == ComponentType::RESISTOR) {
                        float r; if (ss >> r) dynamic_cast<ResistorComponent*>(newComp.get())->setResistance(r);
                    } else if (type == ComponentType::CAPACITOR) {
                        float c; if (ss >> c) dynamic_cast<CapacitorComponent*>(newComp.get())->setCapacitance(c);
                    } else if (type == ComponentType::INDUCTOR) {
                        float l; if (ss >> l) dynamic_cast<InductorComponent*>(newComp.get())->setInductance(l);
                    } else if (type == ComponentType::VOLTAGE_SOURCE) {
                        float v; if (ss >> v) dynamic_cast<VoltageSourceComponent*>(newComp.get())->setVoltage(v);
                    } else if (type == ComponentType::BATTERY) {
                        float v; if (ss >> v) dynamic_cast<BatteryComponent*>(newComp.get())->setVoltage(v);
                    } else if (type == ComponentType::CLOCK) {
                        float f; if (ss >> f) dynamic_cast<ClockComponent*>(newComp.get())->setFrequency(f);
                    } else if (type == ComponentType::LED) {
                        int col; if (ss >> col) dynamic_cast<LEDComponent*>(newComp.get())->setActiveColor(static_cast<LEDComponent::LEDColor>(col));
                    } else if (type == ComponentType::DIODE_ZENER) {
                        float vz; if (ss >> vz) dynamic_cast<ZenerDiodeComponent*>(newComp.get())->setVZener(vz);
                    }

                    std::string label;
                    if (ss >> std::quoted(label)) {
                        newComp->setValueLabel(label);
                    }

                    components.push_back(std::move(newComp));
                }
            }
        } else {
            std::string srcId, destId;
            int srcPinIdx, destPinIdx;

            if (ss >> srcId >> srcPinIdx >> destId >> destPinIdx) {
                Component* srcComp = nullptr;
                Component* destComp = nullptr;

                for (const auto& comp : components) {
                    if (comp->getId() == srcId) srcComp = comp.get();
                    if (comp->getId() == destId) destComp = comp.get();
                    if (srcComp && destComp) break;
                }

                if (srcComp && destComp) {
                    wires.emplace_back(srcComp, srcPinIdx, destComp, destPinIdx);
                }
            }
        }
    }
    updateLogic();
}

void CircuitManager::undo() {
    if (historyIndex > 0) {
        historyIndex--;
        loadFromMemoryStream(historyStates[historyIndex]);
    }
}

void CircuitManager::redo() {
    if (historyIndex < static_cast<int>(historyStates.size()) - 1) {
        historyIndex++;
        loadFromMemoryStream(historyStates[historyIndex]);
    }
}

void CircuitManager::saveToFile(const std::string& filename) {
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (file.is_open()) {
        if (!historyStates.empty() && historyIndex >= 0) {
            std::string stateData = historyStates[historyIndex];
            file.write(stateData.c_str(), stateData.size());
        }
        file.close();
    } else {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
    }
}

void CircuitManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for reading: " << filename << std::endl;
        return;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    file.close();

    loadFromMemoryStream(ss.str());
    historyStates.clear();
    saveStateHistory();
}

void CircuitManager::exportScreenshot(SDL_Renderer* renderer, const std::string& filename, bool isA3) {
    int width = isA3 ? 4960 : 3508;
    int height = isA3 ? 3508 : 2480;

    SDL_Texture* targetTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                                   SDL_TEXTUREACCESS_TARGET, width, height);
    if (!targetTexture) return;

    SDL_SetRenderTarget(renderer, targetTexture);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderTarget(renderer, NULL);

    SDL_Surface* sshot = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    if (sshot) {
        SDL_SetRenderTarget(renderer, targetTexture);
        SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_RGBA32, sshot->pixels, sshot->pitch);
        SDL_SetRenderTarget(renderer, NULL);

        SDL_SaveBMP(sshot, filename.c_str());
        SDL_FreeSurface(sshot);
    }

    SDL_DestroyTexture(targetTexture);
}

bool CircuitManager::hasShortCircuit() {
    // بررسی اتصال مستقیم دو پین خروجی با سطوح ولتاژ متفاوت به یک گره مشترک
    for (size_t i = 0; i < wires.size(); ++i) {
        for (size_t j = i + 1; j < wires.size(); ++j) {
            auto& w1 = wires[i];
            auto& w2 = wires[j];

            // اگر دو سیم به یک پینِ ورودیِ مشترک وصل شده باشند ولی منابعشان متفاوت باشد (مثل اتصال مستقیم VCC و GND به یک پین)
            if (w1.getEndComponent() == w2.getEndComponent() && w1.getEndPinIndex() == w2.getEndPinIndex()) {
                auto pin1Type = w1.getStartComponent()->getPins()[w1.getStartPinIndex()].type;
                auto pin2Type = w2.getStartComponent()->getPins()[w2.getStartPinIndex()].type;
                if (pin1Type == PinType::OUTPUT && pin2Type == PinType::OUTPUT) {
                    float v1 = w1.getStartComponent()->getPins()[w1.getStartPinIndex()].voltage;
                    float v2 = w2.getStartComponent()->getPins()[w2.getStartPinIndex()].voltage;
                    if (std::abs(v1 - v2) > 2.0f) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool CircuitManager::hasFloatingPins() {
    for (const auto& comp : components) {
        ComponentType type = comp->getType();
        // قطعات زیر از بررسی سخت‌گیرانه پین شناور معاف هستند
        if (type == ComponentType::RESISTOR || type == ComponentType::CAPACITOR ||
            type == ComponentType::INDUCTOR || type == ComponentType::VOLTMETER ||
            type == ComponentType::AMPEREMETER || type == ComponentType::GROUND ||
            type == ComponentType::DIODE || type == ComponentType::DIODE_ZENER) {
            continue;
        }

        const auto& pins = comp->getPins();
        for (size_t i = 0; i < pins.size(); ++i) {
            if (pins[i].type == PinType::INPUT) {
                bool connected = false;
                for (const auto& wire : wires) {
                    if ((wire.getEndComponent() == comp.get() && wire.getEndPinIndex() == static_cast<int>(i)) ||
                        (wire.getStartComponent() == comp.get() && wire.getStartPinIndex() == static_cast<int>(i))) {
                        connected = true;
                        break;
                    }
                }
                if (!connected) return true;
            }
        }
    }
    return false;
}