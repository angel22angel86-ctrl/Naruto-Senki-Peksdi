#include "CustomHudLayout.h"
#include "HudLayer.h"

namespace
{
    const char *kPrefix = "custom_hud_v1_";

    const char *elementName(CustomHudElement id)
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
        default:                         return "unknown";
        }
    }

    std::string key(const char *name, const char *suffix)
    {
        return std::string(kPrefix) + name + suffix;
    }

    bool hasSaved(CustomHudElement id)
    {
        const std::string name = elementName(id);
        return UserDefault::sharedUserDefault()->getBoolForKey(
            key(name.c_str(), "_saved").c_str(), false);
    }


}

Vec2 CustomHudLayout::loadPosition(CustomHudElement id, const Vec2 &fallback, const Size &screenSize)
{
    if (!hasSaved(id) || screenSize.width <= 0.0f || screenSize.height <= 0.0f)
        return fallback;

    const std::string name = elementName(id);
    auto *defaults = UserDefault::sharedUserDefault();
    const float x = defaults->getFloatForKey(key(name.c_str(), "_x").c_str(), fallback.x / screenSize.width);
    const float y = defaults->getFloatForKey(key(name.c_str(), "_y").c_str(), fallback.y / screenSize.height);

    return Vec2(x * screenSize.width, y * screenSize.height);
}

float CustomHudLayout::loadScale(CustomHudElement id, float fallback)
{
    if (!hasSaved(id))
        return fallback;

    const std::string name = elementName(id);
    return UserDefault::sharedUserDefault()->getFloatForKey(
        key(name.c_str(), "_scale").c_str(), fallback);
}

void CustomHudLayout::save(CustomHudElement id, const Vec2 &position, float scale, const Size &screenSize)
{
    if (screenSize.width <= 0.0f || screenSize.height <= 0.0f)
        return;

    const std::string name = elementName(id);
    auto *defaults = UserDefault::sharedUserDefault();
    defaults->setFloatForKey(key(name.c_str(), "_x").c_str(), position.x / screenSize.width);
    defaults->setFloatForKey(key(name.c_str(), "_y").c_str(), position.y / screenSize.height);
    defaults->setFloatForKey(key(name.c_str(), "_scale").c_str(), scale);
    defaults->setBoolForKey(key(name.c_str(), "_saved").c_str(), true);
}

void CustomHudLayout::reset()
{
    auto *defaults = UserDefault::sharedUserDefault();
    for (int i = 0; i < static_cast<int>(CustomHudElement::Count); ++i)
    {
        const auto id = static_cast<CustomHudElement>(i);
        const std::string name = elementName(id);
        defaults->setBoolForKey(key(name.c_str(), "_saved").c_str(), false);
    }
}

void CustomHudLayout::flush()
{
    UserDefault::sharedUserDefault()->flush();
}

void CustomHudLayout::apply(HudLayer *hud)
{
    if (!hud)
        return;

    const Size screen = winSize;

    auto applyButton = [screen](ActionButton *button, CustomHudElement id)
    {
        if (!button)
            return;

        button->setPosition(CustomHudLayout::loadPosition(id, button->getPosition(), screen));
        button->setScale(CustomHudLayout::loadScale(id, button->getScale()));
        button->syncHudTransform();
    };

    applyButton(hud->nAttackButton, CustomHudElement::Attack);
    applyButton(hud->skill1Button, CustomHudElement::Skill1);
    applyButton(hud->skill2Button, CustomHudElement::Skill2);
    applyButton(hud->skill3Button, CustomHudElement::Skill3);
    applyButton(hud->skill4Button, CustomHudElement::Skill4);
    applyButton(hud->skill5Button, CustomHudElement::Skill5);
    applyButton(hud->item1Button, CustomHudElement::Ramen);
    applyButton(hud->item2Button, CustomHudElement::Item2);
    applyButton(hud->item3Button, CustomHudElement::Item3);
    applyButton(hud->item4Button, CustomHudElement::Item4);

    if (hud->_joyStick)
    {
        hud->_joyStick->setPosition(loadPosition(
            CustomHudElement::Joystick, hud->_joyStick->getPosition(), screen));
        hud->_joyStick->setScale(loadScale(
            CustomHudElement::Joystick, hud->_joyStick->getScale()));
        hud->_joyStick->_defaultPotion = Vec2(
            hud->_joyStick->getContentSize().width / 2.0f - hud->_joyStick->_joyStickControl->getContentSize().width / 2.0f,
            hud->_joyStick->getContentSize().height / 2.0f - hud->_joyStick->_joyStickControl->getContentSize().height / 2.0f);
        hud->_joyStick->_joyStickControl->setPosition(hud->_joyStick->_defaultPotion);
    }
}
