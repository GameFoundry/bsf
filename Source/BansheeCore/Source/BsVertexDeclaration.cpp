//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVertexDeclaration.h"
#include "BsVertexDeclarationRTTI.h"
#include "BsHardwareBufferManager.h"
#include "BsRenderAPI.h"

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
		return 0;
	}

	VertexElementType VertexElement::getBestColorVertexElementType()
	{
		// Use the current render system to determine if possible
		if (BansheeEngine::RenderAPICore::instancePtr())
		{
			return BansheeEngine::RenderAPICore::instancePtr()->getColorVertexElementType();
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

	VertexDeclarationProperties::VertexDeclarationProperties(const List<VertexElement>& elements)
	{
		for (auto& elem : elements)
		{
			VertexElementType type = elem.getType();

			if (elem.getType() == VET_COLOR)
				type = VertexElement::getBestColorVertexElementType();

			mElementList.push_back(VertexElement(elem.getStreamIdx(), elem.getOffset(), type, elem.getSemantic(), elem.getSemanticIdx()));
		}
	}

	bool VertexDeclarationProperties::operator== (const VertexDeclarationProperties& rhs) const
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

	bool VertexDeclarationProperties::operator!= (const VertexDeclarationProperties& rhs) const
	{
		return !(*this == rhs);
	}

	const VertexElement* VertexDeclarationProperties::getElement(UINT16 index) const
	{
		assert(index < mElementList.size() && "Index out of bounds");

		auto iter = mElementList.begin();
		for (UINT16 i = 0; i < index; ++i)
			++iter;

		return &(*iter);

	}
	
	const VertexElement* VertexDeclarationProperties::findElementBySemantic(VertexElementSemantic sem, UINT16 index) const
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

	List<VertexElement> VertexDeclarationProperties::findElementsBySource(UINT16 source) const
	{
		List<VertexElement> retList;

		for (auto& elem : mElementList)
		{
			if (elem.getStreamIdx() == source)
			{
				retList.push_back(elem);
			}
		}

		return retList;
	}

	UINT32 VertexDeclarationProperties::getVertexSize(UINT16 source) const
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

	UINT32 VertexDeclarationCore::NextFreeId = 0;

	VertexDeclarationCore::VertexDeclarationCore(const List<VertexElement>& elements)
		:mProperties(elements)
	{
		
	}

	void VertexDeclarationCore::initialize()
	{
		mId = NextFreeId++;
		CoreObjectCore::initialize();
	}

	bool VertexDeclarationCore::isCompatible(const SPtr<VertexDeclarationCore>& shaderDecl)
	{
		const List<VertexElement>& shaderElems = shaderDecl->getProperties().getElements();
		const List<VertexElement>& bufferElems = getProperties().getElements();

		for (auto shaderIter = shaderElems.begin(); shaderIter != shaderElems.end(); ++shaderIter)
		{
			const VertexElement* foundElement = nullptr;
			for (auto bufferIter = bufferElems.begin(); bufferIter != bufferElems.end(); ++bufferIter)
			{
				if (shaderIter->getSemantic() == bufferIter->getSemantic() && shaderIter->getSemanticIdx() == bufferIter->getSemanticIdx())
				{
					foundElement = &(*bufferIter);
					break;
				}
			}

			if (foundElement == nullptr)
				return false;
		}

		return true;
	}

	Vector<VertexElement> VertexDeclarationCore::getMissingElements(const SPtr<VertexDeclarationCore>& shaderDecl)
	{
		Vector<VertexElement> missingElements;

		const List<VertexElement>& shaderElems = shaderDecl->getProperties().getElements();
		const List<VertexElement>& bufferElems = getProperties().getElements();

		for (auto shaderIter = shaderElems.begin(); shaderIter != shaderElems.end(); ++shaderIter)
		{
			const VertexElement* foundElement = nullptr;
			for (auto bufferIter = bufferElems.begin(); bufferIter != bufferElems.end(); ++bufferIter)
			{
				if (shaderIter->getSemantic() == bufferIter->getSemantic() && shaderIter->getSemanticIdx() == bufferIter->getSemanticIdx())
				{
					foundElement = &(*bufferIter);
					break;
				}
			}

			if (foundElement == nullptr)
				missingElements.push_back(*shaderIter);
		}

		return missingElements;
	}

	VertexDeclaration::VertexDeclaration(const List<VertexElement>& elements)
		:mProperties(elements)
	{

	}

	SPtr<VertexDeclarationCore> VertexDeclaration::getCore() const
	{
		return std::static_pointer_cast<VertexDeclarationCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> VertexDeclaration::createCore() const
	{
		return HardwareBufferCoreManager::instance().createVertexDeclarationInternal(mProperties.mElementList);
	}

	VertexDeclarationPtr VertexDeclaration::createVertexDeclaration(const List<VertexElement>& elements)
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

	String toString(const VertexElementSemantic& val)
	{
		switch (val)
		{
		case VES_POSITION:
			return "POSITION";
		case VES_BLEND_WEIGHTS:
			return "BLEND_WEIGHTS";
		case VES_BLEND_INDICES:
			return "BLEND_INDICES";
		case VES_NORMAL:
			return "NORMAL";
		case VES_COLOR:
			return "COLOR";
		case VES_TEXCOORD:
			return "TEXCOORD";
		case VES_BITANGENT:
			return "BITANGENT";
		case VES_TANGENT:
			return "TANGENT";
		case VES_POSITIONT:
			return "POSITIONT";
		case VES_PSIZE:
			return "PSIZE";
		}

		return "";
	}

	WString toWString(const VertexElementSemantic& val)
	{
		return toWString(toString(val));
	}
}