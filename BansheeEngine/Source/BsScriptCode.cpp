#include "BsScriptCode.h"
#include "BsResources.h"
#include "BsScriptCodeRTTI.h"

namespace BansheeEngine
{
	ScriptCode::ScriptCode(const WString& data)
		:Resource(false), mString(data)
	{

	}

	HScriptCode ScriptCode::create(const WString& data)
	{
		return static_resource_cast<ScriptCode>(gResources()._createResourceHandle(_createPtr(data)));
	}

	ScriptCodePtr ScriptCode::_createPtr(const WString& data)
	{
		ScriptCodePtr scriptCodePtr = bs_core_ptr<ScriptCode, PoolAlloc>(
			new (bs_alloc<ScriptCode, PoolAlloc>()) ScriptCode(data));
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