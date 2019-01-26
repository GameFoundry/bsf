#include "BsScriptTextureSurface.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace bs
{
	ScriptTextureSurface::ScriptTextureSurface(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	void ScriptTextureSurface::initRuntimeData()
	{ }

	MonoObject*ScriptTextureSurface::box(const TextureSurface& value)
	{
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	TextureSurface ScriptTextureSurface::unbox(MonoObject* value)
	{
		return *(TextureSurface*)MonoUtil::unbox(value);
	}

}
