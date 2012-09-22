#include "TestingGround.h"

#include "CmFileSerializer.h"
#include "CmResource.h"
#include "CmTextureManager.h"

using namespace CamelotEngine;

void test()
{
	Resource* dbgResource = new Resource();
	UINT32 size = 5;
	dbgResource->getSerializable()->setPlainValue(dbgResource, "Size", size);

	FileSerializer fs;
	fs.encode(dbgResource, "C:\\ResourceTest.res");

	ResourcePtr loadedResource(new Resource());
	fs.decode(loadedResource, "C:\\ResourceTest.res");

	TexturePtr dbgTexture = TextureManager::instance().create(TEX_TYPE_2D, 128, 128, 1, PF_A8B8G8R8);
	dbgTexture->setFSAA(0, "test");
	fs.encode(dbgTexture.get(), "C:\\DbgTexture.tex");

	TexturePtr emptyTexture = TextureManager::instance().create(TEX_TYPE_2D, 512, 512, 1, PF_UNKNOWN);
	fs.decode(emptyTexture, "C:\\DbgTexture.tex");
}