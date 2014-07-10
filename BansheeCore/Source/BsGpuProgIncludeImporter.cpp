//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGpuProgIncludeImporter.h"
#include "BsGpuProgInclude.h"
#include "BsPath.h"
#include "BsDataStream.h"
#include "BsFileSystem.h"

namespace BansheeEngine
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

	ResourcePtr GpuProgIncludeImporter::import(const Path& filePath, ConstImportOptionsPtr importOptions)
	{
		DataStreamPtr stream = FileSystem::openFile(filePath);
		String includeString = stream->getAsString();

		GpuProgIncludePtr gpuInclude = GpuProgInclude::_createPtr(includeString);

		WString fileName = filePath.getWFilename(false);
		gpuInclude->setName(toString(fileName));

		return gpuInclude;
	}
}