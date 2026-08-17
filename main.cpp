#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include "Types.h"
#include "CircuitManager.h"
#include "ComponentFactory.h"
#include "AdvancedUI.h"
#include "ContextMenu.h"
#include "MainMenu.h"

enum class AppState {
    MENU,
    SIMULATOR
};

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return -1;
    if (TTF_Init() < 0) { SDL_Quit(); return -1; }

    SDL_Window* window = SDL_CreateWindow("Digital Logic Simulator - Proteus",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) return -1;

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    TTF_Font* font = TTF_OpenFont("arial.ttf", 13);
    if (!font) font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 13);

    AppState appState = AppState::MENU; // شروع برنامه از منوی آغازین
    MainMenu mainMenu;
    CircuitManager circuit;
    AdvancedSidebar sidebar;
    ContextMenu contextMenu;

    SimState simState = SimState::STOPPED;
    Point2D cameraOffset = {0.0f, 0.0f};
    float zoom = 1.0f;

    bool isPlacingComponent = false;
    ComponentType selectedTypeToPlace = ComponentType::GATE_AND;
    bool isWiring = false;
    Component* wireSrcComp = nullptr;
    int wireSrcPinIdx = -1;
    bool isDraggingComp = false;
    Component* draggedComp = nullptr;
    Point2D dragOffset = {0.0f, 0.0f};

    std::string probeText = "";
    std::vector<std::string> logMessages;
    bool showTerminal = true;
    bool showInputBox = false;
    std::string inputBuffer = "";
    std::string inputPromptTitle = "";
    Component* targetPropertyComp = nullptr;

    Uint32 lastClickTime = 0;
    Component* lastClickedComp = nullptr;

    SDL_StartTextInput();
    bool running = true;
    SDL_Event event;

    while (running) {
        int winWidth, winHeight;
        SDL_GetWindowSize(window, &winWidth, &winHeight);

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_MOUSEWHEEL:
                    if (appState == AppState::SIMULATOR && !showInputBox) {
                        if (event.wheel.y > 0) sidebar.navigateUp();
                        else if (event.wheel.y < 0) sidebar.navigateDown();
                    }
                    break;
                case SDL_TEXTINPUT:
                    if (appState == AppState::SIMULATOR && showInputBox) {
                        for (char c : event.text.text) {
                            if (c == '\0') break;
                            if ((c >= '0' && c <= '9') || c == '.' || c == '-' || c == 'e' || c == 'E') {
                                inputBuffer += c;
                            }
                        }
                    }
                    break;
                case SDL_KEYDOWN:
                    if (appState == AppState::SIMULATOR) {
                        if (showInputBox) {
                            if (event.key.keysym.sym == SDLK_BACKSPACE && !inputBuffer.empty()) {
                                inputBuffer.pop_back();
                            }
                            else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                                if (targetPropertyComp && !inputBuffer.empty()) {
                                    float val = std::stof(inputBuffer);
                                    bool updated = false;

                                    if (targetPropertyComp->getType() == ComponentType::RESISTOR) {
                                        auto* comp = dynamic_cast<ResistorComponent*>(targetPropertyComp);
                                        comp->setResistance(val);
                                        comp->setValueLabel(inputBuffer + " Ohm");
                                        updated = true;
                                        logMessages.push_back("Resistor " + targetPropertyComp->getId() + " set to " + inputBuffer + " Ohm.");
                                    }
                                    else if (targetPropertyComp->getType() == ComponentType::CAPACITOR) {
                                        auto* comp = dynamic_cast<CapacitorComponent*>(targetPropertyComp);
                                        comp->setCapacitance(val);
                                        comp->setValueLabel(inputBuffer + " F");
                                        updated = true;
                                        logMessages.push_back("Capacitor " + targetPropertyComp->getId() + " set to " + inputBuffer + " F.");
                                    }
                                    else if (targetPropertyComp->getType() == ComponentType::INDUCTOR) {
                                        auto* comp = dynamic_cast<InductorComponent*>(targetPropertyComp);
                                        comp->setInductance(val);
                                        comp->setValueLabel(inputBuffer + " H");
                                        updated = true;
                                        logMessages.push_back("Inductor " + targetPropertyComp->getId() + " set to " + inputBuffer + " H.");
                                    }
                                    else if (targetPropertyComp->getType() == ComponentType::VOLTAGE_SOURCE) {
                                        auto* comp = dynamic_cast<VoltageSourceComponent*>(targetPropertyComp);
                                        comp->setVoltage(val);
                                        comp->setValueLabel(inputBuffer + " V");
                                        updated = true;
                                        logMessages.push_back("Voltage Source " + targetPropertyComp->getId() + " set to " + inputBuffer + " V.");
                                    }
                                    else if (targetPropertyComp->getType() == ComponentType::BATTERY) {
                                        auto* comp = dynamic_cast<BatteryComponent*>(targetPropertyComp);
                                        comp->setVoltage(val);
                                        comp->setValueLabel(inputBuffer + " V");
                                        updated = true;
                                        logMessages.push_back("Battery " + targetPropertyComp->getId() + " set to " + inputBuffer + " V.");
                                    }
                                    if (updated) {
                                        circuit.saveStateHistory();
                                        circuit.updateLogic();
                                    }
                                }
                                showInputBox = false;
                                inputBuffer.clear();
                                targetPropertyComp = nullptr;
                            }
                            else if (event.key.keysym.sym == SDLK_ESCAPE) {
                                showInputBox = false;
                                inputBuffer.clear();
                                targetPropertyComp = nullptr;
                            }
                        }
                        else {
                            if (event.key.keysym.sym == SDLK_UP) sidebar.navigateUp();
                            else if (event.key.keysym.sym == SDLK_DOWN) sidebar.navigateDown();
                            else if (event.key.keysym.sym == SDLK_SPACE) {
                                auto activeType = sidebar.getSelectedType();
                                if (activeType) {
                                    selectedTypeToPlace = *activeType;
                                    isPlacingComponent = true;
                                    logMessages.push_back("Placement mode activated.");
                                }
                            }
                            else if (event.key.keysym.sym == SDLK_F5) {
                                logMessages.clear();
                                bool hasError = false;
                                if (circuit.hasShortCircuit()) {
                                    logMessages.push_back("DRC Error: Short circuit detected!");
                                    hasError = true;
                                }
                                if (circuit.hasFloatingPins()) {
                                    logMessages.push_back("DRC Warning: Floating pin(s) detected.");
                                }
                                if (hasError) {
                                    logMessages.push_back("Error: Critical error found. Simulation aborted.");
                                    simState = SimState::STOPPED;
                                } else {
                                    logMessages.push_back("Report: Circuit is ready for simulation.");
                                    simState = (simState == SimState::RUNNING) ? SimState::PAUSED : SimState::RUNNING;
                                }
                            }
                            else if (event.key.keysym.sym == SDLK_F6) {
                                simState = SimState::STOPPED;
                                logMessages.push_back("Report: Simulation stopped.");
                            }
                            else if (event.key.keysym.sym == SDLK_DELETE || event.key.keysym.sym == SDLK_BACKSPACE) {
                                circuit.deleteSelectedItems();
                                logMessages.push_back("Selected element deleted.");
                            }
                            else if (event.key.keysym.sym == SDLK_r) {
                                Component* target = draggedComp;
                                if (!target) {
                                    for (auto& c : circuit.getComponents()) {
                                        if (c->getSelected()) { target = c.get(); break; }
                                    }
                                }
                                if (target) {
                                    target->rotate();
                                    circuit.saveStateHistory();
                                }
                            }
                            else if (event.key.keysym.sym == SDLK_h) {
                                for (auto& c : circuit.getComponents()) {
                                    if (c->getSelected()) { c->mirrorHorizontal(); break; }
                                }
                            }
                            else if (event.key.keysym.sym == SDLK_v) {
                                for (auto& c : circuit.getComponents()) {
                                    if (c->getSelected()) { c->mirrorVertical(); break; }
                                }
                            }
                            else if (event.key.keysym.mod & KMOD_CTRL) {
                                if (event.key.keysym.sym == SDLK_z) circuit.undo();
                                else if (event.key.keysym.sym == SDLK_y) circuit.redo();
                                else if (event.key.keysym.sym == SDLK_s) circuit.saveToFile("project.dat");
                                else if (event.key.keysym.sym == SDLK_p) circuit.exportScreenshot(renderer, "schematic.bmp");
                            }
                            else if (event.key.keysym.sym == SDLK_ESCAPE) {
                                isWiring = false;
                                wireSrcComp = nullptr;
                                wireSrcPinIdx = -1;
                                isPlacingComponent = false;
                                sidebar.clearSelection();
                                circuit.clearSelections();
                                contextMenu.hide();
                            }
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN: {
                    float mx = static_cast<float>(event.button.x);
                    float my = static_cast<float>(event.button.y);
                    if (appState == AppState::MENU) {
                        MenuResult result = mainMenu.handleEvent(event, winWidth, winHeight);
                        if (result == MenuResult::NEW_PROJECT || result == MenuResult::OPEN_RECENT || result == MenuResult::OPEN_PROJECT) {
                            appState = AppState::SIMULATOR;
                            logMessages.push_back("Project loaded successfully.");
                        } else if (result == MenuResult::EXIT) {
                            running = false;
                        }
                        break;
                    }
                    Point2D worldPos = { (mx / zoom) + cameraOffset.x, (my / zoom) + cameraOffset.y };
                    if (showInputBox) break;
                    if (event.button.button == SDL_BUTTON_RIGHT) {
                        circuit.clearSelections();
                        Component* targetComp = circuit.findComponentAt(worldPos);
                        if (targetComp) targetComp->setSelected(true);
                        contextMenu.show(mx, my);
                        break;
                    }
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (contextMenu.isActive()) {
                            int actionId = contextMenu.handleClick(mx, my);
                            contextMenu.hide();
                            if (actionId != -1) {
                                Component* selectedComp = nullptr;
                                for (auto& c : circuit.getComponents()) {
                                    if (c->getSelected()) { selectedComp = c.get(); break; }
                                }
                                if (actionId == 1 && selectedComp) {
                                    ComponentType type = selectedComp->getType();
                                    if (type == ComponentType::RESISTOR || type == ComponentType::CAPACITOR ||
                                        type == ComponentType::INDUCTOR || type == ComponentType::VOLTAGE_SOURCE ||
                                        type == ComponentType::BATTERY) {
                                        targetPropertyComp = selectedComp;
                                        showInputBox = true;
                                        inputBuffer = "";
                                        if (type == ComponentType::RESISTOR) inputPromptTitle = "Enter Resistance (Ohms):";
                                        else if (type == ComponentType::CAPACITOR) inputPromptTitle = "Enter Capacitance (Farads):";
                                        else if (type == ComponentType::INDUCTOR) inputPromptTitle = "Enter Inductance (Henrys):";
                                        else inputPromptTitle = "Enter Voltage (Volts):";
                                    } else {
                                        logMessages.push_back("Properties: Component has no editable numeric value.");
                                    }
                                }
                                else if (actionId == 2) {
                                    for (auto& c : circuit.getComponents()) {
                                        if (c->getSelected()) { c->rotate(); circuit.saveStateHistory(); break; }
                                    }
                                }
                                else if (actionId == 3) {
                                    for (auto& c : circuit.getComponents()) {
                                        if (c->getSelected()) { c->mirrorHorizontal(); break; }
                                    }
                                }
                                else if (actionId == 4) {
                                    for (auto& c : circuit.getComponents()) {
                                        if (c->getSelected()) { c->mirrorVertical(); break; }
                                    }
                                }
                                else if (actionId == 5) {
                                    circuit.deleteSelectedItems();
                                    logMessages.push_back("Selected element deleted.");
                                }
                                break;
                            }
                        }
                        if (mx <= sidebar.getWidth()) {
                            ComponentType* clickedType = sidebar.handleClick(mx, my);
                            if (clickedType) {
                                selectedTypeToPlace = *clickedType;
                                isPlacingComponent = true;
                                logMessages.push_back("Placement mode activated from sidebar.");
                            }
                            isWiring = false;
                            wireSrcComp = nullptr;
                        }
                        else {
                            circuit.clearSelections();
                            if (isPlacingComponent) {
                                std::string newId = circuit.generateUniqueId(selectedTypeToPlace);
                                std::unique_ptr<Component> newComp = nullptr;
                                switch (selectedTypeToPlace) {
                                    case ComponentType::GATE_AND:       newComp = std::make_unique<GateAND>(newId, worldPos); break;
                                    case ComponentType::GATE_OR:        newComp = std::make_unique<GateOR>(newId, worldPos); break;
                                    case ComponentType::GATE_NOT:       newComp = std::make_unique<GateNOT>(newId, worldPos); break;
                                    case ComponentType::GATE_NAND:      newComp = std::make_unique<GateNAND>(newId, worldPos); break;
                                    case ComponentType::GATE_NOR:       newComp = std::make_unique<GateNOR>(newId, worldPos); break;
                                    case ComponentType::GATE_XOR:       newComp = std::make_unique<GateXOR>(newId, worldPos); break;
                                    case ComponentType::GATE_XNOR:      newComp = std::make_unique<GateXNOR>(newId, worldPos); break;
                                    case ComponentType::SWITCH:         newComp = std::make_unique<SwitchComponent>(newId, worldPos); break;
                                    case ComponentType::CONST_0:        newComp = std::make_unique<Constant0Component>(newId, worldPos); break;
                                    case ComponentType::CONST_1:        newComp = std::make_unique<Constant1Component>(newId, worldPos); break;
                                    case ComponentType::CLOCK:          newComp = std::make_unique<ClockComponent>(newId, worldPos); break;
                                    case ComponentType::GROUND:         newComp = std::make_unique<GroundComponent>(newId, worldPos); break;
                                    case ComponentType::RESISTOR:       newComp = std::make_unique<ResistorComponent>(newId, worldPos); break;
                                    case ComponentType::CAPACITOR:      newComp = std::make_unique<CapacitorComponent>(newId, worldPos); break;
                                    case ComponentType::INDUCTOR:       newComp = std::make_unique<InductorComponent>(newId, worldPos); break;
                                    case ComponentType::DIODE:          newComp = std::make_unique<DiodeComponent>(newId, worldPos); break;
                                    case ComponentType::DIODE_ZENER:    newComp = std::make_unique<ZenerDiodeComponent>(newId, worldPos); break;
                                    case ComponentType::VOLTAGE_SOURCE: newComp = std::make_unique<VoltageSourceComponent>(newId, worldPos); break;
                                    case ComponentType::BATTERY:        newComp = std::make_unique<BatteryComponent>(newId, worldPos); break;
                                    case ComponentType::LED:            newComp = std::make_unique<LEDComponent>(newId, worldPos); break;
                                    case ComponentType::VOLTMETER:      newComp = std::make_unique<VoltmeterComponent>(newId, worldPos); break;
                                    case ComponentType::AMPEREMETER:    newComp = std::make_unique<AmperemeterComponent>(newId, worldPos); break;
                                    default: break;
                                }
                                if (newComp) {
                                    newComp->snapToGrid();
                                    circuit.addComponent(std::move(newComp));
                                    logMessages.push_back("Component " + newId + " added.");
                                }
                                isPlacingComponent = false;
                                sidebar.clearSelection();
                            }
                            else {
                                Component* hitComp = nullptr;
                                int hitPinIdx = -1;
                                // بررسی کلیک روی پین بدون محدودیت نوع پین (پشتیبانی کامل از قطعات پسیو و آنالوگ)
                                if (circuit.findPinAt(worldPos, hitComp, hitPinIdx, zoom)) {
                                    if (!isWiring) {
                                        isWiring = true;
                                        wireSrcComp = hitComp;
                                        wireSrcPinIdx = hitPinIdx;
                                        logMessages.push_back("Started wiring from pin.");
                                    } else {
                                        if (wireSrcComp != hitComp || wireSrcPinIdx != hitPinIdx) {
                                            circuit.addWire(wireSrcComp, wireSrcPinIdx, hitComp, hitPinIdx);
                                            logMessages.push_back("Wiring connected successfully.");
                                        }
                                        isWiring = false;
                                        wireSrcComp = nullptr;
                                        wireSrcPinIdx = -1;
                                    }
                                }
                                else {
                                    Component* targetComp = circuit.findComponentAt(worldPos);
                                    if (targetComp) {
                                        Uint32 currentTime = SDL_GetTicks();
                                        if (targetComp == lastClickedComp && (currentTime - lastClickTime < 350)) {
                                            targetComp->setSelected(true);
                                            ComponentType type = targetComp->getType();
                                            if (type == ComponentType::RESISTOR || type == ComponentType::CAPACITOR ||
                                                type == ComponentType::INDUCTOR || type == ComponentType::VOLTAGE_SOURCE ||
                                                type == ComponentType::BATTERY) {
                                                targetPropertyComp = targetComp;
                                                showInputBox = true;
                                                inputBuffer = "";
                                                if (type == ComponentType::RESISTOR) inputPromptTitle = "Enter Resistance (Ohms):";
                                                else if (type == ComponentType::CAPACITOR) inputPromptTitle = "Enter Capacitance (Farads):";
                                                else if (type == ComponentType::INDUCTOR) inputPromptTitle = "Enter Inductance (Henrys):";
                                                else inputPromptTitle = "Enter Voltage (Volts):";
                                            }
                                        }
                                        lastClickTime = currentTime;
                                        lastClickedComp = targetComp;
                                        targetComp->setSelected(true);
                                        if (targetComp->getType() == ComponentType::SWITCH) {
                                            static_cast<SwitchComponent*>(targetComp)->toggle();
                                            circuit.updateLogic();
                                        } else {
                                            isDraggingComp = true;
                                            draggedComp = targetComp;
                                            dragOffset = worldPos - targetComp->getPosition();
                                        }
                                    } else {
                                        Wire* hitWire = circuit.findWireAt(worldPos);
                                        if (hitWire) hitWire->setSelected(true);
                                        isWiring = false;
                                        wireSrcComp = nullptr;
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                    if (appState == AppState::SIMULATOR && event.button.button == SDL_BUTTON_LEFT) {
                        if (isDraggingComp && draggedComp) draggedComp->snapToGrid();
                        isDraggingComp = false;
                        draggedComp = nullptr;
                    }
                    break;
                case SDL_MOUSEMOTION: {
                    if (appState == AppState::SIMULATOR) {
                        float mx = static_cast<float>(event.motion.x);
                        float my = static_cast<float>(event.motion.y);
                        Point2D worldPos = { (mx / zoom) + cameraOffset.x, (my / zoom) + cameraOffset.y };

                        if (isDraggingComp && draggedComp) {
                            draggedComp->setPosition(worldPos - dragOffset);
                        }
                        probeText = circuit.inspectProbeAt(worldPos, zoom);
                    }
                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 20, 24, 30, 255);
        SDL_RenderClear(renderer);

        if (appState == AppState::MENU) {
            mainMenu.render(renderer, font, winWidth, winHeight);
        } else {
            if (simState == SimState::RUNNING || simState == SimState::PAUSED) {
                circuit.updateLogic();
            }

            circuit.render(renderer, font, cameraOffset, zoom, simState);

            if (isWiring && wireSrcComp) {
                int mx, my;
                SDL_GetMouseState(&mx, &my);
                Point2D p1 = wireSrcComp->getPinWorldPosition(wireSrcPinIdx);
                Point2D s1 = (p1 - cameraOffset) * zoom;
                float smx = static_cast<float>(mx);
                float smy = static_cast<float>(my);
                SDL_SetRenderDrawColor(renderer, 255, 255, 100, 255);
                SDL_RenderDrawLineF(renderer, s1.x, s1.y, (s1.x + smx) / 2.0f, s1.y);
                SDL_RenderDrawLineF(renderer, (s1.x + smx) / 2.0f, s1.y, (s1.x + smx) / 2.0f, smy);
                SDL_RenderDrawLineF(renderer, (s1.x + smx) / 2.0f, smy, smx, smy);
            }

            sidebar.render(renderer, simState);

            if (showTerminal) {
                SDL_SetRenderDrawColor(renderer, 15, 18, 22, 240);
                SDL_FRect termRect = { 200, (float)(winHeight - 140), (float)(winWidth - 200), 140 };
                SDL_RenderFillRectF(renderer, &termRect);
                SDL_SetRenderDrawColor(renderer, 60, 70, 90, 255);
                SDL_RenderDrawRectF(renderer, &termRect);
                if (font) {
                    SDL_Surface* tSurf = TTF_RenderText_Blended(font, "Simulation Log & DRC Terminal:", {100, 200, 255, 255});
                    if (tSurf) {
                        SDL_Texture* tTex = SDL_CreateTextureFromSurface(renderer, tSurf);
                        SDL_FRect tDr = {210, (float)(winHeight - 130), (float)tSurf->w, (float)tSurf->h};
                        SDL_RenderCopyF(renderer, tTex, NULL, &tDr);
                        SDL_DestroyTexture(tTex);
                        SDL_FreeSurface(tSurf);
                    }
                    int startY = winHeight - 105;
                    int maxLines = 5;
                    int startIndex = (logMessages.size() > maxLines) ? logMessages.size() - maxLines : 0;
                    for (size_t i = startIndex; i < logMessages.size(); ++i) {
                        SDL_Surface* lSurf = TTF_RenderText_Blended(font, logMessages[i].c_str(), {220, 220, 220, 255});
                        if (lSurf) {
                            SDL_Texture* lTex = SDL_CreateTextureFromSurface(renderer, lSurf);
                            SDL_FRect lDr = {210, (float)(startY + (i - startIndex) * 18), (float)lSurf->w, (float)lSurf->h};
                            SDL_RenderCopyF(renderer, lTex, NULL, &lDr);
                            SDL_DestroyTexture(lTex);
                            SDL_FreeSurface(lSurf);
                        }
                    }
                }
            }

            if (showInputBox) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 220);
                SDL_FRect overlay = {0, 0, (float)winWidth, (float)winHeight};
                SDL_RenderFillRectF(renderer, &overlay);
                SDL_SetRenderDrawColor(renderer, 35, 45, 60, 255);
                SDL_FRect boxRect = {(float)(winWidth/2 - 200), (float)(winHeight/2 - 70), 400, 140};
                SDL_RenderFillRectF(renderer, &boxRect);
                SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
                SDL_RenderDrawRectF(renderer, &boxRect);
                if (font) {
                    SDL_Surface* titleSurf = TTF_RenderText_Blended(font, inputPromptTitle.c_str(), {255, 255, 255, 255});
                    if (titleSurf) {
                        SDL_Texture* tTex = SDL_CreateTextureFromSurface(renderer, titleSurf);
                        SDL_FRect tDr = {(float)(winWidth/2 - 180), (float)(winHeight/2 - 50), (float)titleSurf->w, (float)titleSurf->h};
                        SDL_RenderCopyF(renderer, tTex, NULL, &tDr);
                        SDL_DestroyTexture(tTex);
                        SDL_FreeSurface(titleSurf);
                    }
                    SDL_SetRenderDrawColor(renderer, 20, 25, 35, 255);
                    SDL_FRect textInputRect = {(float)(winWidth/2 - 180), (float)(winHeight/2 - 16), 360, 35};
                    SDL_RenderFillRectF(renderer, &textInputRect);
                    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
                    SDL_RenderDrawRectF(renderer, &textInputRect);
                    std::string displayVal = inputBuffer + "_";
                    SDL_Surface* valSurf = TTF_RenderText_Blended(font, displayVal.c_str(), {0, 255, 200, 255});
                    if (valSurf) {
                        SDL_Texture* vTex = SDL_CreateTextureFromSurface(renderer, valSurf);
                        SDL_FRect vDr = {(float)(winWidth/2 - 170), (float)(winHeight/2 - 8), (float)valSurf->w, (float)valSurf->h};
                        SDL_RenderCopyF(renderer, vTex, NULL, &vDr);
                        SDL_DestroyTexture(vTex);
                        SDL_FreeSurface(valSurf);
                    }
                    std::string hint = "[ENTER]: Save  |  [ESC]: Cancel";
                    SDL_Surface* hintSurf = TTF_RenderText_Blended(font, hint.c_str(), {180, 180, 180, 255});
                    if (hintSurf) {
                        SDL_Texture* hTex = SDL_CreateTextureFromSurface(renderer, hintSurf);
                        SDL_FRect hDr = {(float)(winWidth/2 - 180), (float)(winHeight/2 + 25), (float)hintSurf->w, (float)hintSurf->h};
                        SDL_RenderCopyF(renderer, hTex, NULL, &hDr);
                        SDL_DestroyTexture(hTex);
                        SDL_FreeSurface(hintSurf);
                    }
                }
            }

            if (!probeText.empty()) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
                SDL_FRect pRect = { 230, 540, 200, 30 };
                SDL_RenderFillRectF(renderer, &pRect);
                if (font) {
                    SDL_Surface* surf = TTF_RenderText_Blended(font, probeText.c_str(), {0, 255, 200, 255});
                    if (surf) {
                        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
                        SDL_FRect dR = {240, 545, (float)surf->w, (float)surf->h};
                        SDL_RenderCopyF(renderer, tex, NULL, &dR);
                        SDL_DestroyTexture(tex);
                        SDL_FreeSurface(surf);
                    }
                }
            }

            contextMenu.render(renderer, font);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_StopTextInput();
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}