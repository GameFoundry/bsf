#include "CmGpuProgIncludeImporter.h"
#include "CmGpuProgInclude.h"
#include "CmPath.h"
#include "CmDataStream.h"
#include "CmFileSystem.h"

namespace BansheeEngine
{
	GpuProgIncludeImporter::GpuProgIncludeImporter()
		:SpecificImporter()
	{

	}

	GpuProgIncludeImporter::~GpuProgIncludeImporter()
	{

	}

	bool GpuProgIncludeImporter::isExtensionSupported(const WString& ext) const
	{
		WString lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return lowerCaseExt == L"gpuproginc";
	}

	bool GpuProgIncludeImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		return true; // Plain-text so I don't even check for magic number
	}

	ResourcePtr GpuProgIncludeImporter::import(const WString& filePath, ConstImportOptionsPtr importOptions)
	{
		DataStreamPtr stream = FileSystem::openFile(filePath);
		String includeString = stream->getAsString();

		GpuProgIncludePtr gpuInclude = GpuProgInclude::_createPtr(includeString);

		WString fileName = OldPath::getFilename(filePath, false);
		gpuInclude->setName(toString(fileName));

		return gpuInclude;
	}
}