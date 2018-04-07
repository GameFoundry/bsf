//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Resources/BsPlainTextImporter.h"
#include "Resources/BsPlainText.h"
#include "FileSystem/BsDataStream.h"
#include "FileSystem/BsFileSystem.h"

namespace bs
{
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

	SPtr<Resource> PlainTextImporter::import(const Path& filePath, SPtr<const ImportOptions> importOptions)
	{
		WString textData;
		{
			Lock fileLock = FileScheduler::getLock(filePath);

			SPtr<DataStream> stream = FileSystem::openFile(filePath);
			textData = stream->getAsWString();
		}

		SPtr<PlainText> plainText = PlainText::_createPtr(textData);

		WString fileName = filePath.getWFilename(false);
		plainText->setName(fileName);

		return plainText;
	}
}