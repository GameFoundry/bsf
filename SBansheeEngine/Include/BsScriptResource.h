#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptResourceBase : public ScriptObjectBase
	{
	public:
		virtual HResource getNativeHandle() const = 0;
		virtual void setNativeHandle(const HResource& resource) = 0;

	protected:
		friend class ScriptResourceManager;

		ScriptResourceBase(MonoObject* instance)
			:ScriptObjectBase(instance)
		{ }
		virtual ~ScriptResourceBase() {}
	};

	class BS_SCR_BE_EXPORT ScriptResource : public ScriptObject<ScriptResource, ScriptResourceBase>
	{
	public:
		static String getAssemblyName() { return BansheeEngineAssemblyName; }
		static String getNamespace() { return "BansheeEngine"; }
		static String getTypeName() { return "Resource"; }
		static void initRuntimeData() { }

	private:
		ScriptResource(MonoObject* instance)
			:ScriptObject(instance)
		{ }
	};
}