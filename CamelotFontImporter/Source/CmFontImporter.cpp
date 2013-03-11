#include "CmFontImporter.h"

namespace CamelotEngine
{
	FontImporter::FontImporter()
		:SpecificImporter() 
	{
		mExtensions.push_back("ttf");
		mExtensions.push_back("otf");
	}

	FontImporter::~FontImporter() 
	{

	}

	bool FontImporter::isExtensionSupported(const String& ext) const
	{
		String lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return find(mExtensions.begin(), mExtensions.end(), lowerCaseExt) != mExtensions.end();
	}

	bool FontImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		// TODO
		return false;
	}

	BaseResourceHandle FontImporter::import(const String& filePath, ConstImportOptionsPtr importOptions)
	{
		// TODO
		return BaseResourceHandle();
	}
}