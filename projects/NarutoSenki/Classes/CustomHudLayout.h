#pragma once

#include "Defines.h"

// IDs used by both the Custom HUD editor and the real gameplay HUD.
// Do not change the numeric order: it is part of the UserDefault key format.
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
    // Keep all persistence keys in one place.  The key is built as a std::string
    // first, so no pointer to a temporary std::string is ever retained.
    static inline std::string key(CustomHudElement element, const char *suffix)
    {
        return std::string("customHud_") +
               std::to_string(static_cast<int>(element)) +
               "_" + suffix;
    }

    static inline float normalized(float value, float size)
    {
        return size > 0.0f ? value / size : 0.0f;
    }

    static inline Vec2 loadPosition(CustomHudElement element,
                                    const Vec2 &fallback,
                                    const CCSize &screen)
    {
        UserDefault *prefs = UserDefault::sharedUserDefault();

        const float fallbackX = normalized(fallback.x, screen.width);
        const float fallbackY = normalized(fallback.y, screen.height);

        const std::string xKey = key(element, "x");
        const std::string yKey = key(element, "y");

        const float x = prefs->getFloatForKey(xKey.c_str(), fallbackX);
        const float y = prefs->getFloatForKey(yKey.c_str(), fallbackY);

        return Vec2(x * screen.width, y * screen.height);
    }

    static inline float loadScale(CustomHudElement element, float fallback)
    {
        UserDefault *prefs = UserDefault::sharedUserDefault();
        const std::string scaleKey = key(element, "scale");
        return prefs->getFloatForKey(scaleKey.c_str(), fallback);
    }

    static inline void save(CustomHudElement element,
                            const Vec2 &position,
                            float scale,
                            const CCSize &screen)
    {
        UserDefault *prefs = UserDefault::sharedUserDefault();

        const std::string xKey = key(element, "x");
        const std::string yKey = key(element, "y");
        const std::string scaleKey = key(element, "scale");

        prefs->setFloatForKey(xKey.c_str(), normalized(position.x, screen.width));
        prefs->setFloatForKey(yKey.c_str(), normalized(position.y, screen.height));
        prefs->setFloatForKey(scaleKey.c_str(), scale);
    }

    static inline void reset(CustomHudElement element)
    {
        UserDefault *prefs = UserDefault::sharedUserDefault();

        const std::string xKey = key(element, "x");
        const std::string yKey = key(element, "y");
        const std::string scaleKey = key(element, "scale");

        prefs->deleteValueForKey(xKey.c_str());
        prefs->deleteValueForKey(yKey.c_str());
        prefs->deleteValueForKey(scaleKey.c_str());
    }

    static inline void flush()
    {
        UserDefault::sharedUserDefault()->flush();
    }
}
