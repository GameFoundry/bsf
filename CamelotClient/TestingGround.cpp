#include "TestingGround.h"

#include "CmFileSerializer.h"
#include "CmResource.h"

using namespace CamelotEngine;

void test()
{
	Resource* dbgResource = new Resource();
	UINT32 size = 5;
	dbgResource->getSerializable()->setPlainValue(dbgResource, "Size", size);

	FileSerializer fs;
	fs.encode(dbgResource, "C:\\ResourceTest.res");

	Resource* loadedResource = new Resource();
	fs.decode(loadedResource, "C:\\ResourceTest.res");
}