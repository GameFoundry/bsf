#pragma once

#include "BsPrerequisites.h"
#include "CmInputFwd.h"
#include "CmVector2I.h"

namespace BansheeEngine
{
	class BS_EXPORT GUITextInputEvent
	{
	public:
		GUITextInputEvent();

		const UINT32& getInputChar() const { return mInputChar; }
	private:
		friend class GUIManager;

		UINT32 mInputChar;

		void setData(UINT32 inputChar);
	};
}