#include "StartMenu.h"
#include "CustomHudLayout.h"

GameMode s_GameMode = GameMode::Classic;
std::array<std::unique_ptr<IGameModeHandler>, GameMode::__Internal_Max_Length> s_ModeHandlers = {
	std::make_unique<Mode1v1>(),
	std::make_unique<Mode3v3>(),
	std::make_unique<Mode4v4>(),
	std::make_unique<ModeHardCore>(),
	std::make_unique<ModeBoss>(),
	std::make_unique<ModeClone>(false),
	std::make_unique<ModeDeathmatch>(),
	std::make_unique<ModeRandomDeathmatch>(),
};
int Cheats = 0;
bool enableCustomSelect = false;

/*----------------------
init MenuButton ;
----------------------*/


class CustomHudEditorLayer : public Layer, public CCTouchDelegate
{
public:
    struct Item
    {
        CustomHudElement id;
        Sprite *sprite;
        const char *name;
    };

    StartMenu *_delegate = nullptr;
    vector<Item> _items;
    int _selected = -1;
    Vec2 _lastTouch;
    CCLabelBMFont *_selectedLabel = nullptr;

    static CustomHudEditorLayer *create(StartMenu *delegate)
    {
        auto *layer = new CustomHudEditorLayer();
        if (layer && layer->init(delegate))
        {
            layer->autorelease();
            return layer;
        }
        delete layer;
        return nullptr;
    }

    bool init(StartMenu *delegate)
    {
        RETURN_FALSE_IF(!Layer::init());
        _delegate = delegate;

        // The HUD sprites are packed in UI.plist. The main menu does not need
        // this sheet normally, so load it lazily when the editor is opened.
        if (getSpriteFrame("JoyStickBg.png") == nullptr)
            addSprites("UI.plist");

        addChild(LayerColor::create(ccc4(0, 0, 0, 235), winSize.width, winSize.height), -10);

        auto topBar = Sprite::create("menu_bar3.png");
        topBar->setAnchorPoint(Vec2(0, 0));
        topBar->setPosition(Vec2(0, winSize.height - topBar->getContentSize().height));
        FULL_SCREEN_SPRITE(topBar);
        addChild(topBar, -5);

        auto title = CCLabelBMFont::create("CUSTOM HUD", Fonts::Default);
        title->setScale(0.55f);
        title->setPosition(Vec2(115, winSize.height - 28));
        addChild(title, 10);

        auto help = CCLabelBMFont::create("DRAG = MOVE   +/- = RESIZE   SAVE = APPLY", Fonts::Default);
        help->setScale(0.28f);
        help->setPosition(Vec2(winSize.width / 2, winSize.height - 42));
        addChild(help, 10);

        _selectedLabel = CCLabelBMFont::create("Selected: Skill 1", Fonts::Default);
        _selectedLabel->setScale(0.32f);
        _selectedLabel->setAnchorPoint(Vec2(0, 0));
        _selectedLabel->setPosition(Vec2(12, winSize.height - 62));
        addChild(_selectedLabel, 10);

        createItem(CustomHudElement::Attack, "n_attack.png", "Attack");
        createItem(CustomHudElement::Skill1, "Naruto_skill1.png", "Skill 1");
        createItem(CustomHudElement::Skill2, "Naruto_skill2.png", "Skill 2");
        createItem(CustomHudElement::Skill3, "Naruto_skill3.png", "Skill 3");
        createItem(CustomHudElement::Skill4, "Naruto_skill4.png", "Skill 4");
        createItem(CustomHudElement::Skill5, "Naruto_skill5.png", "Skill 5");
        createItem(CustomHudElement::Ramen, "item1.png", "Ramen");
        createItem(CustomHudElement::Item2, "item2.png", "Item 2");
        createItem(CustomHudElement::Item3, "item3.png", "Item 3");
        createItem(CustomHudElement::Item4, "item4.png", "Item 4");
        createJoystick();

        for (auto &item : _items)
            loadItem(item);

        createControls();
        selectItem(1);
        return true;
    }

    void createItem(CustomHudElement id, const char *frameName, const char *name)
    {
        auto *sprite = Sprite::createWithSpriteFrameName(frameName);
        if (!sprite)
        {
            CCLOGERROR("Custom HUD: missing frame %s", frameName);
            return;
        }
        sprite->setAnchorPoint(Vec2(0.5f, 0.5f));
        addChild(sprite, 5);
        _items.push_back({id, sprite, name});
    }

    void createJoystick()
    {
        auto *bg = Sprite::createWithSpriteFrameName("JoyStickBg.png");
        if (!bg)
        {
            CCLOGERROR("Custom HUD: missing JoyStickBg.png");
            return;
        }
        bg->setAnchorPoint(Vec2(0.5f, 0.5f));
        auto *stick = Sprite::createWithSpriteFrameName("JoyStick.png");
        if (stick)
        {
            stick->setAnchorPoint(Vec2(0.5f, 0.5f));
            stick->setPosition(Vec2(bg->getContentSize().width / 2, bg->getContentSize().height / 2));
            bg->addChild(stick);
        }
        addChild(bg, 5);
        _items.push_back({CustomHudElement::Joystick, bg, "Analog"});
    }

    float defaultScale(CustomHudElement id) const
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        return 1.0f;
#else
        if (id >= CustomHudElement::Skill1 && id <= CustomHudElement::Skill5)
            return DESKTOP_UI_SCALE;
        return 1.0f;
#endif
    }

    Vec2 defaultPosition(CustomHudElement id, Sprite *node) const
    {
        const float width = winSize.width;
        const float height = winSize.height;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        switch (id)
        {
        case CustomHudElement::Attack: return Vec2(width - 60, 8);
        case CustomHudElement::Skill1: return Vec2(width - node->getContentSize().width - 64, 2);
        case CustomHudElement::Skill2: return Vec2(width - 95, 50);
        case CustomHudElement::Skill3: return Vec2(width - 44, 8 + node->getContentSize().height + 8);
        case CustomHudElement::Skill4:
        {
            auto *skill1 = findSprite(CustomHudElement::Skill1);
            return Vec2(skill1->getPositionX() - node->getContentSize().width - 8, 2);
        }
        case CustomHudElement::Skill5:
        {
            auto *skill4 = findSprite(CustomHudElement::Skill4);
            return Vec2(skill4->getPositionX() - node->getContentSize().width - 8, 2);
        }
        case CustomHudElement::Ramen:
        {
            auto *skill3 = findSprite(CustomHudElement::Skill3);
            return Vec2(8, skill3->getPositionY() + skill3->getContentSize().height);
        }
        case CustomHudElement::Item2:
        {
            auto *skill3 = findSprite(CustomHudElement::Skill3);
            return Vec2(width - 44, skill3->getPositionY() + skill3->getContentSize().height + 8);
        }
        case CustomHudElement::Item3:
        {
            auto *skill5 = findSprite(CustomHudElement::Skill5);
            return Vec2(skill5->getPositionX() - skill5->getContentSize().width - 8, 2);
        }
        case CustomHudElement::Item4:
            return Vec2(findSprite(CustomHudElement::Skill2)->getPositionX() - node->getContentSize().width - 8, 50);
        case CustomHudElement::Joystick:
            return Vec2((width / 570.0f) * 28.0f, (height / 320.0f) * 16.0f);
        default: break;
        }
#else
        const float skillWidth = findSprite(CustomHudElement::Skill1)->getContentSize().width;
        switch (id)
        {
        case CustomHudElement::Attack: return Vec2(width + 100, -100);
        case CustomHudElement::Skill1: return Vec2(width / 2 - (skillWidth + 8) * 2, 2);
        case CustomHudElement::Skill2: return Vec2(width / 2 - skillWidth - 8, 2);
        case CustomHudElement::Skill3: return Vec2(width / 2, 2);
        case CustomHudElement::Skill4: return Vec2(width / 2 + skillWidth + 8, 2);
        case CustomHudElement::Skill5: return Vec2(width / 2 + (skillWidth + 8) * 2, 2);
        case CustomHudElement::Ramen: return Vec2(8, 8 + findSprite(CustomHudElement::Attack)->getContentSize().height + skillWidth);
        case CustomHudElement::Item3: return Vec2(width - skillWidth - 64, 2);
        case CustomHudElement::Item4: return Vec2(width - 95, 50);
        case CustomHudElement::Item2: return Vec2(width - 44, 8 + findSprite(CustomHudElement::Attack)->getContentSize().height + 8);
        case CustomHudElement::Joystick: return Vec2((width / 570.0f) * 28.0f, (height / 320.0f) * 16.0f);
        default: break;
        }
#endif
        return Vec2(width / 2, height / 2);
    }

    Sprite *findSprite(CustomHudElement id) const
    {
        for (const auto &item : _items)
            if (item.id == id)
                return item.sprite;
        return nullptr;
    }

    Item *findItem(CustomHudElement id)
    {
        for (auto &item : _items)
            if (item.id == id)
                return &item;
        return nullptr;
    }

    void loadItem(Item &item)
    {
        const Vec2 fallback = defaultPosition(item.id, item.sprite);
        item.sprite->setPosition(CustomHudLayout::loadPosition(item.id, fallback, winSize));
        item.sprite->setScale(CustomHudLayout::loadScale(item.id, defaultScale(item.id)));
    }

    void saveAll()
    {
        for (const auto &item : _items)
            CustomHudLayout::save(item.id, item.sprite->getPosition(), item.sprite->getScale(), winSize);
        CustomHudLayout::flush();
    }

    void resetAll()
    {
        for (auto &item : _items)
        {
            item.sprite->setPosition(defaultPosition(item.id, item.sprite));
            item.sprite->setScale(defaultScale(item.id));
        }
    }

    void createControls()
    {
        auto *saveItem = MenuItemSprite::create(
            Sprite::createWithSpriteFrameName("yes_btn1.png"),
            Sprite::createWithSpriteFrameName("yes_btn2.png"),
            this, menu_selector(CustomHudEditorLayer::onSave));
        auto *saveText = CCLabelBMFont::create("SAVE HUD", Fonts::Default);
        saveText->setScale(0.28f);
        saveText->setPosition(Vec2(saveItem->getContentSize().width / 2, saveItem->getContentSize().height / 2));
        saveItem->addChild(saveText);

        auto *resetItem = MenuItemSprite::create(
            Sprite::createWithSpriteFrameName("no_btn1.png"),
            Sprite::createWithSpriteFrameName("no_btn2.png"),
            this, menu_selector(CustomHudEditorLayer::onReset));
        auto *resetText = CCLabelBMFont::create("RESET", Fonts::Default);
        resetText->setScale(0.28f);
        resetText->setPosition(Vec2(resetItem->getContentSize().width / 2, resetItem->getContentSize().height / 2));
        resetItem->addChild(resetText);

        auto *minusItem = MenuItemLabel::create(CCLabelBMFont::create("-", Fonts::Default), this, menu_selector(CustomHudEditorLayer::onScaleDown));
        auto *plusItem = MenuItemLabel::create(CCLabelBMFont::create("+", Fonts::Default), this, menu_selector(CustomHudEditorLayer::onScaleUp));
        minusItem->setScale(0.8f);
        plusItem->setScale(0.8f);

        auto *menu = Menu::create(saveItem, resetItem, minusItem, plusItem, nullptr);
        menu->setPosition(Vec2(winSize.width / 2, 30));
        menu->alignItemsHorizontallyWithPadding(18);
        addChild(menu, 20);
    }

    void selectItem(int index)
    {
        if (_items.empty())
            return;
        _selected = std::max(0, std::min(index, static_cast<int>(_items.size()) - 1));
        _selectedLabel->setString(string("Selected: ") + _items[_selected].name);
    }

    void onScaleUp(Ref *)
    {
        if (_selected < 0 || _selected >= static_cast<int>(_items.size())) return;
        auto *node = _items[_selected].sprite;
        node->setScale(std::min(2.5f, node->getScale() + 0.1f));
    }

    void onScaleDown(Ref *)
    {
        if (_selected < 0 || _selected >= static_cast<int>(_items.size())) return;
        auto *node = _items[_selected].sprite;
        node->setScale(std::max(0.35f, node->getScale() - 0.1f));
    }

    void onReset(Ref *)
    {
        resetAll();
    }

    void onSave(Ref *)
    {
        saveAll();
        if (_delegate)
            _delegate->exitCustomMode();
    }

    void onEnter()
    {
        Layer::onEnter();
        Director::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -10, true);
    }

    void onExit()
    {
        Director::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        Layer::onExit();
    }

    bool ccTouchBegan(Touch *touch, Event *)
    {
        const Vec2 point = convertTouchToNodeSpace(touch);
        for (int i = static_cast<int>(_items.size()) - 1; i >= 0; --i)
        {
            auto *node = _items[i].sprite;
            if (node->getBoundingBox().containsPoint(point))
            {
                selectItem(i);
                _lastTouch = point;
                return true;
            }
        }
        return false;
    }

    void ccTouchMoved(Touch *touch, Event *)
    {
        if (_selected < 0 || _selected >= static_cast<int>(_items.size())) return;
        const Vec2 point = convertTouchToNodeSpace(touch);
        const Vec2 delta = point - _lastTouch;
        _items[_selected].sprite->setPosition(_items[_selected].sprite->getPosition() + delta);
        _lastTouch = point;
    }

    void ccTouchEnded(Touch *, Event *)
    {
        _lastTouch = Vec2::ZERO;
    }
};

bool MenuButton::init(const char *szImage)
{
	RETURN_FALSE_IF(!Sprite::init());

	initWithSpriteFrameName(szImage);
	setAnchorPoint(Vec2(0.5, 0));

	return true;
}

void MenuButton::onEnter()
{
	Sprite::onEnter();
	Director::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 10, true);

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

	// JniMethodInfo minfo;

	// bool isHave = JniHelper::getStaticMethodInfo(minfo, "net/zakume/game/NarutoSenki", "showBanner", "()V");
	// if (isHave)
	// {
	// 	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID);
	// }

#endif
}

void MenuButton::onExit()
{
	Sprite::onExit();
	Director::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

CCRect MenuButton::getRect()
{
	CCSize size = getContentSize();
	return CCRect(0, 0, size.width, size.height);
}

bool MenuButton::containsTouchLocation(Touch *touch)
{
	return getRect().containsPoint(convertTouchToNodeSpace(touch));
}

void MenuButton::setBtnType(MenuButtonType type)
{
	_type = type;
}

MenuButtonType MenuButton::getBtnType()
{
	return _type;
}

bool MenuButton::ccTouchBegan(Touch *touch, Event *event)
{
	// touch area
	if (!containsTouchLocation(touch))
		return false;

	// click();

	prePosY = 0;

	return true;
}

void MenuButton::ccTouchMoved(Touch *touch, Event *event)
{
	// touch area
	if (prePosY == 0)
	{
		prePosY = touch->getLocation().y;
	}
	else
	{
		if (getBtnType() != MenuButtonType::HardCore)
		{
			if (abs(touch->getLocation().y - prePosY) > 16)
			{
				if (touch->getLocation().y < prePosY)
				{
					_startMenu->isClockwise = true;
				}
				else
				{
					_startMenu->isClockwise = false;
				}
				_startMenu->isDrag = true;
			}
		}
	}
}

void MenuButton::ccTouchEnded(Touch *touch, Event *event)
{
	if (_isTop && !_startMenu->isDrag)
	{
		switch (_type)
		{
		case MenuButtonType::Training:
			SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");
			_startMenu->onTrainingCallBack();
			break;
		case MenuButtonType::Credits:
			SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");
			_startMenu->onCreditsCallBack();
			break;
		case MenuButtonType::Exit:
			SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");
			_startMenu->onExitCallBack();
			break;
		case MenuButtonType::Custom:
			SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");
			_startMenu->enterCustomMode();
			break;
		case MenuButtonType::HardCore:
			SimpleAudioEngine::sharedEngine()->playEffect(SELECT_SOUND);
			auto frame = getSpriteFrame("menu05_text.png");
			_startMenu->menuText->setDisplayFrame(frame);
			_startMenu->onHardLayerCallBack();
			break;
		}
	}
	else
	{
		SimpleAudioEngine::sharedEngine()->playEffect(SELECT_SOUND);
		prePosY = 0;
		_startMenu->scrollMenu(getPositionY());
		_startMenu->isDrag = false;
	}
}

void MenuButton::playSound()
{
	SimpleAudioEngine::sharedEngine()->stopAllEffects();

	switch (_type)
	{
	case MenuButtonType::Training:
		SimpleAudioEngine::sharedEngine()->playEffect(TRAINING_SOUND);
		break;
	case MenuButtonType::Custom:
		SimpleAudioEngine::sharedEngine()->playEffect(NETWORK_SOUND);
		break;
	case MenuButtonType::Credits:
		SimpleAudioEngine::sharedEngine()->playEffect(CREDITS_SOUND);
		break;
	case MenuButtonType::Exit:
		SimpleAudioEngine::sharedEngine()->playEffect(EXIT_SOUND);
		break;
	default:
		break;
	}
}

MenuButton *MenuButton::create(const char *szImage)
{
	auto mb = new MenuButton();
	if (mb && mb->init(szImage))
	{
		mb->autorelease();
		return mb;
	}
	else
	{
		delete mb;
		return nullptr;
	}
}

/*----------------------
init StartMenu layer;
----------------------*/

StartMenu::StartMenu()
{
	isClockwise = false;
	isDrag = false;
	menuText = nullptr;
	hardCoreLayer = nullptr;
	notice_layer = nullptr;
	noticeLabel = nullptr;
	login_btn = nullptr;
	customHudLayer = nullptr;
}

bool StartMenu::init()
{
	RETURN_FALSE_IF(!Layer::init());

	addSprites("Menu.plist");
	addSprites("Result.plist");
	addSprites("NamePlate.plist");

	// Vec2 origin = Director::sharedDirector()->getVisibleOrigin();

	// Sprite* bgSprite = Sprite::create("red_bg.png");
	////pSprite->setPosition(Vec2(winSize.width/2 + origin.x, winSize.height/2 + origin.y));
	// FULL_SCREEN_SPRITE(bgSprite);
	// bgSprite->setAnchorPoint(Vec2(0,0));
	// bgSprite->setPosition(Vec2(0,0));
	// addChild(bgSprite, -5);

	// produce groud
	Sprite *gold_left = Sprite::createWithSpriteFrameName("gold_left.png");
	gold_left->setAnchorPoint(Vec2(0, 0));
	gold_left->setPosition(Vec2(0, 20));
	addChild(gold_left, 1);

	Sprite *gold_right = Sprite::createWithSpriteFrameName("gold_right.png");
	gold_right->setAnchorPoint(Vec2(0, 1));
	gold_right->setPosition(Vec2(winSize.width - gold_right->getContentSize().width - 20, winSize.height - 20));
	addChild(gold_right, 1);

	// produce the cloud
	Sprite *cloud_left = Sprite::createWithSpriteFrameName("cloud.png");
	cloud_left->setPosition(Vec2(0, 15));
	cloud_left->setFlipX(true);
	cloud_left->setFlipY(true);
	cloud_left->setAnchorPoint(Vec2(0, 0));
	addChild(cloud_left, 1);

	auto cmv1 = MoveBy::create(1, Vec2(-15, 0));
	auto cseq1 = RepeatForever::create(newSequence(cmv1, cmv1->reverse()));
	cloud_left->runAction(cseq1);

	Sprite *cloud_right = Sprite::createWithSpriteFrameName("cloud.png");
	cloud_right->setPosition(Vec2(winSize.width - cloud_right->getContentSize().width,
								  winSize.height - (cloud_right->getContentSize().height + 15)));
	cloud_right->setAnchorPoint(Vec2(0, 0));
	addChild(cloud_right, 1);

	auto cmv2 = MoveBy::create(1, Vec2(15, 0));
	auto cseq2 = RepeatForever::create(newSequence(cmv2, cmv2->reverse()));
	cloud_right->runAction(cseq2);

	// produce the menu_bar
	Sprite *menu_bar_b = Sprite::create("menu_bar2.png");
	menu_bar_b->setAnchorPoint(Vec2(0, 0));
	FULL_SCREEN_SPRITE(menu_bar_b);
	addChild(menu_bar_b, 2);

	Sprite *menu_bar_t = Sprite::create("menu_bar3.png");
	menu_bar_t->setAnchorPoint(Vec2(0, 0));
	menu_bar_t->setPosition(Vec2(0, winSize.height - menu_bar_t->getContentSize().height));
	FULL_SCREEN_SPRITE(menu_bar_t);
	addChild(menu_bar_t, 2);

	Sprite *startmenu_title = Sprite::createWithSpriteFrameName("startmenu_title.png");
	startmenu_title->setAnchorPoint(Vec2(0, 0));
	startmenu_title->setPosition(Vec2(2, winSize.height - startmenu_title->getContentSize().height - 2));
	addChild(startmenu_title, 3);

	// produce the menu button
	auto gamemode_btn = MenuButton::create("menu01.png");
	gamemode_btn->setDelegate(this);
	gamemode_btn->setBtnType(MenuButtonType::Custom);
	gamemode_btn->setScale(0.5f);
	gamemode_btn->setPositionY(_pos03);
	_menuArray.push_back(gamemode_btn);

	auto credits_btn = MenuButton::create("menu04.png");
	credits_btn->setDelegate(this);
	credits_btn->setBtnType(MenuButtonType::Credits);
	credits_btn->setScale(0.5f);
	credits_btn->setVisible(false);
	credits_btn->_isBottom = true;
	credits_btn->setPositionY(_pos02);
	_menuArray.push_back(credits_btn);

	auto training_btn = MenuButton::create("menu02.png");
	training_btn->setDelegate(this);
	training_btn->setBtnType(MenuButtonType::Training);
	training_btn->_isTop = true;
	training_btn->setPositionY(_pos02);
	_menuArray.push_back(training_btn);

	auto exit_btn = MenuButton::create("menu03.png");
	exit_btn->setDelegate(this);
	exit_btn->setBtnType(MenuButtonType::Exit);
	exit_btn->setScale(0.5f);
	exit_btn->setPositionY(_pos01);
	_menuArray.push_back(exit_btn);

	menuText = Sprite::createWithSpriteFrameName("menu02_text.png");
	menuText->setAnchorPoint(Vec2(0, 0));
	menuText->setPosition(Vec2(10, 2));
	addChild(menuText, 5);

	for (auto menu : _menuArray)
	{
		menu->setPositionX(105);
		addChild(menu, 2);
	}
	auto versionLabel = CCLabelBMFont::create(VERSION_CODE, Fonts::Default);
	versionLabel->setScale(0.3f);
	versionLabel->setPosition(winSize.width - 25, 10);
	addChild(versionLabel, 5);

	Sprite *avator = Sprite::createWithSpriteFrameName("avator1.png");
	avator->setAnchorPoint(Vec2(0, 0));
	avator->setOpacity(0);
	avator->setPosition(Vec2(winSize.width - avator->getContentSize().width, 19));
	addChild(avator, 1);

	Vector<SpriteFrame *> frames;
	Vector<FiniteTimeAction *> list;
	int i = 0;
	while (++i < 5)
	{
		auto frame = getSpriteFrame("avator{}.png", i);
		frames.pushBack(frame);
		auto tempAnimation = Animation::createWithSpriteFrames(frames, 0.1f);
		auto tempAction = Animate::create(tempAnimation);
		list.pushBack(tempAction);
		auto fadeIn = FadeIn::create(0.8f);
		auto delay = DelayTime::create(1.0f);
		auto fadeOut = FadeOut::create(0.5f);
		list.pushBack(fadeIn);
		list.pushBack(delay);
		list.pushBack(fadeOut);
	}

	avator->runAction(RepeatForever::create(Sequence::create(list)));
	MenuItem *news_btn = MenuItemSprite::create(Sprite::createWithSpriteFrameName("news_btn.png"), nullptr, this, menu_selector(StartMenu::onNewsBtn));
	Menu *menu = Menu::create(news_btn, nullptr);
	news_btn->setAnchorPoint(Vec2(0, 0.5f));
	menu->setPosition(15, winSize.height - 50);
	addChild(menu, 5);

	setNotice();

	login_btn = MenuItemSprite::create(Sprite::createWithSpriteFrameName("login_btn.png"), nullptr, this, menu_selector(StartMenu::onLoginBtn));
	Menu *menu2 = Menu::create(login_btn, nullptr);
	login_btn->setAnchorPoint(Vec2(1, 0.5f));
	menu2->setPosition(winSize.width - 15, winSize.height - 50);
	addChild(menu2, 5);

	scheduleUpdate();

	return true;
}

void StartMenu::onEnter()
{
	Layer::onEnter();
	// SimpleAudioEngine::sharedEngine()->end();
	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(true);

	if (UserDefault::sharedUserDefault()->getBoolForKey("isBGM"))
	{
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic(MENU_MUSIC, true);
	}
}

void StartMenu::onExit()
{
	Layer::onExit();
	// Keep audio engine alive across scene transitions (e.g. Credits),
	// otherwise newly started BGM can be cut off when StartMenu exits.
}

void StartMenu::onLoginBtn(Ref *sender)
{
	auto tip = CCTips::create("ServerMainten");
	addChild(tip, 5000);
	return;
}

void StartMenu::update(float dt)
{
	if(!noticeLabel){
		return;
	}

	float currentX = noticeLabel->getPositionX();
	//float contentX = getContentSize().width;
	float lableX = noticeLabel->getContentSize().width;


	if(noticeLabel->getPositionX()>=-lableX){
		noticeLabel->setPositionX(noticeLabel->getPositionX()-0.6f);
	}	
	else{
		noticeLabel->setPositionX(190);
	}
}

void StartMenu::setNotice()
{
	if (!notice_layer)
	{
		notice_layer = Layer::create();
		Sprite *notice_bg = Sprite::createWithSpriteFrameName("notice_bg.png");
		notice_bg->setAnchorPoint(Vec2(0, 0));
		notice_bg->setPosition(Vec2(15, 228));
		notice_layer->addChild(notice_bg);

		ClippingNode *clipper = ClippingNode::create();
		Node *stencil = Sprite::createWithSpriteFrameName("notice_mask.png");
		stencil->setAnchorPoint(Vec2(0, 0));
		clipper->setStencil(stencil);

		auto strings = CCDictionary::createWithContentsOfFile("Config/strings.xml");
		auto reply = ((CCString *)strings->objectForKey("Notice"))->m_sString.c_str();

		noticeLabel = CCLabelTTF::create(reply, FONT_NAME, 12);
		noticeLabel->setAnchorPoint(Vec2(0, 0));
		clipper->addChild(noticeLabel);
		clipper->setPosition(Vec2(35, 228));

		notice_layer->addChild(clipper);

		addChild(notice_layer);
	}
}

void StartMenu::onNewsBtn(Ref *sender)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

	// JniMethodInfo minfo;

	// bool isHave = JniHelper::getStaticMethodInfo(minfo, "net/zakume/game/NarutoSenki", "getInstance", "()Lnet/zakume/game/NarutoSenki;");
	// jobject jobj;
	// if (isHave)
	// {
	// 	jobj = minfo.env->CallStaticObjectMethod(minfo.classID, minfo.methodID);

	// 	isHave = JniHelper::getMethodInfo(minfo, "net/zakume/game/NarutoSenki", "openWebview", "()V");
	// 	if (isHave)
	// 	{
	// 		minfo.env->CallVoidMethod(jobj, minfo.methodID);
	// 	}
	// }

#endif
}

void StartMenu::onHardCoreOn(Ref *sender)
{
	SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");
	if (hardCoreLayer)
	{
		hardCoreLayer->removeAllChildren();
		hardCoreLayer->removeFromParent();
		hardCoreLayer = nullptr;
	}
}

void StartMenu::onHardCoreOff(Ref *sender)
{
	SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/cancel.ogg");
	if (hardCoreLayer)
	{
		hardCoreLayer->removeAllChildren();
		hardCoreLayer->removeFromParent();
		hardCoreLayer = nullptr;
	}
}

void StartMenu::onHardLayerCallBack()
{
	if (UserDefault::sharedUserDefault()->getBoolForKey("isHardCore") == false)
	{
		if (!hardCoreLayer)
		{
			hardCoreLayer = Layer::create();

			Sprite *confirm_bg = Sprite::createWithSpriteFrameName("confirm_bg.png");
			confirm_bg->setPosition(Vec2(winSize.width / 2, winSize.height / 2));

			Sprite *hardcore_title = Sprite::createWithSpriteFrameName("hardcore_title.png");
			hardcore_title->setPosition(Vec2(winSize.width / 2, winSize.height / 2 + 38));

			Sprite *hardcore_text = Sprite::createWithSpriteFrameName("hardcore_text.png");
			hardcore_text->setPosition(Vec2(winSize.width / 2, winSize.height / 2 + 8));

			MenuItem *yes_btn = MenuItemSprite::create(Sprite::createWithSpriteFrameName("yes_btn1.png"), Sprite::createWithSpriteFrameName("yes_btn2.png"), this, menu_selector(StartMenu::onHardCoreOn));
			MenuItem *no_btn = MenuItemSprite::create(Sprite::createWithSpriteFrameName("no_btn1.png"), Sprite::createWithSpriteFrameName("no_btn2.png"), this, menu_selector(StartMenu::onHardCoreOff));

			Menu *confirm_menu = Menu::create(yes_btn, no_btn, nullptr);
			confirm_menu->alignItemsHorizontallyWithPadding(24);
			confirm_menu->setPosition(Vec2(winSize.width / 2, winSize.height / 2 - 30));

			hardCoreLayer->addChild(confirm_bg, 600);
			hardCoreLayer->addChild(confirm_menu, 650);
			hardCoreLayer->addChild(hardcore_title, 650);
			hardCoreLayer->addChild(hardcore_text, 650);
			addChild(hardCoreLayer, 700);
		}
	}
}

void StartMenu::enterCustomMode()
{
    if (customHudLayer)
        return;

    customHudLayer = CustomHudEditorLayer::create(this);
    if (customHudLayer)
        addChild(customHudLayer, 1000);
}

void StartMenu::exitCustomMode()
{
    if (customHudLayer)
    {
        customHudLayer->removeFromParentAndCleanup(true);
        customHudLayer = nullptr;
    }
}

void StartMenu::onTrainingCallBack()
{
	// Enter game mode scene
	auto modeScene = Scene::create();
	auto gameModeLayer = GameModeLayer::create();
	modeScene->addChild(gameModeLayer);

	Director::sharedDirector()->replaceScene(TransitionFade::create(1.25f, modeScene));
}

void StartMenu::onCreditsCallBack()
{
	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
	Scene *creditsScene = Scene::create();
	CreditsLayer *creditsLayer = CreditsLayer::create();
	creditsScene->addChild(creditsLayer);
	Director::sharedDirector()->replaceScene(TransitionFade::create(1.25f, creditsScene));
}

void StartMenu::scrollMenu(int posY)
{
	if (posY > _pos02 || (isDrag && isClockwise))
	{
		for (auto menu : _menuArray)
		{
			if (menu->getPositionY() == _pos01)
			{
				auto spn = MoveTo::create(0.5, Vec2(105, _pos02));
				reorderChild(menu, 1);
				menu->_isBottom = true;
				auto fo = FadeOut::create(0.3f);
				auto seq = Spawn::createWithTwoActions(spn, fo);
				menu->runAction(seq);
			}
			else if (menu->getPositionY() == _pos03)
			{
				auto spn = Spawn::createWithTwoActions(
					MoveTo::create(0.5, Vec2(105, _pos02)),
					ScaleTo::create(0.5, 1));
				reorderChild(menu, 3);
				menu->_isTop = true;

				auto call = CallFunc::create(std::bind(&MenuButton::playSound, menu));
				auto seq = newSequence(spn, call);
				menu->runAction(seq);
			}
			else if (menu->getPositionY() == _pos02 && menu->_isBottom)
			{
				auto spn = MoveTo::create(0.5, Vec2(105, _pos03));
				reorderChild(menu, 1);
				menu->setVisible(true);
				auto fi = FadeIn::create(0.3f);
				auto seq = Spawn::createWithTwoActions(spn, fi);
				menu->_isBottom = false;
				menu->runAction(seq);
			}
			else if (menu->getPositionY() == _pos02 && !menu->_isBottom)
			{
				auto spn = Spawn::createWithTwoActions(
					MoveTo::create(0.5, Vec2(105, _pos01)),
					ScaleTo::create(0.5, 0.5));
				reorderChild(menu, 1);
				menu->_isTop = false;
				menu->runAction(spn);
			}
		}
	}
	else
	{
		for (auto menu : _menuArray)
		{
			if (menu->getPositionY() == _pos01)
			{
				auto spn = Spawn::createWithTwoActions(
					MoveTo::create(0.5, Vec2(105, _pos02)),
					ScaleTo::create(0.5, 1));
				reorderChild(menu, 3);
				menu->_isTop = true;

				auto call = CallFunc::create(std::bind(&MenuButton::playSound, menu));
				auto seq = newSequence(spn, call);
				menu->runAction(seq);
			}
			else if (menu->getPositionY() == _pos03)
			{
				auto spn = MoveTo::create(0.5, Vec2(105, _pos02));
				reorderChild(menu, 1);
				menu->_isBottom = true;
				auto fo = FadeOut::create(0.3f);
				auto seq = Spawn::createWithTwoActions(spn, fo);
				menu->runAction(seq);
			}
			else if (menu->getPositionY() == _pos02 && menu->_isBottom)
			{
				auto spn = MoveTo::create(0.5, Vec2(105, _pos01));
				reorderChild(menu, 2);
				menu->setVisible(true);
				auto fi = FadeIn::create(0.3f);
				auto seq = Spawn::createWithTwoActions(spn, fi);
				menu->_isBottom = false;
				menu->runAction(seq);
			}

			else if (menu->getPositionY() == _pos02 && !menu->_isBottom)
			{
				auto spn = Spawn::createWithTwoActions(
					MoveTo::create(0.5, Vec2(105, _pos03)),
					ScaleTo::create(0.5, 0.5));
				reorderChild(menu, 2);
				menu->_isTop = false;
				menu->runAction(spn);
			}
		}
	}

	string src;
	for (auto menu : _menuArray)
	{
		if (menu->_isTop)
		{
			switch (menu->getBtnType())
			{
			case MenuButtonType::Training:
				src = "menu02_text.png";
				break;
			case MenuButtonType::Custom:
				src = "menu01_text.png";
				break;
			case MenuButtonType::Credits:
				src = "menu04_text.png";
				break;
			case MenuButtonType::Exit:
				src = "menu03_text.png";
				break;
			default:
				break;
			}
		}
	}

	auto frame = getSpriteFrame(src);
	menuText->setDisplayFrame(frame);
}

void StartMenu::keyBackClicked()
{
	if (customHudLayer)
	{
		exitCustomMode();
		return;
	}

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

	// JniMethodInfo minfo;
	// bool isHave = JniHelper::getStaticMethodInfo(minfo, "net/zakume/game/DialogUtils", "showTipDialog", "(Ljava/lang/String;Ljava/lang/String;)V");

	// if (isHave)
	// {
	// 	jstring jTitle = minfo.env->NewStringUTF("Exit Game");
	// 	jstring jMsg = minfo.env->NewStringUTF("Do you really want to exit?");
	// 	minfo.env->CallStaticVoidMethod(minfo.classID, minfo.methodID, jTitle, jMsg);
	// 	minfo.env->DeleteLocalRef(jTitle);
	// 	minfo.env->DeleteLocalRef(jMsg);
	// }
#else
	Director::sharedDirector()->end();
	exit(0);
#endif
}

void StartMenu::setCheats(int cheats)
{
	Cheats = cheats;
}

void StartMenu::onExitCallBack()
{
	keyBackClicked();
}
