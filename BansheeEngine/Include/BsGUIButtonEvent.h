#pragma once

#include "BsPrerequisites.h"
#include "CmInputFwd.h"
#include "CmInt2.h"

namespace BansheeEngine
{
	enum class GUIKeyEventType
	{
		KeyUp,
		KeyDown,
		TextInput
	};

	class BS_EXPORT GUIKeyEvent
	{
	public:
		GUIKeyEvent();
		GUIKeyEvent(bool shift, bool ctrl, bool alt);

		GUIKeyEventType getType() const { return mType; }
		CM::ButtonCode getKey() const { return mKey; }
		const CM::UINT32& getInputChar() const { return mInputChar; }

		bool isShiftDown() const { return mShift; }
		bool isCtrlDown() const { return mCtrl; }
		bool isAltDown() const { return mAlt; }
	private:
		friend class GUIManager;

		GUIKeyEventType mType;
		CM::ButtonCode mKey;
		CM::UINT32 mInputChar;

		bool mShift;
		bool mCtrl;
		bool mAlt;

		void setKeyDownData(CM::ButtonCode key);
		void setKeyUpData(CM::ButtonCode key);
		void setTextInputData(CM::UINT32 inputChar);
	};
}