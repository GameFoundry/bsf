#include "TestingGround.h"

#include "CmFileSerializer.h"
#include "CmResource.h"
#include "CmTextureData.h"
#include "CmTextureManager.h"

using namespace CamelotEngine;

void test()
{
	FileSerializer fs;

	TexturePtr dbgTexture = TextureManager::instance().create(TEX_TYPE_2D, 128, 128, 1, PF_A8B8G8R8);
	dbgTexture->setFSAA(0, "test");
	fs.encode(dbgTexture.get(), "C:\\DbgTexture.tex");

	TexturePtr emptyTexture = TextureManager::instance().create(TEX_TYPE_2D, 512, 512, 1, PF_UNKNOWN);
	fs.decode(emptyTexture, "C:\\DbgTexture.tex");


	TextureDataPtr data = emptyTexture->mTextureData[0];
	UINT32 size2 = data->getSize();

	int a = 5;
}