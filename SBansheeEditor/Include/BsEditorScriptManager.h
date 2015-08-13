#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles all editor script modules, updates editor windows and similar.
	 */
	class BS_SCR_BED_EXPORT EditorScriptManager : public Module<EditorScriptManager>
	{
	public:
		EditorScriptManager();
		~EditorScriptManager();

		/**
		 * @brief	Called every frame. Updates editor windows and script modules.
		 *
		 * @note	Internal method.
		 */
		void update();

	private:
		/**
		 * @brief	Triggers Program::OnInitialize callback. Should be called
		 *			after startup and after assembly reload.
		 */
		void triggerOnInitialize();

		/**
		 * @brief	Triggered when an assembly refreshed finished.
		 */
		void onAssemblyRefreshDone();

		/**
		 * @brief	Loads all managed types and methods used by this module.
		 */
		void loadMonoTypes();

		/**
		 * @brief	Triggers assembly refresh on next ::update.
		 */
		static void debug_refreshAssembly();

		static const float EDITOR_UPDATE_RATE;

		MonoAssembly* mEditorAssembly;
		float mLastUpdateTime;

		MonoClass* mProgramEdClass;
		MonoMethod* mUpdateMethod;

		HEvent mOnDomainLoadConn;
		HEvent mOnAssemblyRefreshDoneConn;

		static bool mDebugRefresh;
	};
}