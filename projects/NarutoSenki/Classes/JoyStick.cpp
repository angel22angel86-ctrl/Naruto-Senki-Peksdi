#include "JoyStick.h"
#include "HudLayer.h"

bool JoyStick::init()
{
	RETURN_FALSE_IF(!Sprite::init());

	initWithSpriteFrameName("JoyStickBg.png");
	setAnchorPoint(Vec2(0, 0));

	_joyStickControl = Sprite::createWithSpriteFrameName("JoyStick.png");
	_joyStickControl->setAnchorPoint(Vec2(0, 0));

	_defaultPotion = Vec2(getContentSize().width / 2 - _joyStickControl->getContentSize().width / 2,
						  getContentSize().height / 2 - _joyStickControl->getContentSize().height / 2);

	_joyStickControl->setPosition(_defaultPotion);
	addChild(_joyStickControl);

	return true;
}

void JoyStick::onEnter()
{
	Sprite::onEnter();
	Director::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

void JoyStick::onExit()
{
	Sprite::onExit();
	Director::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

bool JoyStick::ccTouchBegan(Touch *touch, Event *event)
{
	Vec2 curPoint = touch->getLocation();
	// The joystick can be moved from the Network/Custom HUD menu, so its
	// touch area must follow the actual node instead of being hard-coded to
	// the lower-left half of the screen.
	CCRect touchRect = getBoundingBox();
	touchRect.origin.x -= 12.0f;
	touchRect.origin.y -= 12.0f;
	touchRect.size.width += 24.0f;
	touchRect.size.height += 24.0f;
	return touchRect.containsPoint(curPoint);
}

void JoyStick::ccTouchMoved(Touch *touch, Event *event)
{
	if (!_delegate->_isAllButtonLocked)
	{
		updateDirectionForTouchLocation(touch);
	}
}

void JoyStick::ccTouchEnded(Touch *touch, Event *event)
{
	if (!_delegate->_isAllButtonLocked)
	{
		_joyStickControl->setPosition(_defaultPotion);
		_delegate->JoyStickRelease();
	}
}

void JoyStick::updateDirectionForTouchLocation(Touch *touch)
{
	Vec2 startPoint = getPosition() + Vec2(
		getContentSize().width * getScale() / 2.0f,
		getContentSize().height * getScale() / 2.0f);
	Vec2 curPoint = touch->getLocation();
	Vec2 sp = curPoint - startPoint;
	float distance = sp.getLength();
	float radians = sp.getAngle();
	float degrees = -1 * CC_RADIANS_TO_DEGREES(radians);

	if (degrees <= 22.5 && degrees >= -22.5)
	{
		// right
		_direction = Vec2(1.0, 0.0);
	}
	else if (degrees > 22.5 && degrees < 67.5)
	{
		// bottom right
		_direction = Vec2(1.0, -1.0);
	}
	else if (degrees >= 67.5 && degrees <= 112.5)
	{
		// bottom
		_direction = Vec2(0.0, -1.0);
	}
	else if (degrees > 112.5 && degrees < 157.5)
	{
		// bottom left
		_direction = Vec2(-1.0, -1.0);
	}
	else if (degrees >= 157.5 || degrees <= -157.5)
	{
		// left
		_direction = Vec2(-1.0, 0.0);
	}
	else if (degrees < -22.5 && degrees > -67.5)
	{
		// top right
		_direction = Vec2(1.0, 1.0);
	}
	else if (degrees <= -67.5 && degrees >= -112.5)
	{
		// top
		_direction = Vec2(0.0, 1.0);
	}
	else if (degrees < -112.5 && degrees > -157.5)
	{
		// top left
		_direction = Vec2(-1.0, 1.0);
	};

	updateJoyStick(distance, _direction);
	_delegate->JoyStickUpdate(_direction);
}

void JoyStick::updateJoyStick(float distance, Vec2 direction)
{
	// Touch distance is measured in screen coordinates. Convert it to the
	// joystick's local coordinates when the user changed its scale.
	const float scale = getScale() > 0.0f ? getScale() : 1.0f;
	const float localDistance = distance / scale;
	if (localDistance < 27.5f)
	{
		_joyStickControl->setPosition(_defaultPotion + (direction * localDistance));
	} /*else if (localDistance > 96) {
		_joyStickControl->setPosition(_defaultPotion + (direction * 53));
	} */
	else
	{
		_joyStickControl->setPosition(_defaultPotion + (direction * 26.5f));
	}
}
