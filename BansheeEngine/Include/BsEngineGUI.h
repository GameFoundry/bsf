#pragma once

#include "BsPrerequisites.h"
#include "BsGUISkin.h"
#include "CmModule.h"

namespace BansheeEngine
{
	class BS_EXPORT EngineGUI : public CamelotFramework::Module<EngineGUI>
	{
	public:
		EngineGUI();

		const GUISkin& getSkin() const { return mSkin; }

	private:
		GUISkin mSkin;

		static const CM::String DefaultFontPath;
		static const UINT32 DefaultFontSize;
	};
}