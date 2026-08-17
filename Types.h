#ifndef TYPES_H
#define TYPES_H

#include <string>

enum class Orientation {
    DEG_0,
    DEG_90,
    DEG_180,
    DEG_270
};

enum class ComponentType {
    // گیت‌های منطقی
    GATE_AND,
    GATE_OR,
    GATE_NOT,
    GATE_NAND,
    GATE_NOR,
    GATE_XOR,
    GATE_XNOR,

    // ورودی‌ها و سیگنال‌ها
    SWITCH,
    CONST_0,
    CONST_1,
    CLOCK,

    // خروجی‌ها و ابزارهای اندازه‌گیری
    LED,
    VOLTMETER,
    AMPEREMETER,

    // قطعات آنالوگ و منابع
    RESISTOR,
    CAPACITOR,
    INDUCTOR,
    DIODE,
    DIODE_ZENER,
    GROUND,
    VOLTAGE_SOURCE,
    BATTERY,

    // قطعات فعال پیشرفته
    TRANSISTOR_NPN,
    MOSFET_N,
    OPAMP
};

enum class PinType {
    INPUT,
    OUTPUT,
    PASSIVE // جهت پشتیبانی از قطعات دوطرفه آنالوگ
};

enum class SimState {
    STOPPED,
    RUNNING,
    PAUSED
};

enum class LogicState {
    LOW_STATE,
    HIGH_STATE,
    UNDEFINED_STATE
};

struct Point2D {
    float x;
    float y;

    Point2D operator+(const Point2D& other) const { return {x + other.x, y + other.y}; }
    Point2D operator-(const Point2D& other) const { return {x - other.x, y - other.y}; }
    Point2D operator*(float scalar) const { return {x * scalar, y * scalar}; }
};

struct Pin {
    int id;
    std::string name;
    Point2D relativePos;
    PinType type;
    LogicState state = LogicState::UNDEFINED_STATE;
    float voltage = 0.0f;
    float current = 0.0f; // جریان تعبیه شده برای شبیه‌سازی دقیق
};

#endif // TYPES_H