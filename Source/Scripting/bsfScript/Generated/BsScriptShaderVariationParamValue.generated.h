//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../Foundation/bsfCore/Material/BsShader.h"

namespace bs
{
	struct __ShaderVariationParamValueInterop
	{
		MonoString* name;
		int32_t value;
	};

	class BS_SCR_BE_EXPORT ScriptShaderVariationParamValue : public ScriptObject<ScriptShaderVariationParamValue>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "ShaderVariationParamValue")

		static MonoObject* box(const __ShaderVariationParamValueInterop& value);
		static __ShaderVariationParamValueInterop unbox(MonoObject* value);
		static ShaderVariationParamValue fromInterop(const __ShaderVariationParamValueInterop& value);
		static __ShaderVariationParamValueInterop toInterop(const ShaderVariationParamValue& value);

	private:
		ScriptShaderVariationParamValue(MonoObject* managedInstance);

	};
}
