//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "RenderAPI/BsVertexData.h"
#include "Managers/BsHardwareBufferManager.h"
#include "RenderAPI/BsVertexBuffer.h"
#include "Math/BsVector3.h"
#include "Error/BsException.h"
#include "RenderAPI/BsRenderAPI.h"

namespace bs { namespace ct
{
	VertexData::VertexData()
	{
		vertexCount = 0;

	}

	VertexData::~VertexData()
	{
	}

	void VertexData::setBuffer(UINT32 index, SPtr<VertexBuffer> buffer)
	{
		mVertexBuffers[index] = buffer;

		recalculateMaxIndex();
	}

	SPtr<VertexBuffer> VertexData::getBuffer(UINT32 index) const
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
}}
