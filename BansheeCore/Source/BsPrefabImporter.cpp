#include "BsPrefabImporter.h"
#include "BsFileSerializer.h"
#include "BsResource.h"

namespace BansheeEngine
{
	PrefabImporter::PrefabImporter()
		:SpecificImporter()
	{

	}

	PrefabImporter::~PrefabImporter()
	{

	}

	bool PrefabImporter::isExtensionSupported(const WString& ext) const
	{
		WString lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return lowerCaseExt == L"prefab";
	}

	bool PrefabImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		return true; // No magic number of asset files, they must always rely on extension
	}

	ResourcePtr PrefabImporter::import(const Path& filePath, ConstImportOptionsPtr importOptions)
	{
		FileDecoder fs(filePath);
		fs.skip(); // Skipped over saved resource data
		std::shared_ptr<IReflectable> loadedData = fs.decode();

		if (loadedData == nullptr)
			BS_EXCEPT(InternalErrorException, "Unable to import resource.");

		if (!loadedData->isDerivedFrom(Resource::getRTTIStatic()))
			BS_EXCEPT(InternalErrorException, "Imported object doesn't derive from Resource.");

		ResourcePtr resource = std::static_pointer_cast<Resource>(loadedData);

		return resource;
	}
}