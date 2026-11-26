#include <time.h>
#include "StartMenu.h"
#include "GameLayer.h"
#include "Data/Config.h"
#include "Utils/Cocos2dxHelper.hpp"
#include "Constants/UiFlowKeys.hpp"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "../../../cocos2dx/platform/android/jni/JniHelper.h"
#endif

SelectLayer::~SelectLayer()
{
	CC_SAFE_RELEASE(_selectList);

	auto handler = getGameModeHandler();
	handler->selectLayer = nullptr;
	if (!isStart)
		handler->resetCheats();
}

void SelectLayer::onGameStart()
{
	// First START: initialize the selected heroes and open Map Select.
	// Second START (inside Map Select): launch the original loading flow.
	if (!isMapSelect)
	{
		isMapSelect = true;
		auto gameModeHandler = getGameModeHandler();
		gameModeHandler->selectLayer = this;
		gameModeHandler->onInitHeros();
		selectedMapId = 1;
		showMapSelect();
		return;
	}

	if (isStart)
		return;
	isStart = true;
	setKeypadEnabled(false);
	getGameModeHandler()->setSelectedMap(selectedMapId);

	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(true);
	SimpleAudioEngine::sharedEngine()->playEffect("Audio/Menu/confirm.ogg");

	Scene *loadScene = Scene::create();
	LoadLayer *loadLayer = LoadLayer::create();
	loadLayer->preloadAudio();
	loadScene->addChild(loadLayer);
	Director::sharedDirector()->replaceScene(TransitionFade::create(1.25f, loadScene));
}

void SelectLayer::showMapSelect()
{
	// Hide the existing character-select UI without destroying its state.
	for (auto child : getChildren())
		child->setVisible(false);

	mapSelectOverlay = Node::create();
	addChild(mapSelectOverlay, 1000);

	auto background = Sprite::create("blue_bg.png");
	if (background)
	{
		background->setAnchorPoint(Vec2::ZERO);
		background->setPosition(Vec2::ZERO);
		background->setScaleX(winSize.width / background->getContentSize().width);
		background->setScaleY(winSize.height / background->getContentSize().height);
		mapSelectOverlay->addChild(background, -5);
	}
	auto topBar = Sprite::create("menu_bar3.png");
	if (topBar)
	{
		topBar->setAnchorPoint(Vec2::ZERO);
		topBar->setPosition(Vec2(0, winSize.height - topBar->getContentSize().height));
		mapSelectOverlay->addChild(topBar, 1);
	}

	updateMapPreview();

	// Reuse existing atlas artwork: page buttons for map navigation,
	// START for confirmation, and RETURN to go back.
	auto previous = MenuItemSprite::create(
		Sprite::createWithSpriteFrameName("page1_off.png"),
		Sprite::createWithSpriteFrameName("page1_on.png"),
		CC_CALLBACK_1(SelectLayer::onPreviousMap, this));
	auto next = MenuItemSprite::create(
		Sprite::createWithSpriteFrameName("page2_off.png"),
		Sprite::createWithSpriteFrameName("page2_on.png"),
		CC_CALLBACK_1(SelectLayer::onNextMap, this));
	auto start = MenuItemSprite::create(
		Sprite::createWithSpriteFrameName("start_btn.png"),
		Sprite::createWithSpriteFrameName("start_btn.png"),
		CC_CALLBACK_1(SelectLayer::onConfirmMap, this));
	auto back = MenuItemSprite::create(
		Sprite::createWithSpriteFrameName("return_btn.png"),
		Sprite::createWithSpriteFrameName("return_btn.png"),
		CC_CALLBACK_1(SelectLayer::onBackFromMapSelect, this));

	if (previous && next && start && back)
	{
		auto menu = Menu::create(previous, next, start, back, nullptr);
		menu->setPosition(Vec2::ZERO);
		previous->setPosition(Vec2(90, 42));
		next->setPosition(Vec2(winSize.width - 90, 42));
		start->setPosition(Vec2(winSize.width - 55, 38));
		back->setPosition(Vec2(45, winSize.height - 35));
		mapSelectOverlay->addChild(menu, 10);
	}
}

void SelectLayer::updateMapPreview()
{
	const int count = GameLayer::getMapCount();
	if (count <= 0) return;
	selectedMapId = MAX(1, MIN(selectedMapId, count));
	if (mapPreview)
	{
		mapPreview->removeFromParentAndCleanup(true);
		mapPreview = nullptr;
	}
	auto path = format("Maps/map_bg{}.png", selectedMapId);
	mapPreview = Sprite::create(path.c_str());
	if (mapPreview)
	{
		mapPreview->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
		const auto size = mapPreview->getContentSize();
		const float scale = MIN((winSize.width - 120) / size.width, (winSize.height - 150) / size.height);
		mapPreview->setScale(scale);
		mapSelectOverlay->addChild(mapPreview, 2);
	}
}

void SelectLayer::onPreviousMap(Ref *)
{
	const int count = GameLayer::getMapCount();
	if (count <= 0) return;
	selectedMapId = selectedMapId <= 1 ? count : selectedMapId - 1;
	updateMapPreview();
}

void SelectLayer::onNextMap(Ref *)
{
	const int count = GameLayer::getMapCount();
	if (count <= 0) return;
	selectedMapId = selectedMapId >= count ? 1 : selectedMapId + 1;
	updateMapPreview();
}

void SelectLayer::onConfirmMap(Ref *)
{
	onGameStart();
}

void SelectLayer::onBackFromMapSelect(Ref *)
{
	if (mapSelectOverlay)
	{
		mapSelectOverlay->removeFromParentAndCleanup(true);
		mapSelectOverlay = nullptr;
		mapPreview = nullptr;
	}
	for (auto child : getChildren())
		child->setVisible(true);
	isMapSelect = false;
}

void SelectLayer::onExitTransitionDidStart()
{
	Layer::onExitTransitionDidStart();
	setKeypadEnabled(false);
}

void SelectLayer::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
	setKeypadEnabled(true);
}

void SelectLayer::keyBackClicked()
{
	setKeypadEnabled(false);

	lua_call_func(UiFlowKeys::kBackToStartMenu);
}
