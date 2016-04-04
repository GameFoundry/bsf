//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsScriptCodeImportOptions.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

	class BS_EXPORT ScriptCodeImportOptionsRTTI : public RTTIType <ScriptCodeImportOptions, ImportOptions, ScriptCodeImportOptionsRTTI>
	{
	private:
		bool& getEditorScript(ScriptCodeImportOptions* obj) { return obj->mEditorScript; }
		void setEditorScript(ScriptCodeImportOptions* obj, bool& val) { obj->mEditorScript = val; }
	public:
		ScriptCodeImportOptionsRTTI()
		{
			addPlainField("mEditorScript", 0, &ScriptCodeImportOptionsRTTI::getEditorScript, &ScriptCodeImportOptionsRTTI::setEditorScript);
		}

		const String& getRTTIName() override
		{
			static String name = "ScriptCodeImportOptions";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ScriptCodeImportOptions;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<ScriptCodeImportOptions>();
		}
	};

	/** @} */
	/** @endcond */
}