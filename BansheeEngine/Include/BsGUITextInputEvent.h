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

		const CM::UINT32& getInputChar() const { return mInputChar; }
	private:
		friend class GUIManager;

		CM::UINT32 mInputChar;

		void setData(CM::UINT32 inputChar);
	};
}