/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "CmHardwareVertexBuffer.h"
#include "CmColor.h"
#include "CmException.h"
#include "CmHardwareBufferManager.h"
#include "CmDefaultHardwareBufferManager.h"
#include "CmRenderSystem.h"
#include "CmVertexDeclarationRTTI.h"

namespace CamelotEngine {

    //-----------------------------------------------------------------------------
    HardwareVertexBuffer::HardwareVertexBuffer(HardwareBufferManagerBase* mgr, UINT32 vertexSize,  
        UINT32 numVertices, HardwareBuffer::Usage usage, 
        bool useSystemMemory) 
        : HardwareBuffer(usage, useSystemMemory), 
		  mMgr(mgr),
          mNumVertices(numVertices),
          mVertexSize(vertexSize)
    {
        // Calculate the size of the vertices
        mSizeInBytes = mVertexSize * numVertices;

    }
    //-----------------------------------------------------------------------------
    HardwareVertexBuffer::~HardwareVertexBuffer()
    {
		if (mMgr)
		{
			mMgr->_notifyVertexBufferDestroyed(this);
		}
    }
    //-----------------------------------------------------------------------------
    VertexElement::VertexElement(unsigned short source, UINT32 offset, 
        VertexElementType theType, VertexElementSemantic semantic, unsigned short index)
        : mSource(source), mOffset(offset), mType(theType), 
        mSemantic(semantic), mIndex(index)
    {
    }
    //-----------------------------------------------------------------------------
	UINT32 VertexElement::getSize(void) const
	{
		return getTypeSize(mType);
	}
	//-----------------------------------------------------------------------------
	UINT32 VertexElement::getTypeSize(VertexElementType etype)
	{
		switch(etype)
		{
		case VET_COLOUR:
		case VET_COLOUR_ABGR:
		case VET_COLOUR_ARGB:
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
	//-----------------------------------------------------------------------------
	unsigned short VertexElement::getTypeCount(VertexElementType etype)
	{
		switch (etype)
		{
		case VET_COLOUR:
		case VET_COLOUR_ABGR:
		case VET_COLOUR_ARGB:
			return 1;
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
		CM_EXCEPT(InvalidParametersException, "Invalid type");
	}
	//-----------------------------------------------------------------------------
	VertexElementType VertexElement::multiplyTypeCount(VertexElementType baseType, 
		unsigned short count)
	{
		switch (baseType)
		{
		case VET_FLOAT1:
			switch(count)
			{
			case 1:
				return VET_FLOAT1;
			case 2:
				return VET_FLOAT2;
			case 3:
				return VET_FLOAT3;
			case 4:
				return VET_FLOAT4;
            default:
                break;
			}
			break;
		case VET_SHORT1:
			switch(count)
			{
			case 1:
				return VET_SHORT1;
			case 2:
				return VET_SHORT2;
			case 3:
				return VET_SHORT3;
			case 4:
				return VET_SHORT4;
            default:
                break;
			}
			break;
        default:
            break;
		}
		CM_EXCEPT(InvalidParametersException, "Invalid base type");
	}
	//--------------------------------------------------------------------------
	VertexElementType VertexElement::getBestColourVertexElementType(void)
	{
		// Use the current render system to determine if possible
		if (CamelotEngine::RenderSystem::instancePtr())
		{
			return CamelotEngine::RenderSystem::instancePtr()->getColorVertexElementType();
		}
		else
		{
			// We can't know the specific type right now, so pick a type
			// based on platform
#if CM_PLATFORM == CM_PLATFORM_WIN32
			return VET_COLOUR_ARGB; // prefer D3D format on windows
#else
			return VET_COLOUR_ABGR; // prefer GL format on everything else
#endif

		}
	}
	//--------------------------------------------------------------------------
	void VertexElement::convertColourValue(VertexElementType srcType, 
		VertexElementType dstType, UINT32* ptr)
	{
		if (srcType == dstType)
			return;

		// Conversion between ARGB and ABGR is always a case of flipping R/B
		*ptr = 
		   ((*ptr&0x00FF0000)>>16)|((*ptr&0x000000FF)<<16)|(*ptr&0xFF00FF00);				
	}
	//--------------------------------------------------------------------------
	UINT32 VertexElement::convertColourValue(const Color& src, 
		VertexElementType dst)
	{
		switch(dst)
		{
#if CM_PLATFORM == CM_PLATFORM_WIN32
        default:
#endif
		case VET_COLOUR_ARGB:
			return src.getAsARGB();
#if CM_PLATFORM != CM_PLATFORM_WIN32
        default:
#endif
		case VET_COLOUR_ABGR: 
			return src.getAsABGR();
		};

	}
	//-----------------------------------------------------------------------------
	VertexElementType VertexElement::getBaseType(VertexElementType multiType)
	{
		switch (multiType)
		{
			case VET_FLOAT1:
			case VET_FLOAT2:
			case VET_FLOAT3:
			case VET_FLOAT4:
				return VET_FLOAT1;
			case VET_COLOUR:
				return VET_COLOUR;
			case VET_COLOUR_ABGR:
				return VET_COLOUR_ABGR;
			case VET_COLOUR_ARGB:
				return VET_COLOUR_ARGB;
			case VET_SHORT1:
			case VET_SHORT2:
			case VET_SHORT3:
			case VET_SHORT4:
				return VET_SHORT1;
			case VET_UBYTE4:
				return VET_UBYTE4;
		};
        // To keep compiler happy
        return VET_FLOAT1;
	}
	//-----------------------------------------------------------------------------
    VertexDeclaration::VertexDeclaration()
    {
    }
    //-----------------------------------------------------------------------------
    VertexDeclaration::~VertexDeclaration()
    {
    }
    //-----------------------------------------------------------------------------
    const VertexDeclaration::VertexElementList& VertexDeclaration::getElements(void) const
    {
        return mElementList;
    }
    //-----------------------------------------------------------------------------
    const VertexElement& VertexDeclaration::addElement(unsigned short source, 
        UINT32 offset, VertexElementType theType,
        VertexElementSemantic semantic, unsigned short index)
    {
		// Refine colour type to a specific type
		if (theType == VET_COLOUR)
		{
			theType = VertexElement::getBestColourVertexElementType();
		}
        mElementList.push_back(
            VertexElement(source, offset, theType, semantic, index)
            );
		return mElementList.back();
    }
    //-----------------------------------------------------------------------------
    const VertexElement& VertexDeclaration::insertElement(unsigned short atPosition,
        unsigned short source, UINT32 offset, VertexElementType theType,
        VertexElementSemantic semantic, unsigned short index)
    {
        if (atPosition >= mElementList.size())
        {
            return addElement(source, offset, theType, semantic, index);
        }

        VertexElementList::iterator i = mElementList.begin();
        for (unsigned short n = 0; n < atPosition; ++n)
            ++i;

        i = mElementList.insert(i, 
            VertexElement(source, offset, theType, semantic, index));
        return *i;

    }
    //-----------------------------------------------------------------------------
    const VertexElement* VertexDeclaration::getElement(unsigned short index)
    {
        assert(index < mElementList.size() && "Index out of bounds");

        VertexElementList::iterator i = mElementList.begin();
        for (unsigned short n = 0; n < index; ++n)
            ++i;

        return &(*i);

    }
    //-----------------------------------------------------------------------------
    void VertexDeclaration::removeElement(unsigned short elem_index)
    {
        assert(elem_index < mElementList.size() && "Index out of bounds");
        VertexElementList::iterator i = mElementList.begin();
        for (unsigned short n = 0; n < elem_index; ++n)
            ++i;
        mElementList.erase(i);
    }
    //-----------------------------------------------------------------------------
    void VertexDeclaration::removeElement(VertexElementSemantic semantic, unsigned short index)
    {
		VertexElementList::iterator ei, eiend;
		eiend = mElementList.end();
		for (ei = mElementList.begin(); ei != eiend; ++ei)
		{
			if (ei->getSemantic() == semantic && ei->getIndex() == index)
			{
				mElementList.erase(ei);
                break;
			}
		}
    }
	//-----------------------------------------------------------------------------
	void VertexDeclaration::removeAllElements(void)
	{
		mElementList.clear();
	}
    //-----------------------------------------------------------------------------
    void VertexDeclaration::modifyElement(unsigned short elem_index, 
        unsigned short source, UINT32 offset, VertexElementType theType,
        VertexElementSemantic semantic, unsigned short index)
    {
        assert(elem_index < mElementList.size() && "Index out of bounds");
        VertexElementList::iterator i = mElementList.begin();
        std::advance(i, elem_index);
        (*i) = VertexElement(source, offset, theType, semantic, index);
    }
    //-----------------------------------------------------------------------------
	const VertexElement* VertexDeclaration::findElementBySemantic(
		VertexElementSemantic sem, unsigned short index)
	{
		VertexElementList::const_iterator ei, eiend;
		eiend = mElementList.end();
		for (ei = mElementList.begin(); ei != eiend; ++ei)
		{
			if (ei->getSemantic() == sem && ei->getIndex() == index)
			{
				return &(*ei);
			}
		}

		return NULL;


	}
	//-----------------------------------------------------------------------------
	VertexDeclaration::VertexElementList VertexDeclaration::findElementsBySource(
		unsigned short source)
	{
		VertexElementList retList;
		VertexElementList::const_iterator ei, eiend;
		eiend = mElementList.end();
		for (ei = mElementList.begin(); ei != eiend; ++ei)
		{
			if (ei->getSource() == source)
			{
				retList.push_back(*ei);
			}
		}
		return retList;

	}

	//-----------------------------------------------------------------------------
	UINT32 VertexDeclaration::getVertexSize(unsigned short source)
	{
		VertexElementList::const_iterator i, iend;
		iend = mElementList.end();
		UINT32 sz = 0;

		for (i = mElementList.begin(); i != iend; ++i)
		{
			if (i->getSource() == source)
			{
				sz += i->getSize();

			}
		}
		return sz;
	}
    //-----------------------------------------------------------------------------
    VertexDeclarationPtr VertexDeclaration::clone(HardwareBufferManagerBase* mgr)
    {
		HardwareBufferManagerBase* pManager = mgr ? mgr : HardwareBufferManager::instancePtr(); 
        VertexDeclarationPtr ret = pManager->createVertexDeclaration();

		VertexElementList::const_iterator i, iend;
		iend = mElementList.end();
		for (i = mElementList.begin(); i != iend; ++i)
		{
            ret->addElement(i->getSource(), i->getOffset(), i->getType(), i->getSemantic(), i->getIndex());
        }
        return ret;
    }
    //-----------------------------------------------------------------------------
    // Sort routine for VertexElement
    bool VertexDeclaration::vertexElementLess(const VertexElement& e1, const VertexElement& e2)
    {
        // Sort by source first
        if (e1.getSource() < e2.getSource())
        {
            return true;
        }
        else if (e1.getSource() == e2.getSource())
        {
            // Use ordering of semantics to sort
            if (e1.getSemantic() < e2.getSemantic())
            {
                return true;
            }
            else if (e1.getSemantic() == e2.getSemantic())
            {
                // Use index to sort
                if (e1.getIndex() < e2.getIndex())
                {
                    return true;
                }
            }
        }
        return false;
    }
    void VertexDeclaration::sort(void)
    {
        mElementList.sort(VertexDeclaration::vertexElementLess);
    }
    //-----------------------------------------------------------------------------
    void VertexDeclaration::closeGapsInSource(void)
    {
        if (mElementList.empty())
            return;

        // Sort first
        sort();

        VertexElementList::iterator i, iend;
        iend = mElementList.end();
        unsigned short targetIdx = 0;
        unsigned short lastIdx = getElement(0)->getSource();
        unsigned short c = 0;
        for (i = mElementList.begin(); i != iend; ++i, ++c)
        {
            VertexElement& elem = *i;
            if (lastIdx != elem.getSource())
            {
                targetIdx++;
                lastIdx = elem.getSource();
            }
            if (targetIdx != elem.getSource())
            {
                modifyElement(c, targetIdx, elem.getOffset(), elem.getType(), 
                    elem.getSemantic(), elem.getIndex());
            }

        }

    }
    //-----------------------------------------------------------------------------
    unsigned short VertexDeclaration::getMaxSource(void) const
    {
        VertexElementList::const_iterator i, iend;
        iend = mElementList.end();
        unsigned short ret = 0;
        for (i = mElementList.begin(); i != iend; ++i)
        {
            if (i->getSource() > ret)
            {
                ret = i->getSource();
            }

        }
        return ret;
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

    //-----------------------------------------------------------------------------
	VertexBufferBinding::VertexBufferBinding() : mHighIndex(0)
	{
	}
    //-----------------------------------------------------------------------------
	VertexBufferBinding::~VertexBufferBinding()
	{
        unsetAllBindings();
	}
    //-----------------------------------------------------------------------------
	void VertexBufferBinding::setBinding(unsigned short index, const HardwareVertexBufferPtr& buffer)
	{
        // NB will replace any existing buffer ptr at this index, and will thus cause
        // reference count to decrement on that buffer (possibly destroying it)
		mBindingMap[index] = buffer;
		mHighIndex = std::max(mHighIndex, (unsigned short)(index+1));
	}
    //-----------------------------------------------------------------------------
	void VertexBufferBinding::unsetBinding(unsigned short index)
	{
		VertexBufferBindingMap::iterator i = mBindingMap.find(index);
		if (i == mBindingMap.end())
		{
			CM_EXCEPT(ItemIdentityException,
				"Cannot find buffer binding for index " + toString(index));
		}
		mBindingMap.erase(i);
	}
    //-----------------------------------------------------------------------------
    void VertexBufferBinding::unsetAllBindings(void)
    {
        mBindingMap.clear();
        mHighIndex = 0;
    }
    //-----------------------------------------------------------------------------
	const VertexBufferBinding::VertexBufferBindingMap& 
	VertexBufferBinding::getBindings(void) const
	{
		return mBindingMap;
	}
    //-----------------------------------------------------------------------------
	const HardwareVertexBufferPtr& VertexBufferBinding::getBuffer(unsigned short index) const
	{
		VertexBufferBindingMap::const_iterator i = mBindingMap.find(index);
		if (i == mBindingMap.end())
		{
			CM_EXCEPT(ItemIdentityException, "No buffer is bound to that index.");
		}
		return i->second;
	}
	//-----------------------------------------------------------------------------
	bool VertexBufferBinding::isBufferBound(unsigned short index) const
	{
		return mBindingMap.find(index) != mBindingMap.end();
	}
    //-----------------------------------------------------------------------------
    unsigned short VertexBufferBinding::getLastBoundIndex(void) const
    {
        return mBindingMap.empty() ? 0 : mBindingMap.rbegin()->first + 1;
    }
    //-----------------------------------------------------------------------------
    bool VertexBufferBinding::hasGaps(void) const
    {
        if (mBindingMap.empty())
            return false;
        if (mBindingMap.rbegin()->first + 1 == (int) mBindingMap.size())
            return false;
        return true;
    }
    //-----------------------------------------------------------------------------
    void VertexBufferBinding::closeGaps(BindingIndexMap& bindingIndexMap)
    {
        bindingIndexMap.clear();

        VertexBufferBindingMap newBindingMap;

        VertexBufferBindingMap::const_iterator it;
        UINT16 targetIndex = 0;
        for (it = mBindingMap.begin(); it != mBindingMap.end(); ++it, ++targetIndex)
        {
            bindingIndexMap[it->first] = targetIndex;
            newBindingMap[targetIndex] = it->second;
        }

        mBindingMap.swap(newBindingMap);
        mHighIndex = targetIndex;
    }
}
