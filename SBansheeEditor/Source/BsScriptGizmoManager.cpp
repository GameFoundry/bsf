#include "BsScriptGizmoManager.h"
#include "BsRuntimeScriptObjects.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoField.h"
#include "BsMonoManager.h"
#include "BsSceneManager.h"
#include "BsSceneObject.h"
#include "BsComponent.h"
#include "BsManagedComponent.h"
#include "BsGizmoManager.h"
#include "BsSelection.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGizmoManager::ScriptGizmoManager(RuntimeScriptObjects& scriptObjectManager)
		:mScriptObjectManager(scriptObjectManager), mDrawGizmoAttribute(nullptr), mNextAssemblyId(0), mFlagsField(nullptr)
	{
		mAssemblyRefreshedConn = mScriptObjectManager.onAssemblyRefreshed.connect(std::bind(&ScriptGizmoManager::reloadAssemblyMethods, this, _1));

		Vector<String> initializedAssemblyNames = mScriptObjectManager.getInitializedAssemblies();
		for (auto& assemblyName : initializedAssemblyNames)
		{
			MonoAssembly* assembly = MonoManager::instance().getAssembly(assemblyName);
			if (assembly != nullptr)
				reloadAssemblyMethods(assembly);
		}
	}

	ScriptGizmoManager::~ScriptGizmoManager()
	{
		mAssemblyRefreshedConn.disconnect();
	}

	bool dummyIsSelected(const HSceneObject& so)
	{
		return false; // TODO - Replace with a call to Selection class once I have it
	}

	void ScriptGizmoManager::update()
	{
		GizmoManager::instance().clearGizmos();

		HSceneObject rootSO = SceneManager::instance().getRootNode();

		Stack<HSceneObject> todo;
		todo.push(rootSO);

		bool isParentSelected = false;
		UINT32 parentSelectedPopIdx = 0;
		
		Vector<HSceneObject> selectedObjects = Selection::instance().getSceneObjects();

		while (!todo.empty())
		{
			if (isParentSelected && parentSelectedPopIdx == (UINT32)todo.size())
			{
				isParentSelected = false;
			}

			HSceneObject curSO = todo.top();
			todo.pop();

			bool isSelected = std::count(selectedObjects.begin(), selectedObjects.end(), curSO) > 0;
			if (isSelected && !isParentSelected)
			{
				isParentSelected = true;
				parentSelectedPopIdx = (UINT32)todo.size();
			}

			const Vector<HComponent>& components = curSO->getComponents();
			for (auto& component : components)
			{
				if (rtti_is_of_type<ManagedComponent>(component.get()))
				{
					ManagedComponent* managedComponent = static_cast<ManagedComponent*>(component.get());

					auto iterFind = mGizmoDrawers.find(managedComponent->getManagedFullTypeName());
					if (iterFind != mGizmoDrawers.end())
					{
						UINT32 flags = iterFind->second.flags;

						bool drawGizmo = false;
						if (((flags & (UINT32)DrawGizmoFlags::Selected) != 0) && isSelected)
							drawGizmo = true;

						if (((flags & (UINT32)DrawGizmoFlags::ParentSelected) != 0) && isParentSelected)
							drawGizmo = true;

						if (((flags & (UINT32)DrawGizmoFlags::NotSelected) != 0) && !isSelected && !isParentSelected)
							drawGizmo = true;

						if (drawGizmo)
						{
							bool pickable = (flags & (UINT32)DrawGizmoFlags::Pickable) != 0;
							GizmoManager::instance().startGizmo(curSO);
							GizmoManager::instance().setPickable(pickable);

							void* params[1] = { managedComponent->getManagedInstance() };
							iterFind->second.drawGizmosMethod->invoke(nullptr, params);

							GizmoManager::instance().endGizmo();
						}
					}
				}
			}

			for (UINT32 i = 0; i < curSO->getNumChildren(); i++)
				todo.push(curSO->getChild(i));
		}
	}

	void ScriptGizmoManager::reloadAssemblyMethods(MonoAssembly* assembly)
	{
		String assemblyName = assembly->getName();

		// If editor assembly, reload DrawGizmo attribute
		if (assemblyName == BansheeEditorAssemblyName) 
		{
			mDrawGizmoAttribute = assembly->getClass("BansheeEditor", "DrawGizmo");
			if (mDrawGizmoAttribute == nullptr)
				BS_EXCEPT(InvalidStateException, "Cannot find DrawGizmo managed class.");

			mFlagsField = mDrawGizmoAttribute->getField("flags");

			// Load delayed assemblies first
			for (auto iter = mDelayedLoad.rbegin(); iter != mDelayedLoad.rend(); ++iter)
				reloadAssemblyMethods(*iter);

			mDelayedLoad.clear();
		}
		else
		{
			// If we haven't loaded editor assembly yet, wait until we do before continuing
			if (mDrawGizmoAttribute == nullptr)
			{
				mDelayedLoad.push_back(assembly);
				return;
			}
		}

		// If we had this assembly previously loaded, clear all of its methods
		UINT32 assemblyId = 0;
		auto iterFind = mAssemblyNameToId.find(assemblyName);
		if (iterFind != mAssemblyNameToId.end())
		{
			assemblyId = iterFind->second;

			Map<String, GizmoData> newGizmoDrawers;
			for (auto& gizmoMethod : mGizmoDrawers)
			{
				if (gizmoMethod.second.assemblyId != assemblyId)
					newGizmoDrawers[gizmoMethod.first] = gizmoMethod.second;
			}

			mGizmoDrawers.swap(newGizmoDrawers);
		}
		else
		{
			assemblyId = mNextAssemblyId++;
			mAssemblyNameToId[assemblyName] = assemblyId;
		}

		// Find new gizmo drawer methods
		const Vector<MonoClass*>& allClasses = assembly->getAllClasses();
		for (auto curClass : allClasses)
		{
			const Vector<MonoMethod*>& methods = curClass->getAllMethods();
			for (auto& curMethod : methods)
			{
				UINT32 drawGizmoFlags = 0;
				MonoClass* componentType = nullptr;
				if (isValidDrawGizmoMethod(curMethod, componentType, drawGizmoFlags))
				{
					String fullComponentName = componentType->getFullName();
					GizmoData& newGizmoData = mGizmoDrawers[fullComponentName];

					newGizmoData.assemblyId = assemblyId;
					newGizmoData.componentType = componentType;
					newGizmoData.drawGizmosMethod = curMethod;
					newGizmoData.flags = drawGizmoFlags;
				}
			}
		}
	}

	bool ScriptGizmoManager::isValidDrawGizmoMethod(MonoMethod* method, MonoClass*& componentType, UINT32& drawGizmoFlags)
	{
		componentType = nullptr;
		drawGizmoFlags = 0;

		if (!method->hasAttribute(mDrawGizmoAttribute))
			return false;

		if (method->getNumParameters() != 1)
			return false;

		if (!method->isStatic())
			return false;

		MonoClass* paramType = method->getParameterType(0);
		MonoClass* componentClass = mScriptObjectManager.getComponentClass();

		if (!paramType->isSubClassOf(componentClass))
			return false;

		componentType = paramType;

		MonoObject* drawGizmoAttrib = method->getAttribute(mDrawGizmoAttribute);
		mFlagsField->getValue(drawGizmoAttrib, &drawGizmoFlags);

		return true;
	}
}