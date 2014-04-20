#pragma once

#include "BsScriptEnginePrerequisites.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptResource
	{
	public:
		virtual HResource getNativeHandle() const = 0;
		virtual void setNativeHandle(const HResource& resource) = 0;

	protected:
		friend class ScriptResourceManager;

		ScriptResource() {}
		virtual ~ScriptResource() {}
	};
}