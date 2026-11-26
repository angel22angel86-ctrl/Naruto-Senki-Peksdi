#pragma once
#include "LoadLayer.h"

class SelectLayer : public Layer
{
public:
	~SelectLayer();

	const char *_selectHero = nullptr;
	const char *_playerSelect = nullptr;
	const char *_com1Select = nullptr;
	const char *_com2Select = nullptr;
	const char *_com3Select = nullptr;
	CCArray *_selectList = nullptr;

	void setSelectHero(const char *var) { _playerSelect = var; }
	void setCom1Select(const char *var) { _com1Select = var; }
	void setCom2Select(const char *var) { _com2Select = var; }
	void setCom3Select(const char *var) { _com3Select = var; }
	void setSelectList(CCArray *var) { _selectList = var; }

	void onGameStart();
	void onPreviousMap(Ref *sender);
	void onNextMap(Ref *sender);
	void onConfirmMap(Ref *sender);
	void onBackFromMapSelect(Ref *sender);
	void showMapSelect();
	void updateMapPreview();
	void setPlayerSelected(const char *charName) {}

	CREATE_FUNC(SelectLayer);

private:
	void onExitTransitionDidStart();
	void onEnterTransitionDidFinish();
	void keyBackClicked();

	bool isStart = false;
	bool isMapSelect = false;
	int selectedMapId = 1;
	Node *mapSelectOverlay = nullptr;
	Sprite *mapPreview = nullptr;
};
