//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptPostProcessSettings.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "BsCoreThread.h"

namespace BansheeEngine
{
	ScriptPostProcessSettings::ScriptPostProcessSettings(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptPostProcessSettings::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateDefault", &ScriptPostProcessSettings::internal_CreateDefault);

		// Validation
#if BS_DEBUG_MODE
		MonoAssembly* engineAssembly = MonoManager::instance().getAssembly(ENGINE_ASSEMBLY);

		MonoClass* autoExposureSettingsClass = engineAssembly->getClass("BansheeEngine", "AutoExposureSettings");
		MonoClass* tonemappingSettingsClass = engineAssembly->getClass("BansheeEngine", "TonemappingSettings");
		MonoClass* whieBalanceSettingsClass = engineAssembly->getClass("BansheeEngine", "WhiteBalanceSettings");
		MonoClass* colorGradingSettingsClass = engineAssembly->getClass("BansheeEngine", "ColorGradingSettings");

		assert(metaData.scriptClass->getInstanceSize() == sizeof(PostProcessSettings));
		assert(autoExposureSettingsClass->getInstanceSize() == sizeof(AutoExposureSettings));
		assert(tonemappingSettingsClass->getInstanceSize() == sizeof(TonemappingSettings));
		assert(whieBalanceSettingsClass->getInstanceSize() == sizeof(WhiteBalanceSettings));
		assert(colorGradingSettingsClass->getInstanceSize() == sizeof(ColorGradingSettings));
#endif
	}

	void ScriptPostProcessSettings::internal_CreateDefault(PostProcessSettings* output)
	{
		*output = PostProcessSettings();
	}
}