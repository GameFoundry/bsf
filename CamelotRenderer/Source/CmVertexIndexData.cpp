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

#include "CmVertexIndexData.h"
#include "CmHardwareBufferManager.h"
#include "CmVertexBuffer.h"
#include "CmIndexBuffer.h"
#include "CmVector3.h"
#include "CmAxisAlignedBox.h"
#include "CmException.h"
#include "CmRenderSystem.h"

namespace CamelotEngine 
{
	VertexData::VertexData(HardwareBufferManager* mgr)
	{
		mMgr = mgr ? mgr : HardwareBufferManager::instancePtr();
		vertexDeclaration = mMgr->createVertexDeclaration();
		vertexCount = 0;

	}

	VertexData::VertexData(VertexDeclarationPtr dcl)
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

	IndexData::IndexData()
	{
		indexCount = 0;
		indexStart = 0;
	}

	IndexData::~IndexData()
	{
	}

	IndexData* IndexData::clone(bool copyData, HardwareBufferManager* mgr) const
	{
		HardwareBufferManager* pManager = mgr ? mgr : HardwareBufferManager::instancePtr();
		IndexData* dest = new IndexData();
		if (indexBuffer.get())
		{
            if (copyData)
            {
			    dest->indexBuffer = pManager->createIndexBuffer(indexBuffer->getType(), indexBuffer->getNumIndexes(),
				    indexBuffer->getUsage());
			    dest->indexBuffer->copyData(*indexBuffer, 0, 0, indexBuffer->getSizeInBytes(), true);
            }
            else
            {
                dest->indexBuffer = indexBuffer;
            }
        }
		dest->indexCount = indexCount;
		dest->indexStart = indexStart;
		return dest;
	}

	// Local Utility class for vertex cache optimizer
	class Triangle
    {
    public:
		enum EdgeMatchType {
			AB, BC, CA, ANY, NONE
		};

		UINT32 a, b, c;		

		inline Triangle()
		{
		}

		inline Triangle( UINT32 ta, UINT32 tb, UINT32 tc ) 
			: a( ta ), b( tb ), c( tc )
		{
		}

		inline Triangle( UINT32 t[3] )
			: a( t[0] ), b( t[1] ), c( t[2] )
		{
		}

		inline Triangle( const Triangle& t )
			: a( t.a ), b( t.b ), c( t.c )
		{
		}

		inline bool sharesEdge(const Triangle& t) const
		{
			return(	a == t.a && b == t.c ||
					a == t.b && b == t.a ||
					a == t.c && b == t.b ||
					b == t.a && c == t.c ||
					b == t.b && c == t.a ||
					b == t.c && c == t.b ||
					c == t.a && a == t.c ||
					c == t.b && a == t.a ||
					c == t.c && a == t.b );
		}

		inline bool sharesEdge(const UINT32 ea, const UINT32 eb, const Triangle& t) const
		{
			return(	ea == t.a && eb == t.c ||
					ea == t.b && eb == t.a ||
					ea == t.c && eb == t.b );	
		}

		inline bool sharesEdge(const EdgeMatchType edge, const Triangle& t) const
		{
			if (edge == AB)
				return sharesEdge(a, b, t);
			else if (edge == BC)
				return sharesEdge(b, c, t);
			else if (edge == CA)
				return sharesEdge(c, a, t);
			else
				return (edge == ANY) == sharesEdge(t);
		}

		inline EdgeMatchType endoSharedEdge(const Triangle& t) const
		{
			if (sharesEdge(a, b, t)) return AB;
			if (sharesEdge(b, c, t)) return BC;
			if (sharesEdge(c, a, t)) return CA;
			return NONE;
		}

		inline EdgeMatchType exoSharedEdge(const Triangle& t) const
		{
			return t.endoSharedEdge(*this);
		}

		inline void shiftClockwise()
		{
			UINT32 t = a;
			a = c;
			c = b;
			b = t;
		}

		inline void shiftCounterClockwise()
		{
			UINT32 t = a;
			a = b;
			b = c;
			c = t;
		}
	};

	void IndexData::optimiseVertexCacheTriList(void)
	{
		if (indexBuffer->isLocked()) return;

		void *buffer = indexBuffer->lock(GBL_READ_WRITE);

		Triangle* triangles;
		UINT32 *dest;

		UINT32 nIndexes = indexCount;
		UINT32 nTriangles = nIndexes / 3;
		UINT32 i, j;
		UINT16 *source = 0;

		if (indexBuffer->getType() == IndexBuffer::IT_16BIT)
		{
			triangles = (Triangle*) malloc(sizeof(Triangle) * nTriangles);
			source = (UINT16 *)buffer;
			dest = (UINT32 *)triangles;
			for (i = 0; i < nIndexes; ++i) dest[i] = source[i];
		}
		else
			triangles = (Triangle*)buffer;

		// sort triangles based on shared edges
		UINT32 *destlist = (UINT32*)malloc(sizeof(UINT32) * nTriangles);
		unsigned char *visited = (unsigned char*)malloc(sizeof(unsigned char) * nTriangles);

		for (i = 0; i < nTriangles; ++i) visited[i] = 0;

		UINT32 start = 0, ti = 0, destcount = 0;

		bool found = false;
		for (i = 0; i < nTriangles; ++i)
		{
			if (found)
				found = false;
			else
			{
				while (visited[start++]);
				ti = start - 1;
			}

			destlist[destcount++] = ti;
			visited[ti] = 1;

			for (j = start; j < nTriangles; ++j)
			{
				if (visited[j]) continue;
				
				if (triangles[ti].sharesEdge(triangles[j]))
				{
					found = true;
					ti = static_cast<UINT32>(j);
					break;
				}
			}
		}

		if (indexBuffer->getType() == IndexBuffer::IT_16BIT)
		{
			// reorder the indexbuffer
			j = 0;
			for (i = 0; i < nTriangles; ++i)
			{
				Triangle *t = &triangles[destlist[i]];
				source[j++] = (UINT16)t->a;
				source[j++] = (UINT16)t->b;
				source[j++] = (UINT16)t->c;
			}
			free(triangles);
		}
		else
		{
			UINT32 *reflist = (UINT32*)malloc(sizeof(UINT32) * nTriangles);

			// fill the referencebuffer
			for (i = 0; i < nTriangles; ++i)
				reflist[destlist[i]] = static_cast<UINT32>(i);
			
			// reorder the indexbuffer
			for (i = 0; i < nTriangles; ++i)
			{
				j = destlist[i];
				if (i == j) continue; // do not move triangle

				// swap triangles

				Triangle t = triangles[i];
				triangles[i] = triangles[j];
				triangles[j] = t;

				// change reference
				destlist[reflist[i]] = static_cast<UINT32>(j);
				// destlist[i] = i; // not needed, it will not be used
			}

			free(reflist);
		}

		free(destlist);
		free(visited);
					
		indexBuffer->unlock();
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
