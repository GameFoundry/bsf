#include "BsScriptKerningPair.generated.h"
#include "BsMonoMethod.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace bs
{
	ScriptKerningPair::ScriptKerningPair(MonoObject* managedInstance)
		:ScriptObject(managedInstance)
	{ }

	void ScriptKerningPair::initRuntimeData()
	{ }

	MonoObject*ScriptKerningPair::box(const KerningPair& value)
	{
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	KerningPair ScriptKerningPair::unbox(MonoObject* value)
	{
		return *(KerningPair*)MonoUtil::unbox(value);
	}

}
