#include "CustomHudLayout.h"
#include "HudLayer.h"

namespace
{
    int g_customHudMode = 0;
    enum { HUD_CLASSIC = 0, HUD_COMPACT = 1, HUD_LEFT = 2 };
}

int CustomHudLayout::getMode() { return g_customHudMode; }

void CustomHudLayout::setMode(int mode)
{
    if (mode < HUD_CLASSIC || mode > HUD_LEFT) mode = HUD_CLASSIC;
    g_customHudMode = mode;
}

void CustomHudLayout::apply(HudLayer *hud)
{
    if (hud == nullptr || g_customHudMode == HUD_CLASSIC) return;

    const float w = winSize.width;
    const float gap = 8.0f;
    const float bottom = 8.0f;

    auto attack = hud->nAttackButton;
    auto s1 = hud->skill1Button;
    auto s2 = hud->skill2Button;
    auto s3 = hud->skill3Button;
    auto s4 = hud->skill4Button;
    auto s5 = hud->skill5Button;

    if (!attack || !s1 || !s2 || !s3 || !s4 || !s5) return;

    const float attackW = attack->getContentSize().width;
    const float attackH = attack->getContentSize().height;
    const float skillW = s1->getContentSize().width;

    if (g_customHudMode == HUD_COMPACT)
    {
        attack->setPosition(Vec2(w - attackW - 10.0f, bottom));
        s1->setPosition(Vec2(w - attackW - skillW - gap - 10.0f, bottom));
        s2->setPosition(Vec2(w - skillW - 10.0f, bottom + attackH + gap));
        s3->setPosition(Vec2(w - attackW - 10.0f, bottom + attackH + gap));
        s4->setPosition(Vec2(w - attackW - skillW * 2.0f - gap * 2.0f - 10.0f, bottom));
        s5->setPosition(Vec2(w - attackW - skillW * 3.0f - gap * 3.0f - 10.0f, bottom));
        return;
    }

    attack->setPosition(Vec2(10.0f, bottom));
    s1->setPosition(Vec2(10.0f + attackW + gap, bottom));
    s2->setPosition(Vec2(10.0f + attackW + gap, bottom + attackH + gap));
    s3->setPosition(Vec2(10.0f, bottom + attackH + gap));
    s4->setPosition(Vec2(10.0f + attackW + skillW + gap * 2.0f, bottom));
    s5->setPosition(Vec2(10.0f + attackW + skillW * 2.0f + gap * 3.0f, bottom));
}
