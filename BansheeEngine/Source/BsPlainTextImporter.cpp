#include "BsPlainTextImporter.h"
#include "BsPlainText.h"
#include "BsPath.h"
#include "BsDataStream.h"
#include "BsFileSystem.h"

namespace BansheeEngine
{
	PlainTextImporter::PlainTextImporter()
		:SpecificImporter()
	{

	}

	PlainTextImporter::~PlainTextImporter()
	{

	}

	bool PlainTextImporter::isExtensionSupported(const WString& ext) const
	{
		WString lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return lowerCaseExt == L"txt" || lowerCaseExt == L"xml" || lowerCaseExt == L"json";
	}

	bool PlainTextImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		return true; // Plain-text so we don't even check for magic number
	}

	ResourcePtr PlainTextImporter::import(const Path& filePath, ConstImportOptionsPtr importOptions)
	{
		DataStreamPtr stream = FileSystem::openFile(filePath);
		WString textData = stream->getAsWString();

		PlainTextPtr plainText = PlainText::_createPtr(textData);

		WString fileName = filePath.getWFilename(false);
		plainText->setName(fileName);

		return plainText;
	}
}