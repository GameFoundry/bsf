//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsSLImporter.h"
#include "FileSystem/BsDataStream.h"
#include "FileSystem/BsFileSystem.h"
#include "BsSLFXCompiler.h"
#include "Importer/BsShaderImportOptions.h"

namespace bs
{
	bool SLImporter::isExtensionSupported(const String& ext) const
	{
		String lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return lowerCaseExt == u8"bsl";
	}

	bool SLImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		return true; // Plain-text so I don't even check for magic number
	}

	SPtr<Resource> SLImporter::import(const Path& filePath, SPtr<const ImportOptions> importOptions)
	{
		String source;
		{
			Lock fileLock = FileScheduler::getLock(filePath);

			SPtr<DataStream> stream = FileSystem::openFile(filePath);
			source = stream->getAsString();
		}

		SPtr<const ShaderImportOptions> io = std::static_pointer_cast<const ShaderImportOptions>(importOptions);
		String shaderName = filePath.getFilename(false);
		BSLFXCompileResult result = BSLFXCompiler::compile(shaderName, source, io->getDefines(), io->languages);

		if (result.shader != nullptr)
			result.shader->setName(shaderName);
		
		if(!result.errorMessage.empty())
		{
			String file;
			if (result.errorFile.empty())
				file = filePath.toString();
			else
				file = result.errorFile;

			BS_LOG(Error, BSLCompiler, "Compilation error when importing shader \"{0}\":\n{1}. Location: {2} ({3})",
				file, result.errorMessage, result.errorLine, result.errorColumn);
		}

		return result.shader;
	}

	SPtr<ImportOptions> SLImporter::createImportOptions() const
	{
		return bs_shared_ptr_new<ShaderImportOptions>();
	}
}
