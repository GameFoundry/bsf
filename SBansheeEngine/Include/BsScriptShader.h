#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsShader.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptShader : public ScriptObject <ScriptShader, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Shader")

		HResource getNativeHandle() const { return mShader; }
		void setNativeHandle(const HResource& resource);

		HShader getShaderHandle() const { return mShader; }
	private:
		friend class ScriptResourceManager;

		static void internal_GetShaderParameters(ScriptShader* nativeInstance, MonoArray** outNames, MonoArray** outTypes);

		ScriptShader(MonoObject* instance, const HShader& shader);

		void _onManagedInstanceDeleted();

		HShader mShader;
	};
}