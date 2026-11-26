#pragma once

#include "Defines.h"

// Persistent layout shared by the Custom HUD editor and the in-game HUD.
// Positions are stored as normalized screen coordinates so the layout survives
// changes in device resolution/aspect ratio.
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
    Joystick,
    Count
};

namespace CustomHudLayout
{
static inline string makeKey(CustomHudElement id, const char *suffix)
{
    return string("customHud_") + to_cstr(static_cast<int>(id)) + "_" + suffix;
}

static inline Vec2 loadPosition(CustomHudElement id, const Vec2 &fallback, const CCSize &screen)
{
    auto *prefs = UserDefault::sharedUserDefault();
    const float defaultX = screen.width > 0 ? fallback.x / screen.width : 0.0f;
    const float defaultY = screen.height > 0 ? fallback.y / screen.height : 0.0f;
    const float x = prefs->getFloatForKey(makeKey(id, "x").c_str(), defaultX);
    const float y = prefs->getFloatForKey(makeKey(id, "y").c_str(), defaultY);
    return Vec2(x * screen.width, y * screen.height);
}

static inline float loadScale(CustomHudElement id, float fallback)
{
    return UserDefault::sharedUserDefault()->getFloatForKey(makeKey(id, "scale").c_str(), fallback);
}

static inline void save(CustomHudElement id, const Vec2 &position, float scale, const CCSize &screen)
{
    auto *prefs = UserDefault::sharedUserDefault();
    const float x = screen.width > 0 ? position.x / screen.width : 0.0f;
    const float y = screen.height > 0 ? position.y / screen.height : 0.0f;
    prefs->setFloatForKey(makeKey(id, "x").c_str(), x);
    prefs->setFloatForKey(makeKey(id, "y").c_str(), y);
    prefs->setFloatForKey(makeKey(id, "scale").c_str(), scale);
}

static inline void flush()
{
    UserDefault::sharedUserDefault()->flush();
}
}
