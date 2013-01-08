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

#include "CmVertexBuffer.h"
#include "CmColor.h"
#include "CmException.h"
#include "CmHardwareBufferManager.h"
#include "CmRenderSystem.h"
#include "CmVertexDeclarationRTTI.h"

namespace CamelotEngine {

    //-----------------------------------------------------------------------------
    VertexBuffer::VertexBuffer(HardwareBufferManager* mgr, UINT32 vertexSize,  
        UINT32 numVertices, GpuBufferUsage usage, 
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
    VertexBuffer::~VertexBuffer()
    {
		if (mMgr)
		{
			mMgr->_notifyVertexBufferDestroyed(this);
		}
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
