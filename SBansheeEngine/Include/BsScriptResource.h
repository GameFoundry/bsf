#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptResource : public ScriptObjectBase
	{
	public:
		virtual HResource getNativeHandle() const = 0;
		virtual void setNativeHandle(const HResource& resource) = 0;

	protected:
		friend class ScriptResourceManager;

		ScriptResource(MonoObject* instance)
			:ScriptObjectBase(instance)
		{ }
		virtual ~ScriptResource() {}
	};
}