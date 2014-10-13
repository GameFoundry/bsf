#include "BsScriptProjectLibrary.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsProjectLibrary.h"
#include "BsScriptResource.h"
#include "BsResources.h"
#include "BsResource.h"
#include "BsProjectResourceMeta.h"
#include "BsScriptResourceManager.h"
#include "BsScriptTexture2D.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptFont.h"

namespace BansheeEngine
{
	ScriptProjectLibrary::ScriptProjectLibrary(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptProjectLibrary::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptProjectLibrary::internal_Create);
		metaData.scriptClass->addInternalCall("Internal_Load", &ScriptProjectLibrary::internal_Load);
	}

	void ScriptProjectLibrary::internal_Create(MonoObject* resource, MonoString* path)
	{
		ScriptResource* scrResource = ScriptResource::toNative(resource);
		Path resourcePath = MonoUtil::monoToWString(path);

		ProjectLibrary::instance().createEntry(scrResource->getNativeHandle(), resourcePath);
	}

	MonoObject* ScriptProjectLibrary::internal_Load(MonoString* path)
	{
		Path resourcePath = MonoUtil::monoToWString(path);

		ProjectLibrary::LibraryEntry* entry = ProjectLibrary::instance().findEntry(resourcePath);

		if (entry == nullptr || entry->type == ProjectLibrary::LibraryEntryType::Directory)
			return nullptr;

		ProjectLibrary::ResourceEntry* resEntry = static_cast<ProjectLibrary::ResourceEntry*>(entry);
		String resUUID = resEntry->meta->getUUID();

		HResource resource = Resources::instance().loadFromUUID(resUUID);
		if (!resource)
			return nullptr;

		ScriptResourceBase* scriptResource = ScriptResourceManager::instance().getScriptResource(resUUID);
		if (scriptResource == nullptr)
			scriptResource = ScriptResourceManager::instance().createScriptResource(resource);

		return scriptResource->getManagedInstance();
	}
}