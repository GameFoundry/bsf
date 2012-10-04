#include "CmFBXImporter.h"
#include "CmResource.h"
#include "CmDebug.h"
#include "CmDataStream.h"
#include "CmPath.h"

namespace CamelotEngine
{
	FBXImporter::FBXImporter()
		:SpecificImporter() 
	{

	}

	FBXImporter::~FBXImporter() 
	{

	}

	bool FBXImporter::isExtensionSupported(const String& ext) const
	{
		String lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return find(mExtensions.begin(), mExtensions.end(), lowerCaseExt) != mExtensions.end();
	}

	bool FBXImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		String ext = magicNumToExtension(magicNumPtr, numBytes);

		return isExtensionSupported(ext);
	}

	String FBXImporter::magicNumToExtension(const UINT8* magic, UINT32 maxBytes) const
	{

		return StringUtil::BLANK;
	}

	ResourcePtr FBXImporter::import(DataStreamPtr fileData)
	{
		return nullptr;
	}
}