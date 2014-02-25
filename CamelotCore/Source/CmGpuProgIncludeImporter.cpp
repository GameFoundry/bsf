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

		return GpuProgInclude::_createPtr(includeString);
	}
}