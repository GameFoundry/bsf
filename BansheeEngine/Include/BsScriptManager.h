#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"

namespace BansheeEngine 
{
	class BS_EXPORT ScriptSystem
	{
	public:
		virtual ~ScriptSystem() { }

		virtual void initialize() = 0;
	};

	class BS_EXPORT ScriptManager : public Module<ScriptManager>
	{
	public:
		ScriptManager() { }
		~ScriptManager() { }

		void initialize(const std::shared_ptr<ScriptSystem>& scriptSystem);

	private:
		std::shared_ptr<ScriptSystem> mScriptSystem;

		void onShutDown();
	};
}