#include "BsShaderIncludeImporter.h"
#include "BsShaderInclude.h"
#include "BsDataStream.h"
#include "BsFileSystem.h"

namespace BansheeEngine
{
	ShaderIncludeImporter::ShaderIncludeImporter()
		:SpecificImporter()
	{

	}

	ShaderIncludeImporter::~ShaderIncludeImporter()
	{

	}

	bool ShaderIncludeImporter::isExtensionSupported(const WString& ext) const
	{
		WString lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return lowerCaseExt == L"bslinc";
	}

	bool ShaderIncludeImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		return true; // Plain-text so I don't even check for magic number
	}

	ResourcePtr ShaderIncludeImporter::import(const Path& filePath, ConstImportOptionsPtr importOptions)
	{
		DataStreamPtr stream = FileSystem::openFile(filePath);
		String includeString = stream->getAsString();

		ShaderIncludePtr gpuInclude = ShaderInclude::_createPtr(includeString);

		WString fileName = filePath.getWFilename(false);
		gpuInclude->setName(fileName);

		return gpuInclude;
	}
}