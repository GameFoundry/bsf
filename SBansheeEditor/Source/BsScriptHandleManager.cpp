#include "BsScriptHandleManager.h"
#include "BsScriptAssemblyManager.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoField.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsSelection.h"
#include "BsSceneObject.h"
#include "BsManagedComponent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptHandleManager::ScriptHandleManager(ScriptAssemblyManager& scriptObjectManager)
		:mScriptObjectManager(scriptObjectManager)
	{
		mDomainLoadConn = MonoManager::instance().onDomainReload.connect(std::bind(&ScriptHandleManager::reloadAssemblyData, this));
		reloadAssemblyData();
	}

	ScriptHandleManager::~ScriptHandleManager()
	{
		for (auto& handle : mActiveHandleData.handles)
		{
			callDestroy(handle.object);
			mono_gchandle_free(handle.gcHandle);
		}

		if (mDefaultHandleManager != nullptr)
		{
			callDestroy(mDefaultHandleManager);
			mono_gchandle_free(mDefaultHandleManagerGCHandle);
		}

		mDomainLoadConn.disconnect();
	}

	void ScriptHandleManager::refreshHandles()
	{
		const Vector<HSceneObject>& selectedSOs = Selection::instance().getSceneObjects();

		HSceneObject newSelectedObject;
		if (selectedSOs.size() > 0)
		{
			// We only consider the first selected object for custom handles, multiple selection is not supported
			newSelectedObject = selectedSOs[0]; 
		}

		if (newSelectedObject != mActiveHandleData.selectedObject)
		{
			for (auto& handle : mActiveHandleData.handles)
			{
				callDestroy(handle.object);
				mono_gchandle_free(handle.gcHandle);
			}

			mActiveHandleData.selectedObject = newSelectedObject;
			mActiveHandleData.handles.clear();

			if (newSelectedObject)
			{
				const Vector<HComponent>& components = newSelectedObject->getComponents();

				for (auto& component : components)
				{
					if (!rtti_is_of_type<ManagedComponent>(component.getInternalPtr()))
						continue;

					HManagedComponent mc = static_object_cast<ManagedComponent>(component);
					const String& componentTypeName = mc->getManagedFullTypeName();

					auto iterFind = mHandles.find(componentTypeName);
					if (iterFind == mHandles.end())
						continue;

					CustomHandleData& handleData = iterFind->second;
					MonoObject* newHandleInstance = handleData.handleType->createInstance(false);

					void* params[1] = { mc->getManagedInstance() };
					handleData.ctor->invoke(newHandleInstance, params);

					mActiveHandleData.handles.push_back(ActiveCustomHandleData());
					ActiveCustomHandleData& newHandleData = mActiveHandleData.handles.back();

					newHandleData.object = newHandleInstance;
					newHandleData.gcHandle = mono_gchandle_new(newHandleInstance, false);
				}
			}
		}

		if (mDefaultHandleManager == nullptr)
		{
			mDefaultHandleManager = mDefaultHandleManagerClass->createInstance(true);
			mDefaultHandleManagerGCHandle = mono_gchandle_new(mDefaultHandleManager, false);
		}

		callPreInput(mDefaultHandleManager);

		for (auto& handle : mActiveHandleData.handles)
			callPreInput(handle.object);
	}

	void ScriptHandleManager::triggerHandles()
	{
		callPostInput(mDefaultHandleManager);

		for (auto& handle : mActiveHandleData.handles)
		{
			callPostInput(handle.object);
		}
	}

	void ScriptHandleManager::queueDrawCommands()
	{
		if (mDefaultHandleManager != nullptr)
			callDraw(mDefaultHandleManager);

		for (auto& handle : mActiveHandleData.handles)
		{
			callDraw(handle.object);
		}
	}

	void ScriptHandleManager::reloadAssemblyData()
	{
		// Reload types from editor assembly
		MonoAssembly* editorAssembly = MonoManager::instance().getAssembly(BansheeEditorAssemblyName);
		mCustomHandleAttribute = editorAssembly->getClass("BansheeEditor", "CustomHandle");
		if (mCustomHandleAttribute == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find CustomHandle managed class.");

		mHandleBaseClass = editorAssembly->getClass("BansheeEditor", "Handle");
		if (mHandleBaseClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find Handle managed class.");

		mDefaultHandleManagerClass = editorAssembly->getClass("BansheeEditor", "DefaultHandleManager");
		if (mDefaultHandleManagerClass == nullptr)
			BS_EXCEPT(InvalidStateException, "Cannot find DefaultHandleManager managed class.");

		mTypeField = mCustomHandleAttribute->getField("type");

		mPreInputMethod = mHandleBaseClass->getMethod("PreInput", 0);
		mPostInputMethod = mHandleBaseClass->getMethod("PostInput", 0);
		mDrawMethod = mHandleBaseClass->getMethod("Draw", 0);
		mDestroyThunk = (DestroyThunkDef)mHandleBaseClass->getMethod("Destroy", 0)->getThunk();

		mDefaultHandleManager = nullptr; // Freed on assembly unload, so not valid anymore
		mDefaultHandleManagerGCHandle = 0;

		Vector<String> scriptAssemblyNames = mScriptObjectManager.getScriptAssemblies();
		for (auto& assemblyName : scriptAssemblyNames)
		{
			MonoAssembly* assembly = MonoManager::instance().getAssembly(assemblyName);

			// Find new custom handle types
			const Vector<MonoClass*>& allClasses = assembly->getAllClasses();
			for (auto curClass : allClasses)
			{
				MonoClass* componentType = nullptr;
				MonoMethod* ctor = nullptr;

				if (isValidHandleType(curClass, componentType, ctor))
				{
					String fullComponentName = componentType->getFullName();

					CustomHandleData& newHandleData = mHandles[fullComponentName];

					newHandleData.componentType = componentType;
					newHandleData.handleType = curClass;
					newHandleData.ctor = ctor;
				}
			}
		}

		// Clear selection data
		mActiveHandleData.selectedObject = HSceneObject();
		mActiveHandleData.handles.clear();
	}

	bool ScriptHandleManager::isValidHandleType(MonoClass* type, MonoClass*& componentType, MonoMethod*& ctor)
	{
		componentType = nullptr;
		ctor = nullptr;

		if (!type->hasAttribute(mCustomHandleAttribute))
			return false;

		if (!type->isSubClassOf(mHandleBaseClass))
			return false;

		MonoObject* customHandleAttrib = type->getAttribute(mCustomHandleAttribute);
		MonoReflectionType* attribReflType = nullptr;

		mTypeField->getValue(customHandleAttrib, &attribReflType);
		MonoType* attribType = mono_reflection_type_get_type(attribReflType);
		::MonoClass* attribMonoClass = mono_class_from_mono_type(attribType);

		MonoClass* attribClass = MonoManager::instance().findClass(attribMonoClass);
		if (attribClass == nullptr)
			return false;

		MonoClass* componentClass = mScriptObjectManager.getComponentClass();
		if (!attribClass->isSubClassOf(componentClass))
			return false;
		
		MonoMethod* constructor = type->getMethod(".ctor", 1);
		if (constructor == nullptr)
			return false;

		MonoClass* paramType = constructor->getParameterType(0);
		if (paramType != attribClass)
			return false;

		componentType = paramType;
		ctor = constructor;

		return true;
	}

	void ScriptHandleManager::callPreInput(MonoObject* instance)
	{
		mPreInputMethod->invokeVirtual(instance, nullptr);
	}

	void ScriptHandleManager::callPostInput(MonoObject* instance)
	{
		mPostInputMethod->invokeVirtual(instance, nullptr);
	}

	void ScriptHandleManager::callDraw(MonoObject* instance)
	{
		mDrawMethod->invokeVirtual(instance, nullptr);
	}

	void ScriptHandleManager::callDestroy(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		mDestroyThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
	}
}