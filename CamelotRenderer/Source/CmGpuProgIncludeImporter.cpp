#include "CmGpuProgIncludeImporter.h"
#include "CmGpuProgInclude.h"
#include "CmDataStream.h"
#include "CmFileSystem.h"

namespace CamelotEngine
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

	BaseResourceHandle GpuProgIncludeImporter::import(const String& filePath)
	{
		DataStreamPtr stream = FileSystem::open(filePath);
		String includeString = stream->getAsString();

		GpuProgIncludeHandle gpuProgInclude = GpuProgInclude::create(includeString);
		gpuProgInclude.waitUntilLoaded();

		return gpuProgInclude;
	}
}