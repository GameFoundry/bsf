#pragma once

#include "BsEditorPrerequisites.h"
#include "BsSettings.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains various globally accessible project-specific preferences.
	 */
	class BS_ED_EXPORT ProjectSettings : public Settings
	{
	public:
		ProjectSettings();

		/**
		 * @brief	Retrieves the UUID of the last scene open in the editor.
		 */
		String getLastOpenScene() const { return mLastOpenScene; }

		/**
		 * @brief	Sets the UUID of the last scene open in the editor.
		 */
		void setLastOpenScene(const String& value) { mLastOpenScene = value; markAsDirty(); }

	private:
		String mLastOpenScene;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ProjectSettingsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}