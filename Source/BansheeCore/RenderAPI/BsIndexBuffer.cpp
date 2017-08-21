//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "RenderAPI/BsIndexBuffer.h"
#include "Managers/BsHardwareBufferManager.h"
#include "RenderAPI/BsRenderAPI.h"

namespace bs 
{
	UINT32 calcIndexSize(IndexType type)
	{
		switch (type)
		{
		case IT_16BIT:
			return sizeof(unsigned short);
		default:
		case IT_32BIT:
			return sizeof(unsigned int);
		}
	}

	IndexBufferProperties::IndexBufferProperties(IndexType idxType, UINT32 numIndices)
		:mIndexType(idxType), mNumIndices(numIndices), mIndexSize(calcIndexSize(idxType))
	{ }

    IndexBuffer::IndexBuffer(const INDEX_BUFFER_DESC& desc)
		:mProperties(desc.indexType, desc.numIndices), mUsage(desc.usage)
    {

	}

	SPtr<ct::IndexBuffer> IndexBuffer::getCore() const
	{
		return std::static_pointer_cast<ct::IndexBuffer>(mCoreSpecific);
	}

	SPtr<ct::CoreObject> IndexBuffer::createCore() const
	{
		INDEX_BUFFER_DESC desc;
		desc.indexType = mProperties.mIndexType;
		desc.numIndices = mProperties.mNumIndices;
		desc.usage = mUsage;

		return ct::HardwareBufferManager::instance().createIndexBufferInternal(desc);
	}

	SPtr<IndexBuffer> IndexBuffer::create(const INDEX_BUFFER_DESC& desc)
	{
		return HardwareBufferManager::instance().createIndexBuffer(desc);
	}

	namespace ct
	{
	IndexBuffer::IndexBuffer(const INDEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
		:HardwareBuffer(calcIndexSize(desc.indexType) * desc.numIndices), mProperties(desc.indexType, desc.numIndices)
	{ }

	SPtr<IndexBuffer> IndexBuffer::create(const INDEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask)
	{
		return HardwareBufferManager::instance().createIndexBuffer(desc, deviceMask);
	}
	}
}