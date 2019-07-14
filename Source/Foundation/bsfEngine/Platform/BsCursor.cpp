//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Platform/BsCursor.h"
#include "Platform/BsPlatform.h"
#include "Error/BsException.h"
#include "Resources/BsBuiltinResources.h"
#include "Debug/BsDebug.h"

namespace bs
{
	Cursor::Cursor()
	{
		for(UINT32 i = 0; i < (UINT32)CursorType::Count; i++)
			restoreCursorIcon((CursorType)i);
	}

	void Cursor::setScreenPosition(const Vector2I& screenPos)
	{
		Platform::setCursorPosition(screenPos);
	}

	Vector2I Cursor::getScreenPosition()
	{
		return Platform::getCursorPosition();
	}

	void Cursor::hide()
	{
		Platform::hideCursor();
	}

	void Cursor::show()
	{
		Platform::showCursor();
	}

	void Cursor::clipToWindow(const RenderWindow& window)
	{
		Platform::clipCursorToWindow(window);
	}

	void Cursor::clipToRect(const Rect2I& screenRect)
	{
		Platform::clipCursorToRect(screenRect);
	}

	void Cursor::clipDisable()
	{
		Platform::clipCursorDisable();
	}

	void Cursor::setCursor(CursorType type)
	{
		UINT32 id = (UINT32)type;
		if((UINT32)mActiveCursorId != id)
		{
			mActiveCursorId = id;
			updateCursorImage();
		}
	}

	void Cursor::setCursor(const String& name)
	{
		auto iterFind = mCustomIconNameToId.find(name);
		if(iterFind == mCustomIconNameToId.end())
		{
			BS_LOG(Warning, Platform, "Cannot find cursor icon with name: " + name);
			return;
		}

		UINT32 id = iterFind->second;
		if((UINT32)mActiveCursorId != id)
		{
			mActiveCursorId = id;
			updateCursorImage();
		}
	}

	void Cursor::setCursorIcon(const String& name, const PixelData& pixelData, const Vector2I& hotSpot)
	{
		auto iterFind = mCustomIconNameToId.find(name);
		if(iterFind != mCustomIconNameToId.end())
		{
			UINT32 id = iterFind->second;
			mCustomIcons[id] = CustomIcon(pixelData, hotSpot);

			if((UINT32)mActiveCursorId == id)
				updateCursorImage(); // Refresh active
		}
		else
		{
			UINT32 id = mNextUniqueId++;
			mCustomIconNameToId[name] = id;
			mCustomIcons[id] = CustomIcon(pixelData, hotSpot);
		}
	}

	void Cursor::setCursorIcon(CursorType type, const PixelData& pixelData, const Vector2I& hotSpot)
	{
		UINT32 id = (UINT32)type;

		mCustomIcons[id].pixelData = pixelData;
		mCustomIcons[id].hotSpot = hotSpot;

		if((UINT32)mActiveCursorId == id)
			updateCursorImage(); // Refresh active
	}

	void Cursor::clearCursorIcon(const String& name)
	{
		auto iterFind = mCustomIconNameToId.find(name);
		if(iterFind == mCustomIconNameToId.end())
			return;

		mCustomIcons.erase(iterFind->second);
		mCustomIconNameToId.erase(iterFind);
	}

	void Cursor::clearCursorIcon(CursorType type)
	{
		restoreCursorIcon(type);

		if(mActiveCursorId == (INT32)type)
			updateCursorImage(); // Refresh active
	}

	void Cursor::restoreCursorIcon(CursorType type)
	{
		UINT32 id = (UINT32)type;
		mCustomIcons[id] = CustomIcon();

		switch (type)
		{
		case CursorType::Arrow:
			mCustomIcons[id].pixelData = BuiltinResources::instance().getCursorArrow(mCustomIcons[id].hotSpot);
			return;
		case CursorType::Wait:
			mCustomIcons[id].pixelData = BuiltinResources::instance().getCursorWait(mCustomIcons[id].hotSpot);
			return;
		case CursorType::IBeam:
			mCustomIcons[id].pixelData = BuiltinResources::instance().getCursorIBeam(mCustomIcons[id].hotSpot);
			return;
		case CursorType::ArrowDrag:
			mCustomIcons[id].pixelData = BuiltinResources::instance().getCursorArrowDrag(mCustomIcons[id].hotSpot);
			return;
		case CursorType::SizeNESW:
			mCustomIcons[id].pixelData = BuiltinResources::instance().getCursorSizeNESW(mCustomIcons[id].hotSpot);
			return;
		case CursorType::SizeNS:
			mCustomIcons[id].pixelData = BuiltinResources::instance().getCursorSizeNS(mCustomIcons[id].hotSpot);
			return;
		case CursorType::SizeNWSE:
			mCustomIcons[id].pixelData = BuiltinResources::instance().getCursorSizeNWSE(mCustomIcons[id].hotSpot);
			return;
		case CursorType::SizeWE:
			mCustomIcons[id].pixelData = BuiltinResources::instance().getCursorSizeWE(mCustomIcons[id].hotSpot);
			return;
		case CursorType::Deny:
			mCustomIcons[id].pixelData = BuiltinResources::instance().getCursorDeny(mCustomIcons[id].hotSpot);
			return;
		case CursorType::ArrowLeftRight:
			mCustomIcons[id].pixelData = BuiltinResources::instance().getCursorMoveLeftRight(mCustomIcons[id].hotSpot);
			return;
		default:
			break;
		}

		BS_EXCEPT(InvalidParametersException, "Invalid cursor type: " + toString((UINT32)type));
	}

	void Cursor::updateCursorImage()
	{
		if (mActiveCursorId < 0)
			return;

		CustomIcon& customIcon = mCustomIcons[mActiveCursorId];
		Platform::setCursor(customIcon.pixelData, customIcon.hotSpot);
	}

	Cursor& gCursor()
	{
		return static_cast<Cursor&>(Cursor::instance());
	}
}
