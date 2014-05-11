#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGameObject : public ScriptObjectBase
	{
	public:
		ScriptGameObject(MonoObject* instance)
			:ScriptObjectBase(instance)
		{ }
		virtual ~ScriptGameObject() { }

		virtual HGameObject getNativeHandle() const = 0;
		virtual void setNativeHandle(const HGameObject& gameObject) = 0;
	};
}