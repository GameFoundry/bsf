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
#include "CmHardwareVertexBuffer.h"
#include "CmHardwareIndexBuffer.h"
#include "CmVector3.h"
#include "CmAxisAlignedBox.h"
#include "CmException.h"
#include "CmRenderSystem.h"
#include "CmRenderSystemManager.h"

namespace CamelotEngine {

    //-----------------------------------------------------------------------
	VertexData::VertexData(HardwareBufferManagerBase* mgr)
	{
		mMgr = mgr ? mgr : HardwareBufferManager::instancePtr();
		vertexBufferBinding = mMgr->createVertexBufferBinding();
		vertexDeclaration = mMgr->createVertexDeclaration();
		mDeleteDclBinding = true;
		vertexCount = 0;
		vertexStart = 0;

	}
	//---------------------------------------------------------------------
	VertexData::VertexData(VertexDeclaration* dcl, VertexBufferBinding* bind)
	{
		// this is a fallback rather than actively used
		mMgr = HardwareBufferManager::instancePtr();
		vertexDeclaration = dcl;
		vertexBufferBinding = bind;
		mDeleteDclBinding = false;
		vertexCount = 0;
		vertexStart = 0;
	}
    //-----------------------------------------------------------------------
	VertexData::~VertexData()
	{
		if (mDeleteDclBinding)
		{
			mMgr->destroyVertexBufferBinding(vertexBufferBinding);
			mMgr->destroyVertexDeclaration(vertexDeclaration);
		}
	}
    //-----------------------------------------------------------------------
	VertexData* VertexData::clone(bool copyData, HardwareBufferManagerBase* mgr) const
	{
		HardwareBufferManagerBase* pManager = mgr ? mgr : mMgr;

		VertexData* dest = new VertexData(mgr);

		// Copy vertex buffers in turn
		const VertexBufferBinding::VertexBufferBindingMap& bindings = 
			this->vertexBufferBinding->getBindings();
		VertexBufferBinding::VertexBufferBindingMap::const_iterator vbi, vbend;
		vbend = bindings.end();
		for (vbi = bindings.begin(); vbi != vbend; ++vbi)
		{
			HardwareVertexBufferPtr srcbuf = vbi->second;
            HardwareVertexBufferPtr dstBuf;
            if (copyData)
            {
			    // create new buffer with the same settings
			    dstBuf = pManager->createVertexBuffer(
					    srcbuf->getVertexSize(), srcbuf->getNumVertices(), srcbuf->getUsage(),
					    srcbuf->hasShadowBuffer());

			    // copy data
			    dstBuf->copyData(*srcbuf, 0, 0, srcbuf->getSizeInBytes(), true);
            }
            else
            {
                // don't copy, point at existing buffer
                dstBuf = srcbuf;
            }

			// Copy binding
			dest->vertexBufferBinding->setBinding(vbi->first, dstBuf);
        }

        // Basic vertex info
        dest->vertexStart = this->vertexStart;
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

		// Copy reference to hardware shadow buffer, no matter whether copy data or not
        dest->hardwareShadowVolWBuffer = hardwareShadowVolWBuffer;
        
        return dest;
	}
    //-----------------------------------------------------------------------
    void VertexData::prepareForShadowVolume(void)
    {
        /* NOTE
        I would dearly, dearly love to just use a 4D position buffer in order to 
        store the extra 'w' value I need to differentiate between extruded and 
        non-extruded sections of the buffer, so that vertex programs could use that.
        Hey, it works fine for GL. However, D3D9 in it's infinite stupidity, does not
        support 4d position vertices in the fixed-function pipeline. If you use them, 
        you just see nothing. Since we can't know whether the application is going to use
        fixed function or vertex programs, we have to stick to 3d position vertices and
        store the 'w' in a separate 1D texture coordinate buffer, which is only used
        when rendering the shadow.
        */

        // Upfront, lets check whether we have vertex program capability
        bool useVertexPrograms = false;

		RenderSystem* rend = CamelotEngine::RenderSystemManager::getActive();
        if (rend && rend->getCapabilities()->hasCapability(RSC_VERTEX_PROGRAM))
        {
            useVertexPrograms = true;
        }


        // Look for a position element
        const VertexElement* posElem = vertexDeclaration->findElementBySemantic(VES_POSITION);
        if (posElem)
        {
            size_t v;
            unsigned short posOldSource = posElem->getSource();

            HardwareVertexBufferPtr vbuf = vertexBufferBinding->getBuffer(posOldSource);
            bool wasSharedBuffer = false;
            // Are there other elements in the buffer except for the position?
            if (vbuf->getVertexSize() > posElem->getSize())
            {
                // We need to create another buffer to contain the remaining elements
                // Most drivers don't like gaps in the declaration, and in any case it's waste
                wasSharedBuffer = true;
            }
            HardwareVertexBufferPtr newPosBuffer, newRemainderBuffer;
            if (wasSharedBuffer)
            {
                newRemainderBuffer = vbuf->getManager()->createVertexBuffer(
                    vbuf->getVertexSize() - posElem->getSize(), vbuf->getNumVertices(), vbuf->getUsage(),
                    vbuf->hasShadowBuffer());
            }
            // Allocate new position buffer, will be FLOAT3 and 2x the size
            size_t oldVertexCount = vbuf->getNumVertices();
            size_t newVertexCount = oldVertexCount * 2;
            newPosBuffer = vbuf->getManager()->createVertexBuffer(
                VertexElement::getTypeSize(VET_FLOAT3), newVertexCount, vbuf->getUsage(), 
                vbuf->hasShadowBuffer());

            // Iterate over the old buffer, copying the appropriate elements and initialising the rest
            float* pSrc;
            unsigned char *pBaseSrc = static_cast<unsigned char*>(
                vbuf->lock(HardwareBuffer::HBL_READ_ONLY));
            // Point first destination pointer at the start of the new position buffer,
            // the other one half way along
            float *pDest = static_cast<float*>(newPosBuffer->lock(HardwareBuffer::HBL_DISCARD));
            float* pDest2 = pDest + oldVertexCount * 3; 

            // Precalculate any dimensions of vertex areas outside the position
            size_t prePosVertexSize = 0, postPosVertexSize, postPosVertexOffset;
            unsigned char *pBaseDestRem = 0;
            if (wasSharedBuffer)
            {
                pBaseDestRem = static_cast<unsigned char*>(
                    newRemainderBuffer->lock(HardwareBuffer::HBL_DISCARD));
                prePosVertexSize = posElem->getOffset();
                postPosVertexOffset = prePosVertexSize + posElem->getSize();
                postPosVertexSize = vbuf->getVertexSize() - postPosVertexOffset;
                // the 2 separate bits together should be the same size as the remainder buffer vertex
                assert (newRemainderBuffer->getVertexSize() == prePosVertexSize + postPosVertexSize);

                // Iterate over the vertices
                for (v = 0; v < oldVertexCount; ++v)
                {
                    // Copy position, into both buffers
                    posElem->baseVertexPointerToElement(pBaseSrc, &pSrc);
                    *pDest++ = *pDest2++ = *pSrc++;
                    *pDest++ = *pDest2++ = *pSrc++;
                    *pDest++ = *pDest2++ = *pSrc++;

                    // now deal with any other elements 
                    // Basically we just memcpy the vertex excluding the position
                    if (prePosVertexSize > 0)
                        memcpy(pBaseDestRem, pBaseSrc, prePosVertexSize);
                    if (postPosVertexSize > 0)
                        memcpy(pBaseDestRem + prePosVertexSize, 
                            pBaseSrc + postPosVertexOffset, postPosVertexSize);
                    pBaseDestRem += newRemainderBuffer->getVertexSize();

                    pBaseSrc += vbuf->getVertexSize();

                } // next vertex
            }
            else
            {
                // Unshared buffer, can block copy the whole thing
                memcpy(pDest, pBaseSrc, vbuf->getSizeInBytes());
                memcpy(pDest2, pBaseSrc, vbuf->getSizeInBytes());
            }

            vbuf->unlock();
            newPosBuffer->unlock();
            if (wasSharedBuffer)
                newRemainderBuffer->unlock();

            if (useVertexPrograms)
            {
                // Now it's time to set up the w buffer
                hardwareShadowVolWBuffer = vbuf->getManager()->createVertexBuffer(
                    sizeof(float), newVertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
                // Fill the first half with 1.0, second half with 0.0
                pDest = static_cast<float*>(
                    hardwareShadowVolWBuffer->lock(HardwareBuffer::HBL_DISCARD));
                for (v = 0; v < oldVertexCount; ++v)
                {
                    *pDest++ = 1.0f;
                }
                for (v = 0; v < oldVertexCount; ++v)
                {
                    *pDest++ = 0.0f;
                }
                hardwareShadowVolWBuffer->unlock();
            }

            unsigned short newPosBufferSource; 
            if (wasSharedBuffer)
            {
                // Get the a new buffer binding index
                newPosBufferSource= vertexBufferBinding->getNextIndex();
                // Re-bind the old index to the remainder buffer
                vertexBufferBinding->setBinding(posOldSource, newRemainderBuffer);
            }
            else
            {
                // We can just re-use the same source idex for the new position buffer
                newPosBufferSource = posOldSource;
            }
            // Bind the new position buffer
            vertexBufferBinding->setBinding(newPosBufferSource, newPosBuffer);

            // Now, alter the vertex declaration to change the position source
            // and the offsets of elements using the same buffer
            VertexDeclaration::VertexElementList::const_iterator elemi = 
                vertexDeclaration->getElements().begin();
            VertexDeclaration::VertexElementList::const_iterator elemiend = 
                vertexDeclaration->getElements().end();
            unsigned short idx;
            for(idx = 0; elemi != elemiend; ++elemi, ++idx) 
            {
                if (&(*elemi) == posElem)
                {
                    // Modify position to point at new position buffer
                    vertexDeclaration->modifyElement(
                        idx, 
                        newPosBufferSource, // new source buffer
                        0, // no offset now
                        VET_FLOAT3, 
                        VES_POSITION);
                }
                else if (wasSharedBuffer &&
                    elemi->getSource() == posOldSource &&
                    elemi->getOffset() > prePosVertexSize )
                {
                    // This element came after position, remove the position's
                    // size
                    vertexDeclaration->modifyElement(
                        idx, 
                        posOldSource, // same old source
                        elemi->getOffset() - posElem->getSize(), // less offset now
                        elemi->getType(), 
                        elemi->getSemantic(),
                        elemi->getIndex());

                }

            }


            // Note that we don't change vertexCount, because the other buffer(s) are still the same
            // size after all


        }
    }
	//-----------------------------------------------------------------------
	void VertexData::reorganiseBuffers(VertexDeclaration* newDeclaration, 
		const BufferUsageList& bufferUsages, HardwareBufferManagerBase* mgr)
	{
		HardwareBufferManagerBase* pManager = mgr ? mgr : mMgr;
        // Firstly, close up any gaps in the buffer sources which might have arisen
        newDeclaration->closeGapsInSource();

		// Build up a list of both old and new elements in each buffer
		unsigned short buf = 0;
		vector<void*>::type oldBufferLocks;
        vector<size_t>::type oldBufferVertexSizes;
		vector<void*>::type newBufferLocks;
        vector<size_t>::type newBufferVertexSizes;
		VertexBufferBinding* newBinding = pManager->createVertexBufferBinding();
        const VertexBufferBinding::VertexBufferBindingMap& oldBindingMap = vertexBufferBinding->getBindings();
        VertexBufferBinding::VertexBufferBindingMap::const_iterator itBinding;

        // Pre-allocate old buffer locks
        if (!oldBindingMap.empty())
        {
            size_t count = oldBindingMap.rbegin()->first + 1;
            oldBufferLocks.resize(count);
            oldBufferVertexSizes.resize(count);
        }
		// Lock all the old buffers for reading
        for (itBinding = oldBindingMap.begin(); itBinding != oldBindingMap.end(); ++itBinding)
        {
            assert(itBinding->second->getNumVertices() >= vertexCount);

            oldBufferVertexSizes[itBinding->first] =
                itBinding->second->getVertexSize();
            oldBufferLocks[itBinding->first] =
                itBinding->second->lock(
                    HardwareBuffer::HBL_READ_ONLY);
        }
		
		// Create new buffers and lock all for writing
		buf = 0;
		while (!newDeclaration->findElementsBySource(buf).empty())
		{
            size_t vertexSize = newDeclaration->getVertexSize(buf);

			HardwareVertexBufferPtr vbuf = 
				pManager->createVertexBuffer(
					vertexSize,
					vertexCount, 
					bufferUsages[buf]);
			newBinding->setBinding(buf, vbuf);

            newBufferVertexSizes.push_back(vertexSize);
			newBufferLocks.push_back(
				vbuf->lock(HardwareBuffer::HBL_DISCARD));
			buf++;
		}

		// Map from new to old elements
        typedef map<const VertexElement*, const VertexElement*>::type NewToOldElementMap;
		NewToOldElementMap newToOldElementMap;
		const VertexDeclaration::VertexElementList& newElemList = newDeclaration->getElements();
		VertexDeclaration::VertexElementList::const_iterator ei, eiend;
		eiend = newElemList.end();
		for (ei = newElemList.begin(); ei != eiend; ++ei)
		{
			// Find corresponding old element
			const VertexElement* oldElem = 
				vertexDeclaration->findElementBySemantic(
					(*ei).getSemantic(), (*ei).getIndex());
			if (!oldElem)
			{
				// Error, cannot create new elements with this method
				CM_EXCEPT(ItemIdentityException, 
					"Element not found in old vertex declaration");
			}
			newToOldElementMap[&(*ei)] = oldElem;
		}
		// Now iterate over the new buffers, pulling data out of the old ones
		// For each vertex
		for (size_t v = 0; v < vertexCount; ++v)
		{
			// For each (new) element
			for (ei = newElemList.begin(); ei != eiend; ++ei)
			{
				const VertexElement* newElem = &(*ei);
                NewToOldElementMap::iterator noi = newToOldElementMap.find(newElem);
				const VertexElement* oldElem = noi->second;
				unsigned short oldBufferNo = oldElem->getSource();
				unsigned short newBufferNo = newElem->getSource();
				void* pSrcBase = static_cast<void*>(
					static_cast<unsigned char*>(oldBufferLocks[oldBufferNo])
					+ v * oldBufferVertexSizes[oldBufferNo]);
				void* pDstBase = static_cast<void*>(
					static_cast<unsigned char*>(newBufferLocks[newBufferNo])
					+ v * newBufferVertexSizes[newBufferNo]);
				void *pSrc, *pDst;
				oldElem->baseVertexPointerToElement(pSrcBase, &pSrc);
				newElem->baseVertexPointerToElement(pDstBase, &pDst);
				
				memcpy(pDst, pSrc, newElem->getSize());
				
			}
		}

		// Unlock all buffers
        for (itBinding = oldBindingMap.begin(); itBinding != oldBindingMap.end(); ++itBinding)
        {
            itBinding->second->unlock();
        }
        for (buf = 0; buf < newBinding->getBufferCount(); ++buf)
        {
            newBinding->getBuffer(buf)->unlock();
        }

		// Delete old binding & declaration
		if (mDeleteDclBinding)
		{
			pManager->destroyVertexBufferBinding(vertexBufferBinding);
			pManager->destroyVertexDeclaration(vertexDeclaration);
		}

		// Assign new binding and declaration
		vertexDeclaration = newDeclaration;
		vertexBufferBinding = newBinding;		
		// after this is complete, new manager should be used
		mMgr = pManager;
		mDeleteDclBinding = true; // because we created these through a manager

	}
    //-----------------------------------------------------------------------
    void VertexData::reorganiseBuffers(VertexDeclaration* newDeclaration, HardwareBufferManagerBase* mgr)
    {
        // Derive the buffer usages from looking at where the source has come
        // from
        BufferUsageList usages;
        for (unsigned short b = 0; b <= newDeclaration->getMaxSource(); ++b)
        {
            VertexDeclaration::VertexElementList destElems = newDeclaration->findElementsBySource(b);
            // Initialise with most restrictive version 
            // (not really a usable option, but these flags will be removed)
            HardwareBuffer::Usage final = static_cast<HardwareBuffer::Usage>(
                HardwareBuffer::HBU_STATIC_WRITE_ONLY | HardwareBuffer::HBU_DISCARDABLE);
            VertexDeclaration::VertexElementList::iterator v;
            for (v = destElems.begin(); v != destElems.end(); ++v)
            {
                VertexElement& destelem = *v;
                // get source
                const VertexElement* srcelem =
                    vertexDeclaration->findElementBySemantic(
                        destelem.getSemantic(), destelem.getIndex());
                // get buffer
                HardwareVertexBufferPtr srcbuf = 
                    vertexBufferBinding->getBuffer(srcelem->getSource());
                // improve flexibility only
                if (srcbuf->getUsage() & HardwareBuffer::HBU_DYNAMIC)
                {
                    // remove static
                    final = static_cast<HardwareBuffer::Usage>(
                        final & ~HardwareBuffer::HBU_STATIC);
                    // add dynamic
                    final = static_cast<HardwareBuffer::Usage>(
                        final | HardwareBuffer::HBU_DYNAMIC);
                }
                if (!(srcbuf->getUsage() & HardwareBuffer::HBU_WRITE_ONLY))
                {
                    // remove write only
                    final = static_cast<HardwareBuffer::Usage>(
                        final & ~HardwareBuffer::HBU_WRITE_ONLY);
                }
                if (!(srcbuf->getUsage() & HardwareBuffer::HBU_DISCARDABLE))
                {
                    // remove discardable
                    final = static_cast<HardwareBuffer::Usage>(
                        final & ~HardwareBuffer::HBU_DISCARDABLE);
                }
                
            }
            usages.push_back(final);
        }
        // Call specific method
        reorganiseBuffers(newDeclaration, usages, mgr);

    }
    //-----------------------------------------------------------------------
    void VertexData::closeGapsInBindings(void)
    {
        if (!vertexBufferBinding->hasGaps())
            return;

        // Check for error first
        const VertexDeclaration::VertexElementList& allelems = 
            vertexDeclaration->getElements();
        VertexDeclaration::VertexElementList::const_iterator ai;
        for (ai = allelems.begin(); ai != allelems.end(); ++ai)
        {
            const VertexElement& elem = *ai;
            if (!vertexBufferBinding->isBufferBound(elem.getSource()))
            {
                CM_EXCEPT(ItemIdentityException,
                    "No buffer is bound to that element source.");
            }
        }

        // Close gaps in the vertex buffer bindings
        VertexBufferBinding::BindingIndexMap bindingIndexMap;
        vertexBufferBinding->closeGaps(bindingIndexMap);

        // Modify vertex elements to reference to new buffer index
        unsigned short elemIndex = 0;
        for (ai = allelems.begin(); ai != allelems.end(); ++ai, ++elemIndex)
        {
            const VertexElement& elem = *ai;
            VertexBufferBinding::BindingIndexMap::const_iterator it =
                bindingIndexMap.find(elem.getSource());
            assert(it != bindingIndexMap.end());
            UINT16 targetSource = it->second;
            if (elem.getSource() != targetSource)
            {
                vertexDeclaration->modifyElement(elemIndex, 
                    targetSource, elem.getOffset(), elem.getType(), 
                    elem.getSemantic(), elem.getIndex());
            }
        }
    }
    //-----------------------------------------------------------------------
    void VertexData::removeUnusedBuffers(void)
    {
        set<UINT16>::type usedBuffers;

        // Collect used buffers
        const VertexDeclaration::VertexElementList& allelems = 
            vertexDeclaration->getElements();
        VertexDeclaration::VertexElementList::const_iterator ai;
        for (ai = allelems.begin(); ai != allelems.end(); ++ai)
        {
            const VertexElement& elem = *ai;
            usedBuffers.insert(elem.getSource());
        }

        // Unset unused buffer bindings
        UINT16 count = vertexBufferBinding->getLastBoundIndex();
        for (UINT16 index = 0; index < count; ++index)
        {
            if (usedBuffers.find(index) == usedBuffers.end() &&
                vertexBufferBinding->isBufferBound(index))
            {
                vertexBufferBinding->unsetBinding(index);
            }
        }

        // Close gaps
        closeGapsInBindings();
    }
	//-----------------------------------------------------------------------
	void VertexData::convertPackedColour(
		VertexElementType srcType, VertexElementType destType)
	{
		if (destType != VET_COLOUR_ABGR && destType != VET_COLOUR_ARGB)
		{
			CM_EXCEPT(InvalidParametersException,
				"Invalid destType parameter");
		}
		if (srcType != VET_COLOUR_ABGR && srcType != VET_COLOUR_ARGB)
		{
			CM_EXCEPT(InvalidParametersException,
				"Invalid srcType parameter");
		}

		const VertexBufferBinding::VertexBufferBindingMap& bindMap = 
			vertexBufferBinding->getBindings();
		VertexBufferBinding::VertexBufferBindingMap::const_iterator bindi;
		for (bindi = bindMap.begin(); bindi != bindMap.end(); ++bindi)
		{
			VertexDeclaration::VertexElementList elems = 
				vertexDeclaration->findElementsBySource(bindi->first);
			bool conversionNeeded = false;
			VertexDeclaration::VertexElementList::iterator elemi;
			for (elemi = elems.begin(); elemi != elems.end(); ++elemi)
			{
				VertexElement& elem = *elemi;
				if (elem.getType() == VET_COLOUR || 
					((elem.getType() == VET_COLOUR_ABGR || elem.getType() == VET_COLOUR_ARGB) 
					&& elem.getType() != destType))
				{
					conversionNeeded = true;
				}
			}

			if (conversionNeeded)
			{
				void* pBase = bindi->second->lock(HardwareBuffer::HBL_NORMAL);

				for (size_t v = 0; v < bindi->second->getNumVertices(); ++v)
				{

					for (elemi = elems.begin(); elemi != elems.end(); ++elemi)
					{
						VertexElement& elem = *elemi;
						VertexElementType currType = (elem.getType() == VET_COLOUR) ?
							srcType : elem.getType();
						if (elem.getType() == VET_COLOUR || 
							((elem.getType() == VET_COLOUR_ABGR || elem.getType() == VET_COLOUR_ARGB) 
							&& elem.getType() != destType))
						{
							UINT32* pRGBA;
							elem.baseVertexPointerToElement(pBase, &pRGBA);
							VertexElement::convertColourValue(currType, destType, pRGBA);
						}
					}
					pBase = static_cast<void*>(
						static_cast<char*>(pBase) + bindi->second->getVertexSize());
				}
				bindi->second->unlock();

				// Modify the elements to reflect the changed type
				const VertexDeclaration::VertexElementList& allelems = 
					vertexDeclaration->getElements();
				VertexDeclaration::VertexElementList::const_iterator ai;
				unsigned short elemIndex = 0;
				for (ai = allelems.begin(); ai != allelems.end(); ++ai, ++elemIndex)
				{
					const VertexElement& elem = *ai;
					if (elem.getType() == VET_COLOUR || 
						((elem.getType() == VET_COLOUR_ABGR || elem.getType() == VET_COLOUR_ARGB) 
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
    //-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	IndexData::IndexData()
	{
		indexCount = 0;
		indexStart = 0;
		
	}
    //-----------------------------------------------------------------------
	IndexData::~IndexData()
	{
	}
    //-----------------------------------------------------------------------
	IndexData* IndexData::clone(bool copyData, HardwareBufferManagerBase* mgr) const
	{
		HardwareBufferManagerBase* pManager = mgr ? mgr : HardwareBufferManager::instancePtr();
		IndexData* dest = new IndexData();
		if (indexBuffer.get())
		{
            if (copyData)
            {
			    dest->indexBuffer = pManager->createIndexBuffer(indexBuffer->getType(), indexBuffer->getNumIndexes(),
				    indexBuffer->getUsage(), indexBuffer->hasShadowBuffer());
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
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
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
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
	void IndexData::optimiseVertexCacheTriList(void)
	{
		if (indexBuffer->isLocked()) return;

		void *buffer = indexBuffer->lock(HardwareBuffer::HBL_NORMAL);

		Triangle* triangles;
		UINT32 *dest;

		size_t nIndexes = indexCount;
		size_t nTriangles = nIndexes / 3;
		size_t i, j;
		UINT16 *source = 0;

		if (indexBuffer->getType() == HardwareIndexBuffer::IT_16BIT)
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

		if (indexBuffer->getType() == HardwareIndexBuffer::IT_16BIT)
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
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	void VertexCacheProfiler::profile(const HardwareIndexBufferPtr& indexBuffer)
    {
		if (indexBuffer->isLocked()) return;

		UINT16 *shortbuffer = (UINT16 *)indexBuffer->lock(HardwareBuffer::HBL_READ_ONLY);

		if (indexBuffer->getType() == HardwareIndexBuffer::IT_16BIT)
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

	//-----------------------------------------------------------------------
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
