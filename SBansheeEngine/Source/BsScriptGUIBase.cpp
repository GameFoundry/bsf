#include "BsScriptGUIBase.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptGUIArea.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGUIBase::ScriptGUIBase()
	{

	}

	void ScriptGUIBaseMeta::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUIBase", &ScriptGUIBase::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}
}