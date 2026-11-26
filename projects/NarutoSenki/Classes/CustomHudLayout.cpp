#include "CustomHudLayout.h"
#include "HudLayer.h"
#include "JoyStick.h"
#include "ActionButton.h"
#include <cmath>

using namespace cocos2d;

namespace
{
    const char *keyFor(CustomHudElement id)
    {
        switch (id)
        {
        case CustomHudElement::Attack:   return "attack";
        case CustomHudElement::Skill1:   return "skill1";
        case CustomHudElement::Skill2:   return "skill2";
        case CustomHudElement::Skill3:   return "skill3";
        case CustomHudElement::Skill4:   return "skill4";
        case CustomHudElement::Skill5:   return "skill5";
        case CustomHudElement::Ramen:    return "ramen";
        case CustomHudElement::Item2:    return "item2";
        case CustomHudElement::Item3:    return "item3";
        case CustomHudElement::Item4:    return "item4";
        case CustomHudElement::Joystick: return "joystick";
        }
        return "unknown";
    }

    std::string posXKey(CustomHudElement id)
    {
        return std::string("CustomHUD.") + keyFor(id) + ".x";
    }

    std::string posYKey(CustomHudElement id)
    {
        return std::string("CustomHUD.") + keyFor(id) + ".y";
    }

    std::string scaleKey(CustomHudElement id)
    {
        return std::string("CustomHUD.") + keyFor(id) + ".scale";
    }

    std::string validKey(CustomHudElement id)
    {
        return std::string("CustomHUD.") + keyFor(id) + ".valid";
    }
}

Vec2 CustomHudLayout::loadPosition(CustomHudElement id,
                                   const Vec2 &fallback,
                                   const Size &screenSize)
{
    UserDefault *prefs = UserDefault::sharedUserDefault();
    const std::string valid = validKey(id);

    if (!prefs->getBoolForKey(valid.c_str(), false))
        return fallback;

    const float savedW = prefs->getFloatForKey("CustomHUD.screenW", screenSize.width);
    const float savedH = prefs->getFloatForKey("CustomHUD.screenH", screenSize.height);

    float x = prefs->getFloatForKey(posXKey(id).c_str(), fallback.x);
    float y = prefs->getFloatForKey(posYKey(id).c_str(), fallback.y);

    // Keep the layout usable when the device resolution changes.
    if (savedW > 1.0f && savedH > 1.0f &&
        (fabsf(savedW - screenSize.width) > 0.5f || fabsf(savedH - screenSize.height) > 0.5f))
    {
        x = (x / savedW) * screenSize.width;
        y = (y / savedH) * screenSize.height;
    }

    return Vec2(x, y);
}

float CustomHudLayout::loadScale(CustomHudElement id, float fallback)
{
    UserDefault *prefs = UserDefault::sharedUserDefault();
    if (!prefs->getBoolForKey(validKey(id).c_str(), false))
        return fallback;

    float value = prefs->getFloatForKey(scaleKey(id).c_str(), fallback);
    if (value < 0.35f) value = 0.35f;
    if (value > 2.5f) value = 2.5f;
    return value;
}

void CustomHudLayout::save(CustomHudElement id,
                           const Vec2 &position,
                           float scale,
                           const Size &screenSize)
{
    UserDefault *prefs = UserDefault::sharedUserDefault();

    prefs->setFloatForKey(posXKey(id).c_str(), position.x);
    prefs->setFloatForKey(posYKey(id).c_str(), position.y);
    prefs->setFloatForKey(scaleKey(id).c_str(), scale);
    prefs->setBoolForKey(validKey(id).c_str(), true);
    prefs->setFloatForKey("CustomHUD.screenW", screenSize.width);
    prefs->setFloatForKey("CustomHUD.screenH", screenSize.height);
}

void CustomHudLayout::flush()
{
    UserDefault::sharedUserDefault()->flush();
}

void CustomHudLayout::reset()
{
    UserDefault *prefs = UserDefault::sharedUserDefault();

    const CustomHudElement ids[] = {
        CustomHudElement::Attack,
        CustomHudElement::Skill1,
        CustomHudElement::Skill2,
        CustomHudElement::Skill3,
        CustomHudElement::Skill4,
        CustomHudElement::Skill5,
        CustomHudElement::Ramen,
        CustomHudElement::Item2,
        CustomHudElement::Item3,
        CustomHudElement::Item4,
        CustomHudElement::Joystick
    };

    for (unsigned int i = 0; i < sizeof(ids) / sizeof(ids[0]); ++i)
    {
        prefs->deleteValueForKey(posXKey(ids[i]).c_str());
        prefs->deleteValueForKey(posYKey(ids[i]).c_str());
        prefs->deleteValueForKey(scaleKey(ids[i]).c_str());
        prefs->deleteValueForKey(validKey(ids[i]).c_str());
    }

    prefs->deleteValueForKey("CustomHUD.screenW");
    prefs->deleteValueForKey("CustomHUD.screenH");
    prefs->flush();
}

void CustomHudLayout::apply(HudLayer *hud)
{
    if (hud == nullptr)
        return;

    const Size screen = Director::sharedDirector()->getWinSize();

    ActionButton *buttons[] = {
        hud->nAttackButton,
        hud->skill1Button,
        hud->skill2Button,
        hud->skill3Button,
        hud->skill4Button,
        hud->skill5Button,
        hud->item1Button,
        hud->getItem2Button(),
        hud->getItem3Button(),
        hud->getItem4Button()
    };

    const CustomHudElement ids[] = {
        CustomHudElement::Attack,
        CustomHudElement::Skill1,
        CustomHudElement::Skill2,
        CustomHudElement::Skill3,
        CustomHudElement::Skill4,
        CustomHudElement::Skill5,
        CustomHudElement::Ramen,
        CustomHudElement::Item2,
        CustomHudElement::Item3,
        CustomHudElement::Item4
    };

    for (unsigned int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); ++i)
    {
        if (buttons[i] == nullptr)
            continue;

        buttons[i]->setPosition(loadPosition(ids[i], buttons[i]->getPosition(), screen));
        buttons[i]->setScale(loadScale(ids[i], buttons[i]->getScale()));
    }

    if (hud->_joyStick != nullptr)
    {
        hud->_joyStick->setPosition(loadPosition(CustomHudElement::Joystick,
                                                  hud->_joyStick->getPosition(),
                                                  screen));
        hud->_joyStick->setScale(loadScale(CustomHudElement::Joystick,
                                           hud->_joyStick->getScale()));
    }
}
