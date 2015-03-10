#pragma once

#include "BsPrerequisites.h"
#include "BsResource.h"

namespace BansheeEngine
{
	/**
	 * @brief	Resource containing script source code.
	 */
	class BS_EXPORT ScriptCode : public Resource
	{
	public:
		/**
		 * @brief	Returns the source code contained in the resource.
		 */
		const WString& getString() const { return mString; }

		/**
		 * @brief	Creates a new script code resource with the specified source code.
		 */
		static HScriptCode create(const WString& data, bool editorScript = false);

		/**
		 * @brief	Creates an include file resource with the specified include string.
		 *
		 * @note	Internal method. Use "create" for normal use.
		 */
		static ScriptCodePtr _createPtr(const WString& data, bool editorScript = false);
	private:
		ScriptCode(const WString& data, bool editorScript);

		WString mString;
		bool mEditorScript;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class ScriptCodeRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}