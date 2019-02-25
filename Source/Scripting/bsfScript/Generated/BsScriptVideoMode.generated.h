//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../Foundation/bsfCore/RenderAPI/BsVideoModeInfo.h"

namespace bs
{
#if !BS_IS_BANSHEE3D
	struct __VideoModeInterop
	{
		uint32_t width;
		uint32_t height;
		float refreshRate;
		uint32_t outputIdx;
		bool isCustom;
	};

	class BS_SCR_BE_EXPORT ScriptVideoMode : public ScriptObject<ScriptVideoMode>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "VideoMode")

		static MonoObject* box(const __VideoModeInterop& value);
		static __VideoModeInterop unbox(MonoObject* value);
		static VideoMode fromInterop(const __VideoModeInterop& value);
		static __VideoModeInterop toInterop(const VideoMode& value);

	private:
		ScriptVideoMode(MonoObject* managedInstance);

	};
#endif
}
