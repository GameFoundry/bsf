#include "BsScriptCode.h"
#include "BsResources.h"
#include "BsScriptCodeRTTI.h"

namespace BansheeEngine
{
	ScriptCode::ScriptCode(const WString& data, bool editorScript)
		:Resource(false), mString(data), mEditorScript(editorScript)
	{

	}

	HScriptCode ScriptCode::create(const WString& data, bool editorScript)
	{
		return static_resource_cast<ScriptCode>(gResources()._createResourceHandle(_createPtr(data, editorScript)));
	}

	ScriptCodePtr ScriptCode::_createPtr(const WString& data, bool editorScript)
	{
		ScriptCodePtr scriptCodePtr = bs_core_ptr<ScriptCode>(
			new (bs_alloc<ScriptCode>()) ScriptCode(data, editorScript));
		scriptCodePtr->_setThisPtr(scriptCodePtr);
		scriptCodePtr->initialize();

		return scriptCodePtr;
	}

	RTTITypeBase* ScriptCode::getRTTIStatic()
	{
		return ScriptCodeRTTI::instance();
	}

	RTTITypeBase* ScriptCode::getRTTI() const
	{
		return ScriptCode::getRTTIStatic();
	}
}