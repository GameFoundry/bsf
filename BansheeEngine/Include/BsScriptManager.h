#pragma once

#include "BsPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine 
{
	class BS_EXPORT ScriptSystem
	{
	public:
		virtual ~ScriptSystem() { }

		virtual void initialize() = 0;
		virtual void destroy() = 0;
	};

	class BS_EXPORT ScriptManager : public Module<ScriptManager>
	{
	public:
		ScriptManager() { }
		~ScriptManager() { }

		void initialize(const std::shared_ptr<ScriptSystem>& scriptSystem);
		void destroy();

	private:
		std::shared_ptr<ScriptSystem> mScriptSystem;

		void onShutDown();
	};
}