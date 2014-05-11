#pragma once

#include "BsMonoPrerequisites.h"
#include "BsScriptManager.h"
#include "CmModule.h"

namespace BansheeEngine 
{
	class BS_MONO_EXPORT MonoScriptSystem : public ScriptSystem
	{
	public:
		MonoScriptSystem();
		~MonoScriptSystem();

		virtual void initialize();
		virtual void destroy();

	private:
		bool mIsInitialized;
	};
}