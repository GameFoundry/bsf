#include "BsSLImporter.h"
#include "BsDataStream.h"
#include "BsFileSystem.h"
#include "BsSLFXCompiler.h"

namespace BansheeEngine
{
	SLImporter::SLImporter()
		:SpecificImporter()
	{

	}

	SLImporter::~SLImporter()
	{

	}

	bool SLImporter::isExtensionSupported(const WString& ext) const
	{
		WString lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return lowerCaseExt == L"bsl";
	}

	bool SLImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		return true; // Plain-text so I don't even check for magic number
	}

	ResourcePtr SLImporter::import(const Path& filePath, ConstImportOptionsPtr importOptions)
	{
		DataStreamPtr stream = FileSystem::openFile(filePath);
		String source = stream->getAsString();

		ShaderPtr shader = BSLFXCompiler::compile(source);
		shader->setName(filePath.getWFilename(false));

		return shader;
	}
}