#include "BsScriptCodeImportOptions.h"
#include "BsScriptCodeImportOptionsRTTI.h"

namespace BansheeEngine
{
	ScriptCodeImportOptions::ScriptCodeImportOptions()
		:mEditorScript(false)
	{

	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* ScriptCodeImportOptions::getRTTIStatic()
	{
		return ScriptCodeImportOptionsRTTI::instance();
	}

	RTTITypeBase* ScriptCodeImportOptions::getRTTI() const
	{
		return ScriptCodeImportOptions::getRTTIStatic();
	}
}