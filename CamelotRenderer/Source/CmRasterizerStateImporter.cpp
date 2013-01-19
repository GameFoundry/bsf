#include "CmRasterizerStateImporter.h"
#include "CmResource.h"
#include "CmDebug.h"
#include "CmDataStream.h"
#include "CmPath.h"
#include "CmFileSystem.h"
#include "CmRasterizerState.h"

namespace CamelotEngine
{
	RasterizerStateImporter::RasterizerStateImporter()
		:SpecificImporter() 
	{

	}

	RasterizerStateImporter::~RasterizerStateImporter() 
	{

	}

	bool RasterizerStateImporter::isExtensionSupported(const String& ext) const
	{
		String lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return find(mExtensions.begin(), mExtensions.end(), lowerCaseExt) != mExtensions.end();
	}

	bool RasterizerStateImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		String ext = magicNumToExtension(magicNumPtr, numBytes);

		return isExtensionSupported(ext);
	}

	String RasterizerStateImporter::magicNumToExtension(const UINT8* magic, UINT32 maxBytes) const
	{
		return StringUtil::BLANK;
	}

	BaseResourceHandle RasterizerStateImporter::import(const String& filePath)
	{
		DataStreamPtr fileData = FileSystem::open(filePath, true);

		RASTERIZER_STATE_DESC stateDesc;// = parseStateData(fileData);


		// TODO - Not initialized properly


		RasterizerStateHandle newState(RasterizerState::create(stateDesc));

		newState.waitUntilLoaded();
		fileData->close();

		registerLoadedResource(newState);

		return newState;
	}
}