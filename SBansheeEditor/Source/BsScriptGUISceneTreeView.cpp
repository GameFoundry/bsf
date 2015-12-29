#include "BsScriptGUISceneTreeView.h"
#include "BsScriptMeta.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsGUISceneTreeView.h"
#include "BsGUIOptions.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptSceneObject.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUISceneTreeView::OnModifiedThunkDef ScriptGUISceneTreeView::onModifiedThunk;
	ScriptGUISceneTreeView::OnResourceDroppedThunkDef ScriptGUISceneTreeView::onResourceDroppedThunk;

	ScriptGUISceneTreeView::ScriptGUISceneTreeView(MonoObject* instance, GUISceneTreeView* treeView)
		:TScriptGUIElement(instance, treeView)
	{
		mOnModifiedConn = treeView->onModified.connect(std::bind(&ScriptGUISceneTreeView::sceneModified, this));
		mOnResourceDroppedConn = treeView->onResourceDropped.connect(
			std::bind(&ScriptGUISceneTreeView::resourceDropped, this, _1, _2));
	}

	ScriptGUISceneTreeView::~ScriptGUISceneTreeView()
	{
		mOnModifiedConn.disconnect();
		mOnResourceDroppedConn.disconnect();
	}

	void ScriptGUISceneTreeView::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUISceneTreeView::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_Update", &ScriptGUISceneTreeView::internal_update);

		onModifiedThunk = (OnModifiedThunkDef)metaData.scriptClass->getMethod("Internal_DoOnModified", 0)->getThunk();
		onResourceDroppedThunk = (OnResourceDroppedThunkDef)metaData.scriptClass->getMethod("Internal_DoOnResourceDropped", 2)->getThunk();
	}

	void ScriptGUISceneTreeView::sceneModified()
	{
		MonoUtil::invokeThunk(onModifiedThunk, getManagedInstance());
	}

	void ScriptGUISceneTreeView::resourceDropped(const HSceneObject& parent, const Vector<Path>& resourcePaths)
	{
		MonoObject* sceneMonoObject = nullptr;

		if (parent != nullptr)
		{
			ScriptSceneObject* scriptSceneObject = ScriptGameObjectManager::instance().getOrCreateScriptSceneObject(parent);
			sceneMonoObject = scriptSceneObject->getManagedInstance();
		}

		UINT32 numPaths = (UINT32)resourcePaths.size();
		ScriptArray array = ScriptArray::create<WString>(numPaths);
		for (UINT32 i = 0; i < numPaths; i++)
			array.set(i, resourcePaths[i].toWString());

		MonoUtil::invokeThunk(onResourceDroppedThunk, getManagedInstance(), sceneMonoObject, array.getInternal());
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

	void ScriptGUISceneTreeView::internal_update(ScriptGUISceneTreeView* thisPtr)
	{
		if (thisPtr->mIsDestroyed)
			return;

		GUISceneTreeView* treeView = static_cast<GUISceneTreeView*>(thisPtr->getGUIElement());
		treeView->_update();
	}
}