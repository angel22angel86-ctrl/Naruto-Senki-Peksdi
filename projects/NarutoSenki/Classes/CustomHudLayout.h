#pragma once

class HudLayer;

class CustomHudLayout
{
public:
    // 0 = original HUD, 1 = compact, 2 = left-handed
    static int getMode();
    static void setMode(int mode);
    static void apply(HudLayer *hud);
};
