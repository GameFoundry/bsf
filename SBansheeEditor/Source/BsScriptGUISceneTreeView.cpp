#include "BsScriptGUISceneTreeView.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsGUISceneTreeView.h"
#include "BsGUIOptions.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUISceneTreeView::ScriptGUISceneTreeView(MonoObject* instance, GUISceneTreeView* treeView)
		:TScriptGUIElement(instance, treeView)
	{

	}

	void ScriptGUISceneTreeView::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUISceneTreeView::internal_createInstance);
	}

	void ScriptGUISceneTreeView::internal_createInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		String styleName = toString(MonoUtil::monoToWString(style));

		GUISceneTreeView* treeView = GUISceneTreeView::create(options);
		ScriptGUISceneTreeView* nativeInstance = new (bs_alloc<ScriptGUISceneTreeView>()) ScriptGUISceneTreeView(instance, treeView);
	}
}