#pragma once

#include "BsPrerequisites.h"
#include "BsImportOptions.h"
#include "BsScriptCode.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains import options you may use to control how is a file containing
	 *			script source code importer.
	 */
	class BS_EXPORT ScriptCodeImportOptions : public ImportOptions
	{
	public:
		ScriptCodeImportOptions();

		/**
		* @brief	Sets whether the script is editor-only or a normal game script.
		*/
		void setEditorScript(bool editorScript) { mEditorScript = editorScript; }

		/**
		 * @brief	Checks whether the script is editor-only or a normal game script.
		 */
		bool isEditorScript() const { return mEditorScript; }

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class ScriptCodeImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	private:
		bool mEditorScript;
	};
}