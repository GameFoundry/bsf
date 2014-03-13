#pragma once

#include "BsScriptEnginePrerequisites.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptResource
	{
	public:
		virtual CM::HResource getNativeHandle() const = 0;
		virtual void setNativeHandle(const CM::HResource& resource) = 0;
	};
}