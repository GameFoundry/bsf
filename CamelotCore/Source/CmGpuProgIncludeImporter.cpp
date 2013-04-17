#include "CmGpuProgIncludeImporter.h"
#include "CmGpuProgInclude.h"
#include "CmDataStream.h"
#include "CmFileSystem.h"

namespace CamelotFramework
{
	GpuProgIncludeImporter::GpuProgIncludeImporter()
		:SpecificImporter()
	{

	}

	GpuProgIncludeImporter::~GpuProgIncludeImporter()
	{

	}

	bool GpuProgIncludeImporter::isExtensionSupported(const String& ext) const
	{
		String lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return lowerCaseExt == "gpuproginc";
	}

	bool GpuProgIncludeImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		return true; // Plain-text so I don't even check for magic number
	}

	HResource GpuProgIncludeImporter::import(const String& filePath, ConstImportOptionsPtr importOptions)
	{
		DataStreamPtr stream = FileSystem::open(filePath);
		String includeString = stream->getAsString();

		HGpuProgInclude gpuProgInclude = GpuProgInclude::create(includeString);

		return gpuProgInclude;
	}
}