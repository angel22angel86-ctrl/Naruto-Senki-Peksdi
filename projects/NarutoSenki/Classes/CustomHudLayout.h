#pragma once

#include "cocos2d.h"

class HudLayer;

// Elements that can be moved/scaled in the Custom HUD editor.
enum class CustomHudElement
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
    Joystick
};

class CustomHudLayout
{
public:
    static cocos2d::Vec2 loadPosition(CustomHudElement id,
                                      const cocos2d::Vec2 &fallback,
                                      const cocos2d::Size &screenSize);
    static float loadScale(CustomHudElement id, float fallback);

    static void save(CustomHudElement id,
                     const cocos2d::Vec2 &position,
                     float scale,
                     const cocos2d::Size &screenSize);

    static void flush();
    static void reset();

    // Applies the saved layout to the real in-game HUD.
    static void apply(HudLayer *hud);
};
