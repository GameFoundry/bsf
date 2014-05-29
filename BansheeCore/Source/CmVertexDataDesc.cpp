#include "CmVertexDataDesc.h"
#include "CmHardwareBufferManager.h"
#include "CmVertexDataDescRTTI.h"

namespace BansheeEngine
{
	void VertexDataDesc::addVertElem(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx)
	{
		clearIfItExists(type, semantic, semanticIdx, streamIdx);

		VertexElement newElement(streamIdx, 0, type, semantic, semanticIdx);

		// Insert it so it is sorted by stream
		UINT32 insertToIndex = (UINT32)mVertexElements.size();
		UINT32 idx = 0;
		for(auto& elem : mVertexElements)
		{
			if(elem.getStreamIdx() > streamIdx)
			{
				insertToIndex = idx;
				break;
			}

			idx++;
		}

		mVertexElements.insert(mVertexElements.begin() + insertToIndex, newElement);
	}

	VertexDeclarationPtr VertexDataDesc::createDeclaration() const
	{
		UINT32 maxStreamIdx = getMaxStreamIdx();

		UINT32 numStreams = maxStreamIdx + 1;
		UINT32* streamOffsets = bs_newN<UINT32, ScratchAlloc>(numStreams);
		for(UINT32 i = 0; i < numStreams; i++)
			streamOffsets[i] = 0;

		VertexDeclaration::VertexElementList declarationElements;
		for(auto& vertElem : mVertexElements)
		{
			UINT32 streamIdx = vertElem.getStreamIdx();

			declarationElements.push_back(VertexElement(streamIdx, streamOffsets[streamIdx], vertElem.getType(),
				vertElem.getSemantic(), vertElem.getSemanticIdx()));

			streamOffsets[streamIdx] += vertElem.getSize();
		}

		bs_deleteN<ScratchAlloc>(streamOffsets, numStreams);

		VertexDeclarationPtr declaration = HardwareBufferManager::instance().createVertexDeclaration(declarationElements);
		return declaration;
	}

	UINT32 VertexDataDesc::getMaxStreamIdx() const
	{
		UINT32 maxStreamIdx = 0;
		for(auto& vertElems : mVertexElements)
		{
			UINT32 offset = 0;
			for(auto& vertElem : mVertexElements)
			{
				maxStreamIdx = std::max((UINT32)maxStreamIdx, (UINT32)vertElem.getStreamIdx());
			}
		}

		return maxStreamIdx;
	}

	bool VertexDataDesc::hasStream(UINT32 streamIdx) const
	{
		for(auto& vertElem : mVertexElements)
		{
			if(vertElem.getStreamIdx() == streamIdx)
				return true;
		}

		return false;
	}

	bool VertexDataDesc::hasElement(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx) const
	{
		auto findIter = std::find_if(mVertexElements.begin(), mVertexElements.end(), 
			[semantic, semanticIdx, streamIdx] (const VertexElement& x) 
		{ 
			return x.getSemantic() == semantic && x.getSemanticIdx() == semanticIdx && x.getStreamIdx() == streamIdx; 
		});

		if(findIter != mVertexElements.end())
		{
			return true;
		}

		return false;
	}

	UINT32 VertexDataDesc::getElementSize(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx) const
	{
		for(auto& element : mVertexElements)
		{
			if(element.getSemantic() == semantic && element.getSemanticIdx() == semanticIdx && element.getStreamIdx() == streamIdx)
				return element.getSize();
		}

		return -1;
	}

	UINT32 VertexDataDesc::getElementOffsetFromStream(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx) const
	{
		UINT32 vertexOffset = 0;
		for(auto& element : mVertexElements)
		{
			if(element.getStreamIdx() != streamIdx)
				continue;

			if(element.getSemantic() == semantic && element.getSemanticIdx() == semanticIdx)
				break;

			vertexOffset += element.getSize();
		}

		return vertexOffset;
	}

	UINT32 VertexDataDesc::getVertexStride(UINT32 streamIdx) const
	{
		UINT32 vertexStride = 0;
		for(auto& element : mVertexElements)
		{
			if(element.getStreamIdx() == streamIdx)
				vertexStride += element.getSize();
		}

		return vertexStride;
	}

	UINT32 VertexDataDesc::getVertexStride() const
	{
		UINT32 vertexStride = 0;
		for(auto& element : mVertexElements)
		{
			vertexStride += element.getSize();
		}

		return vertexStride;
	}

	UINT32 VertexDataDesc::getStreamOffset(UINT32 streamIdx) const
	{
		UINT32 streamOffset = 0;
		bool found = false;
		for(auto& element : mVertexElements)
		{
			if(element.getStreamIdx() == streamIdx)
				break;

			streamOffset += element.getSize();
		}

		return streamOffset;
	}

	void VertexDataDesc::clearIfItExists(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx)
	{
		auto findIter = std::find_if(mVertexElements.begin(), mVertexElements.end(), 
			[semantic, semanticIdx, streamIdx] (const VertexElement& x) 
		{ 
			return x.getSemantic() == semantic && x.getSemanticIdx() == semanticIdx && x.getStreamIdx() == streamIdx; 
		});

		if(findIter != mVertexElements.end())
		{
			mVertexElements.erase(findIter);
		}
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/

	RTTITypeBase* VertexDataDesc::getRTTIStatic()
	{
		return VertexDataDescRTTI::instance();
	}

	RTTITypeBase* VertexDataDesc::getRTTI() const
	{
		return VertexDataDesc::getRTTIStatic();
	}
}