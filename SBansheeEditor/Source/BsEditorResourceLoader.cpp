#include "BsEditorResourceLoader.h"
#include "BsProjectLibrary.h"
#include "BsResources.h"
#include "BsProjectResourceMeta.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	HResource EditorResourceLoader::load(const Path& path) const
	{
		ProjectLibrary::LibraryEntry* entry = gProjectLibrary().findEntry(path);

		if (entry == nullptr || entry->type == ProjectLibrary::LibraryEntryType::Directory)
			return HResource();

		ProjectLibrary::ResourceEntry* resEntry = static_cast<ProjectLibrary::ResourceEntry*>(entry);
		String resUUID = resEntry->meta->getUUID();

		if (resEntry->meta->getIncludeInBuild())
		{
			LOGWRN("Dynamically loading a resource at path: \"" + path.toString() + "\" but the resource \
					isn't flagged to be included in the build. It may not be available outside of the editor.");
		}

		return gResources().loadFromUUID(resUUID);
	}
}