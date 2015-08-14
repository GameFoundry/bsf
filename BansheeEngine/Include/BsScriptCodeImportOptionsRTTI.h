#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsScriptCodeImportOptions.h"

namespace BansheeEngine
{
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

		virtual const String& getRTTIName() override
		{
			static String name = "ScriptCodeImportOptions";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_ScriptCodeImportOptions;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<ScriptCodeImportOptions>();
		}
	};
}