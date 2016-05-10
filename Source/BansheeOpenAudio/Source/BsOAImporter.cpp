//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsOAImporter.h"
#include "BsDataStream.h"
#include "BsFileSystem.h"

namespace BansheeEngine
{
	OAImporter::OAImporter()
		:SpecificImporter()
	{

	}

	OAImporter::~OAImporter()
	{

	}

	bool OAImporter::isExtensionSupported(const WString& ext) const
	{
		WString lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		// TODO
		return false;
	}

	bool OAImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		// TODO
		return true; // Plain-text so I don't even check for magic number
	}

	SPtr<Resource> OAImporter::import(const Path& filePath, SPtr<const ImportOptions> importOptions)
	{
		SPtr<DataStream> stream = FileSystem::openFile(filePath);

		return nullptr;
	}
}