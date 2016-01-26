//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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
		 * @brief	Gets the source code contained in the resource.
		 */
		const WString& getString() const { return mString; }

		/**
		 * @brief	Sets the source code contained in the resource.
		 */
		void setString(const WString& data) { mString = data; }

		/**
		 * @brief	Gets a value that determines should the script code be compiled with editor assemblies.
		 */
		bool getIsEditorScript() const { return mEditorScript; }

		/**
		 * @brief	Sets a value that determines should the script code be compiled with editor assemblies.
		 */
		void setIsEditorScript(bool editorScript) { mEditorScript = editorScript; }

		/**
		 * @brief	Creates a new script code resource with the specified source code.
		 */
		static HScriptCode create(const WString& data, bool editorScript = false);

		/**
		 * @brief	Creates a new scriptcode resource with the specified source string.
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
		virtual RTTITypeBase* getRTTI() const override;
	};
}