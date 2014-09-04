#include "BsMaterialProxy.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	MaterialProxy::DirtyParamsInfo* MaterialProxy::DirtyParamsInfo::create(FrameAlloc* alloc, UINT32 numParams)
	{
		UINT32 sizeRequired = sizeof(DirtyParamsInfo) + numParams * sizeof(ParamsBindInfo);
		UINT8* buffer = (UINT8*)alloc->alloc(sizeRequired);

		MaterialProxy::DirtyParamsInfo* dirtyParamsInfo = (MaterialProxy::DirtyParamsInfo*)buffer;
		dirtyParamsInfo->numEntries = numParams;
		dirtyParamsInfo->owner = alloc;

		buffer += sizeof(MaterialProxy::DirtyParamsInfo);
		MaterialProxy::ParamsBindInfo* paramsInfoArray = (MaterialProxy::ParamsBindInfo*)buffer;
		dirtyParamsInfo->entries = paramsInfoArray;

		// Construct shared pointers
		for (UINT32 i = 0; i < numParams; i++)
		{
			new (&dirtyParamsInfo->entries[i].params) GpuParamBlockPtr(nullptr);
		}

		return dirtyParamsInfo;
	}

	void MaterialProxy::DirtyParamsInfo::destroy(DirtyParamsInfo* paramsInfo)
	{
		// Destruct shared pointers
		for (UINT32 i = 0; i < paramsInfo->numEntries; i++)
		{
			paramsInfo->entries[i].params.~shared_ptr();
		}

		paramsInfo->owner->dealloc((UINT8*)paramsInfo);
	}
}