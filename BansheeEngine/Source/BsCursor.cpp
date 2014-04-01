#include "BsCursor.h"
#include "CmPlatform.h"
#include "CmException.h"
#include "BsBuiltinResources.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	Cursor::Cursor()
		:mActiveCursorId(0), mNextUniqueId((UINT32)CursorType::Count)
	{
		for(UINT32 i = 0; i < (UINT32)CursorType::Count; i++)
			restoreCursorIcon((CursorType)i);

		setCursor(CursorType::Arrow);
	}

	void Cursor::move(const Vector2I& screenPos)
	{
		Platform::setCursorPosition(screenPos);
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

	void Cursor::clipToRect(const RectI& screenRect)
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
		CustomIcon& customIcon = mCustomIcons[id];

		Platform::setCursor(customIcon.pixelData, customIcon.hotSpot);

		mActiveCursorId = id;
	}

	void Cursor::setCursor(const CM::String& name)
	{
		auto iterFind = mCustomIconNameToId.find(name);
		if(iterFind == mCustomIconNameToId.end())
		{
			LOGWRN("Cannot find cursor icon with name: " + name);
			return;
		}

		UINT32 id = iterFind->second;
		CustomIcon& customIcon = mCustomIcons[id];
		Platform::setCursor(customIcon.pixelData, customIcon.hotSpot);

		mActiveCursorId = id;
	}

	void Cursor::setCursorIcon(const CM::String& name, const CM::PixelData& pixelData, const CM::Vector2I& hotSpot)
	{
		auto iterFind = mCustomIconNameToId.find(name);
		if(iterFind != mCustomIconNameToId.end())
		{
			UINT32 id = iterFind->second;
			mCustomIcons[id] = CustomIcon(pixelData, hotSpot);

			if(mActiveCursorId == id)
				setCursor(name); // Refresh active
		}
		else
		{
			UINT32 id = mNextUniqueId++;
			mCustomIconNameToId[name] = id;
			mCustomIcons[id] = CustomIcon(pixelData, hotSpot);
		}
	}

	void Cursor::setCursorIcon(CursorType type, const CM::PixelData& pixelData, const CM::Vector2I& hotSpot)
	{
		UINT32 id = (UINT32)type;

		mCustomIcons[id].pixelData = pixelData;
		mCustomIcons[id].hotSpot = hotSpot;

		if(mActiveCursorId == id)
			setCursor(type); // Refresh active
	}

	void Cursor::clearCursorIcon(const CM::String& name)
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

		if(mActiveCursorId == (UINT32)type)
			setCursor(type); // Refresh active
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
		}

		CM_EXCEPT(InvalidParametersException, "Invalid cursor type: " + toString((UINT32)type));
	}
}