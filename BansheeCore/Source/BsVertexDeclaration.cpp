#include "BsVertexDeclaration.h"
#include "BsVertexDeclarationRTTI.h"
#include "BsHardwareBufferManager.h"
#include "BsRenderSystem.h"
#include "BsUtil.h"

namespace BansheeEngine
{
	VertexElement::VertexElement(UINT16 source, UINT32 offset,
		VertexElementType theType, VertexElementSemantic semantic, UINT16 index)
		: mSource(source), mOffset(offset), mType(theType), mSemantic(semantic), mIndex(index)
	{
	}

	UINT32 VertexElement::getSize(void) const
	{
		return getTypeSize(mType);
	}

	UINT32 VertexElement::getTypeSize(VertexElementType etype)
	{
		switch(etype)
		{
		case VET_COLOR:
		case VET_COLOR_ABGR:
		case VET_COLOR_ARGB:
			return sizeof(RGBA);
		case VET_FLOAT1:
			return sizeof(float);
		case VET_FLOAT2:
			return sizeof(float)*2;
		case VET_FLOAT3:
			return sizeof(float)*3;
		case VET_FLOAT4:
			return sizeof(float)*4;
		case VET_SHORT1:
			return sizeof(short);
		case VET_SHORT2:
			return sizeof(short)*2;
		case VET_SHORT3:
			return sizeof(short)*3;
		case VET_SHORT4:
			return sizeof(short)*4;
		case VET_UBYTE4:
			return sizeof(unsigned char)*4;
		}

		return 0;
	}

	unsigned short VertexElement::getTypeCount(VertexElementType etype)
	{
		switch (etype)
		{
		case VET_COLOR:
		case VET_COLOR_ABGR:
		case VET_COLOR_ARGB:
			return 4;
		case VET_FLOAT1:
			return 1;
		case VET_FLOAT2:
			return 2;
		case VET_FLOAT3:
			return 3;
		case VET_FLOAT4:
			return 4;
		case VET_SHORT1:
			return 1;
		case VET_SHORT2:
			return 2;
		case VET_SHORT3:
			return 3;
		case VET_SHORT4:
			return 4;
		case VET_UBYTE4:
			return 4;
		}

		BS_EXCEPT(InvalidParametersException, "Invalid type");
	}

	VertexElementType VertexElement::getBestColorVertexElementType()
	{
		// Use the current render system to determine if possible
		if (BansheeEngine::RenderSystem::instancePtr())
		{
			return BansheeEngine::RenderSystem::instancePtr()->getColorVertexElementType();
		}
		else
		{
			// We can't know the specific type right now, so pick a type
			// based on platform
#if BS_PLATFORM == BS_PLATFORM_WIN32
			return VET_COLOR_ARGB; // prefer D3D format on windows
#else
			return VET_COLOR_ABGR; // prefer GL format on everything else
#endif

		}
	}

	bool VertexElement::operator== (const VertexElement& rhs) const
	{
		if (mType != rhs.mType || mIndex != rhs.mIndex || mOffset != rhs.mOffset ||
			mSemantic != rhs.mSemantic || mSource != rhs.mSource)
		{
			return false;
		}
		else
			return true;
	}

	bool VertexElement::operator!= (const VertexElement& rhs) const
	{
		return !(*this == rhs);
	}

	VertexDeclaration::VertexDeclaration(const VertexElementList& elements)
	{
		for (auto& elem : elements)
		{
			VertexElementType type = elem.getType();

			if (elem.getType() == VET_COLOR)
				type = VertexElement::getBestColorVertexElementType();

			mElementList.push_back(VertexElement(elem.getStreamIdx(), elem.getOffset(), type, elem.getSemantic(), elem.getSemanticIdx()));
		}
	}

	VertexDeclaration::~VertexDeclaration()
	{
	}

	bool VertexDeclaration::operator== (const VertexDeclaration& rhs) const
	{
		if (mElementList.size() != rhs.mElementList.size())
			return false;

		auto myIter = mElementList.begin();
		auto theirIter = rhs.mElementList.begin();

		for (; myIter != mElementList.end() && theirIter != rhs.mElementList.end(); ++myIter, ++theirIter)
		{
			if (!(*myIter == *theirIter))
				return false;
		}

		return true;
	}

	bool VertexDeclaration::operator!= (const VertexDeclaration& rhs) const
	{
		return !(*this == rhs);
	}

	const VertexElement* VertexDeclaration::getElement(UINT16 index)
	{
		assert(index < mElementList.size() && "Index out of bounds");

		auto iter = mElementList.begin();
		for (UINT16 i = 0; i < index; ++i)
			++iter;

		return &(*iter);

	}
	
	const VertexElement* VertexDeclaration::findElementBySemantic(VertexElementSemantic sem, UINT16 index)
	{
		for (auto& elem : mElementList)
		{
			if (elem.getSemantic() == sem && elem.getSemanticIdx() == index)
			{
				return &elem;
			}
		}

		return nullptr;
	}

	VertexDeclaration::VertexElementList VertexDeclaration::findElementsBySource(UINT16 source)
	{
		VertexElementList retList;

		for (auto& elem : mElementList)
		{
			if (elem.getStreamIdx() == source)
			{
				retList.push_back(elem);
			}
		}

		return retList;
	}

	UINT32 VertexDeclaration::getVertexSize(UINT16 source)
	{
		UINT32 size = 0;

		for (auto& elem : mElementList)
		{
			if (elem.getStreamIdx() == source)
			{
				size += elem.getSize();
			}
		}

		return size;
	}

	VertexDeclarationPtr VertexDeclaration::createVertexDeclaration(const VertexDeclaration::VertexElementList& elements)
	{
		return HardwareBufferManager::instance().createVertexDeclaration(elements);
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* VertexDeclaration::getRTTIStatic()
	{
		return VertexDeclarationRTTI::instance();
	}

	RTTITypeBase* VertexDeclaration::getRTTI() const
	{
		return getRTTIStatic();
	}
}