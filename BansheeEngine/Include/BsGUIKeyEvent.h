#pragma once

#include "BsPrerequisites.h"
#include "CmInputHandler.h"
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

		GUIKeyEventType getType() const { return mType; }
		CM::KeyCode getKey() const { return mKey; }
		const CM::UINT32& getInputChar() const { return mInputChar; }
	private:
		friend class GUIManager;

		GUIKeyEventType mType;
		CM::KeyCode mKey;
		CM::UINT32 mInputChar;

		void setKeyDownData(CM::KeyCode key);
		void setKeyUpData(CM::KeyCode key);
		void setTextInputData(CM::UINT32 inputChar);
	};
}