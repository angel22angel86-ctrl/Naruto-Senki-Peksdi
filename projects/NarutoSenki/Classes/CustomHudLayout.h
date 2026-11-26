#pragma once

#include "Defines.h"

class HudLayer;

// Stable IDs for every HUD control that can be edited from the Network menu.
// Keep the numeric order stable because it is also used in the preference keys.
enum class CustomHudElement : uint8_t
{
    Attack = 0,
    Skill1,
    Skill2,
    Skill3,
    Skill4,
    Skill5,
    Ramen,
    Item2,
    Item3,
    Item4,
    Joystick,
    Count
};

class CustomHudLayout
{
public:
    // Load/save uses normalized screen coordinates so a HUD saved on one
    // Android resolution remains in the same relative place on another.
    static Vec2 loadPosition(CustomHudElement id, const Vec2 &fallback, const Size &screenSize);
    static float loadScale(CustomHudElement id, float fallback);
    static void save(CustomHudElement id, const Vec2 &position, float scale, const Size &screenSize);
    static void reset();
    static void flush();

    // Applies the saved layout to the real gameplay HUD. This is intentionally
    // done after the original HUD has been created so every character keeps
    // using the game's existing ActionButton objects and input logic.
    static void apply(HudLayer *hud);
};
