#pragma once

#include "BsScriptEnginePrerequisites.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGameObject
	{
	public:
		virtual HGameObject getNativeHandle() const = 0;
		virtual void setNativeHandle(const HGameObject& gameObject) = 0;
	};
}