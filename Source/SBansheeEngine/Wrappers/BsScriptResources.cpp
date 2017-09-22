//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Wrappers/BsScriptResources.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "Resources/BsResources.h"
#include "Resources/BsGameResourceManager.h"
#include "BsScriptResourceManager.h"
#include "Wrappers/BsScriptResource.h"
#include "Wrappers/BsScriptResourceRef.h"
#include "BsApplication.h"

namespace bs
{
	ScriptResources::ScriptResources(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptResources::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Load", (void*)&ScriptResources::internal_Load);
		metaData.scriptClass->addInternalCall("Internal_LoadRef", (void*)&ScriptResources::internal_LoadRef);
		metaData.scriptClass->addInternalCall("Internal_UnloadUnused", (void*)&ScriptResources::internal_UnloadUnused);
		metaData.scriptClass->addInternalCall("Internal_Release", (void*)&ScriptResources::internal_Release);
		metaData.scriptClass->addInternalCall("Internal_ReleaseRef", (void*)&ScriptResources::internal_ReleaseRef);
	}

	MonoObject* ScriptResources::internal_Load(MonoString* path, bool keepLoaded)
	{
		Path nativePath = MonoUtil::monoToWString(path);

		HResource resource = GameResourceManager::instance().load(nativePath, keepLoaded);
		if (resource == nullptr)
			return nullptr;

		ScriptResourceBase* scriptResource = ScriptResourceManager::instance().getScriptResource(resource, true);
		return scriptResource->getManagedInstance();
	}

	MonoObject* ScriptResources::internal_LoadRef(MonoObject* reference, bool keepLoaded)
	{
		ScriptResourceRef* scriptRef = ScriptResourceRef::toNative(reference);
		if (scriptRef == nullptr)
			return nullptr;

		ResourceLoadFlags loadFlags = ResourceLoadFlag::LoadDependencies;
		if (keepLoaded)
			loadFlags |= ResourceLoadFlag::KeepInternalRef;

		if (gApplication().isEditor())
			loadFlags |= ResourceLoadFlag::KeepSourceData;

		HResource resource = gResources().load(scriptRef->getHandle(), loadFlags);
		if (resource == nullptr)
			return nullptr;

		ScriptResourceBase* scriptResource = ScriptResourceManager::instance().getScriptResource(resource, true);
		return scriptResource->getManagedInstance();
	}

	void ScriptResources::internal_Release(ScriptResourceBase* resource)
	{
		resource->getGenericHandle().release();
	}

	void ScriptResources::internal_ReleaseRef(ScriptResourceRef* resourceRef)
	{
		resourceRef->getHandle().release();
	}

	void ScriptResources::internal_UnloadUnused()
	{
		gResources().unloadAllUnused();
	}
}