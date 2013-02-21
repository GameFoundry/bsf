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

#include "CmIndexData.h"
#include "CmVertexData.h"
#include "CmHardwareBufferManager.h"
#include "CmVertexBuffer.h"
#include "CmVector3.h"
#include "CmException.h"
#include "CmRenderSystem.h"

namespace CamelotEngine 
{
	VertexData::VertexData(HardwareBufferManager* mgr)
		:mOwnsDeclaration(true)
	{
		mMgr = mgr ? mgr : HardwareBufferManager::instancePtr();
		vertexDeclaration = mMgr->createVertexDeclaration();
		vertexCount = 0;

	}

	VertexData::VertexData(VertexDeclarationPtr dcl)
		:mOwnsDeclaration(false)
	{
		// this is a fallback rather than actively used
		mMgr = HardwareBufferManager::instancePtr();
		vertexDeclaration = dcl;
		vertexCount = 0;
	}

	VertexData::~VertexData()
	{
	}

	void VertexData::setBuffer(UINT32 index, VertexBufferPtr buffer)
	{
		mVertexBuffers[index] = buffer;
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

	VertexData* VertexData::clone(bool copyData, HardwareBufferManager* mgr) const
	{
		HardwareBufferManager* pManager = mgr ? mgr : mMgr;

		VertexData* dest = new VertexData(mgr);

		// Copy vertex buffers in turn
		for (auto iter = mVertexBuffers.begin(); iter != mVertexBuffers.end(); ++iter)
		{
			VertexBufferPtr srcbuf = iter->second;
            VertexBufferPtr dstBuf;
            if (copyData)
            {
			    // create new buffer with the same settings
			    dstBuf = pManager->createVertexBuffer(
					    srcbuf->getVertexSize(), srcbuf->getNumVertices(), srcbuf->getUsage());

			    // copy data
			    dstBuf->copyData(*srcbuf, 0, 0, srcbuf->getSizeInBytes(), true);
            }
            else
            {
                // don't copy, point at existing buffer
                dstBuf = srcbuf;
            }

			// Copy binding
			dest->setBuffer(iter->first, dstBuf);
        }

        // Basic vertex info
		dest->vertexCount = this->vertexCount;
        // Copy elements
        const VertexDeclaration::VertexElementList elems = 
            this->vertexDeclaration->getElements();
        VertexDeclaration::VertexElementList::const_iterator ei, eiend;
        eiend = elems.end();
        for (ei = elems.begin(); ei != eiend; ++ei)
        {
            dest->vertexDeclaration->addElement(
                ei->getSource(),
                ei->getOffset(),
                ei->getType(),
                ei->getSemantic(),
                ei->getIndex() );
        }

        return dest;
	}

	void VertexData::convertPackedColour(
		VertexElementType srcType, VertexElementType destType)
	{
		if (destType != VET_COLOR_ABGR && destType != VET_COLOR_ARGB)
		{
			CM_EXCEPT(InvalidParametersException,
				"Invalid destType parameter");
		}
		if (srcType != VET_COLOR_ABGR && srcType != VET_COLOR_ARGB)
		{
			CM_EXCEPT(InvalidParametersException,
				"Invalid srcType parameter");
		}

		for (auto iter = mVertexBuffers.begin(); iter != mVertexBuffers.end(); ++iter)
		{
			VertexDeclaration::VertexElementList elems = 
				vertexDeclaration->findElementsBySource(iter->first);
			bool conversionNeeded = false;
			VertexDeclaration::VertexElementList::iterator elemi;
			for (elemi = elems.begin(); elemi != elems.end(); ++elemi)
			{
				VertexElement& elem = *elemi;
				if (elem.getType() == VET_COLOR || 
					((elem.getType() == VET_COLOR_ABGR || elem.getType() == VET_COLOR_ARGB) 
					&& elem.getType() != destType))
				{
					conversionNeeded = true;
				}
			}

			if (conversionNeeded)
			{
				void* pBase = iter->second->lock(GBL_READ_WRITE);

				for (UINT32 v = 0; v < iter->second->getNumVertices(); ++v)
				{

					for (elemi = elems.begin(); elemi != elems.end(); ++elemi)
					{
						VertexElement& elem = *elemi;
						VertexElementType currType = (elem.getType() == VET_COLOR) ?
							srcType : elem.getType();
						if (elem.getType() == VET_COLOR || 
							((elem.getType() == VET_COLOR_ABGR || elem.getType() == VET_COLOR_ARGB) 
							&& elem.getType() != destType))
						{
							UINT32* pRGBA;
							elem.baseVertexPointerToElement(pBase, &pRGBA);
							VertexElement::convertColourValue(currType, destType, pRGBA);
						}
					}
					pBase = static_cast<void*>(
						static_cast<char*>(pBase) + iter->second->getVertexSize());
				}
				iter->second->unlock();

				// Modify the elements to reflect the changed type
				const VertexDeclaration::VertexElementList& allelems = 
					vertexDeclaration->getElements();
				VertexDeclaration::VertexElementList::const_iterator ai;
				unsigned short elemIndex = 0;
				for (ai = allelems.begin(); ai != allelems.end(); ++ai, ++elemIndex)
				{
					const VertexElement& elem = *ai;
					if (elem.getType() == VET_COLOR || 
						((elem.getType() == VET_COLOR_ABGR || elem.getType() == VET_COLOR_ARGB) 
						&& elem.getType() != destType))
					{
						vertexDeclaration->modifyElement(elemIndex, 
							elem.getSource(), elem.getOffset(), destType, 
							elem.getSemantic(), elem.getIndex());
					}
				}
			}
		} // each buffer
	}

	void VertexCacheProfiler::profile(const IndexBufferPtr& indexBuffer)
    {
		if (indexBuffer->isLocked()) return;

		UINT16 *shortbuffer = (UINT16 *)indexBuffer->lock(GBL_READ_ONLY);

		if (indexBuffer->getType() == IndexBuffer::IT_16BIT)
			for (unsigned int i = 0; i < indexBuffer->getNumIndexes(); ++i)
				inCache(shortbuffer[i]);
		else
		{
			UINT32 *buffer = (UINT32 *)shortbuffer;
			for (unsigned int i = 0; i < indexBuffer->getNumIndexes(); ++i)
				inCache(buffer[i]);
		}

		indexBuffer->unlock();
	}

	bool VertexCacheProfiler::inCache(unsigned int index)
	{
		for (unsigned int i = 0; i < buffersize; ++i)
		{
			if (index == cache[i])
			{
				hit++;
				return true;
			}
		}

		miss++;
		cache[tail++] = index;
		tail %= size;

		if (buffersize < size) buffersize++;

		return false;
	}
}
