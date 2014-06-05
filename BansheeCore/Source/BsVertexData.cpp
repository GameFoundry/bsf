#include "BsVertexData.h"
#include "BsHardwareBufferManager.h"
#include "BsVertexBuffer.h"
#include "BsVector3.h"
#include "BsException.h"
#include "BsRenderSystem.h"

namespace BansheeEngine 
{
	VertexData::VertexData()
	{
		vertexCount = 0;

	}

	VertexData::~VertexData()
	{
	}

	void VertexData::setBuffer(UINT32 index, VertexBufferPtr buffer)
	{
		mVertexBuffers[index] = buffer;

		recalculateMaxIndex();
	}

	VertexBufferPtr VertexData::getBuffer(UINT32 index) const
	{
		auto iterFind = mVertexBuffers.find(index);
		if(iterFind != mVertexBuffers.end())
		{
			return iterFind->second;
		}

		return nullptr;
	}

	bool VertexData::isBufferBound(UINT32 index) const
	{
		auto iterFind = mVertexBuffers.find(index);
		if(iterFind != mVertexBuffers.end())
		{
			if(iterFind->second != nullptr)
				return true;
		}

		return false;
	}

	void VertexData::recalculateMaxIndex()
	{
		mMaxBufferIdx = 0;
		for (auto& bufferData : mVertexBuffers)
			mMaxBufferIdx = std::max(bufferData.first, mMaxBufferIdx);
	}
}
