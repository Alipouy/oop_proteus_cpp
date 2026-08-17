#include "ComponentFactory.h"
#include "Component.h" // شامل تمام هدرهای قطعات و گیت‌ها

std::unique_ptr<Component> ComponentFactory::createComponent(ComponentType type, const std::string& id, Point2D pos) {
    switch (type) {
        // گیت‌های منطقی
        case ComponentType::GATE_AND:
            return std::make_unique<GateAND>(id, pos);
        case ComponentType::GATE_OR:
            return std::make_unique<GateOR>(id, pos);
        case ComponentType::GATE_NOT:
            return std::make_unique<GateNOT>(id, pos);
        case ComponentType::GATE_NAND:
            return std::make_unique<GateNAND>(id, pos);
        case ComponentType::GATE_NOR:
            return std::make_unique<GateNOR>(id, pos);
        case ComponentType::GATE_XOR:
            return std::make_unique<GateXOR>(id, pos);
        case ComponentType::GATE_XNOR:
            return std::make_unique<GateXNOR>(id, pos);

            // ورودی‌ها و سیگنال‌ها
        case ComponentType::SWITCH:
            return std::make_unique<SwitchComponent>(id, pos);
        case ComponentType::CONST_0:
            return std::make_unique<Constant0Component>(id, pos);
        case ComponentType::CONST_1:
            return std::make_unique<Constant1Component>(id, pos);
        case ComponentType::CLOCK:
            return std::make_unique<ClockComponent>(id, pos);

            // خروجی‌ها و ابزارها
        case ComponentType::LED:
            return std::make_unique<LEDComponent>(id, pos);
        case ComponentType::VOLTMETER:
            return std::make_unique<VoltmeterComponent>(id, pos);
        case ComponentType::AMPEREMETER:
            return std::make_unique<AmperemeterComponent>(id, pos);

            // قطعات آنالوگ
        case ComponentType::GROUND:
            return std::make_unique<GroundComponent>(id, pos);
        case ComponentType::RESISTOR:
            return std::make_unique<ResistorComponent>(id, pos);
        case ComponentType::CAPACITOR:
            return std::make_unique<CapacitorComponent>(id, pos);
        case ComponentType::INDUCTOR:
            return std::make_unique<InductorComponent>(id, pos);
        case ComponentType::DIODE:
            return std::make_unique<DiodeComponent>(id, pos);
        case ComponentType::DIODE_ZENER:
            return std::make_unique<ZenerDiodeComponent>(id, pos);
        case ComponentType::VOLTAGE_SOURCE:
            return std::make_unique<VoltageSourceComponent>(id, pos);
        case ComponentType::BATTERY:
            return std::make_unique<BatteryComponent>(id, pos);

            // قطعات فعال پیشرفته
        case ComponentType::TRANSISTOR_NPN:
            return std::make_unique<BJTNPNComponent>(id, pos);
        case ComponentType::OPAMP:
            return std::make_unique<OpAmpComponent>(id, pos);
        case ComponentType::MOSFET_N:
            return std::make_unique<MOSFETNChanComponent>(id, pos);

        default:
            return nullptr;
    }
}