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
		const CM::WString& getInputString() const { return mInputString; }
	private:
		friend class GUIManager;

		GUIKeyEventType mType;
		CM::KeyCode mKey;
		CM::WString mInputString;

		void setKeyDownData(CM::KeyCode key);
		void setKeyUpData(CM::KeyCode key);
		void setTextInputData(const CM::WString& string);
	};
}