#include "BsScriptCodeImporter.h"
#include "BsScriptCode.h"
#include "BsPath.h"
#include "BsDataStream.h"
#include "BsFileSystem.h"

namespace BansheeEngine
{
	ScriptCodeImporter::ScriptCodeImporter()
		:SpecificImporter()
	{

	}

	ScriptCodeImporter::~ScriptCodeImporter()
	{

	}

	bool ScriptCodeImporter::isExtensionSupported(const WString& ext) const
	{
		WString lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return lowerCaseExt == L"cs";
	}

	bool ScriptCodeImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		return true; // Plain-text so we don't even check for magic number
	}

	ResourcePtr ScriptCodeImporter::import(const Path& filePath, ConstImportOptionsPtr importOptions)
	{
		DataStreamPtr stream = FileSystem::openFile(filePath);
		WString textData = stream->getAsWString();

		ScriptCodePtr scriptCode = ScriptCode::_createPtr(textData);

		WString fileName = filePath.getWFilename(false);
		scriptCode->setName(fileName);

		return scriptCode;
	}
}