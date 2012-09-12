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

#include "CmHardwareBufferManager.h"
#include "CmVertexIndexData.h"


namespace CamelotEngine {

    //-----------------------------------------------------------------------
    template<> HardwareBufferManager* Singleton<HardwareBufferManager>::ms_Singleton = 0;
    HardwareBufferManager* HardwareBufferManager::getSingletonPtr(void)
    {
        return ms_Singleton;
    }
    HardwareBufferManager& HardwareBufferManager::getSingleton(void)
    {  
        assert( ms_Singleton );  return ( *ms_Singleton );  
    }
	//---------------------------------------------------------------------
	HardwareBufferManager::HardwareBufferManager(HardwareBufferManagerBase* imp)
		: HardwareBufferManagerBase(), mImpl(imp)
	{

	}
	//---------------------------------------------------------------------
	HardwareBufferManager::~HardwareBufferManager()
	{
		// mImpl must be deleted by the creator
	}
    //-----------------------------------------------------------------------
    HardwareBufferManagerBase::HardwareBufferManagerBase()
    {
    }
    //-----------------------------------------------------------------------
    HardwareBufferManagerBase::~HardwareBufferManagerBase()
    {
        // Clear vertex/index buffer list first, avoid destroyed notify do
        // unnecessary work, and we'll destroy everything here.
		mVertexBuffers.clear();
		mIndexBuffers.clear();

        // Destroy everything
        destroyAllDeclarations();
        destroyAllBindings();
        // No need to destroy main buffers - they will be destroyed by removal of bindings

        // No need to destroy temp buffers - they will be destroyed automatically.
    }
    //-----------------------------------------------------------------------
    VertexDeclaration* HardwareBufferManagerBase::createVertexDeclaration(void)
    {
        VertexDeclaration* decl = createVertexDeclarationImpl();
		CM_LOCK_MUTEX(mVertexDeclarationsMutex)
        mVertexDeclarations.insert(decl);
        return decl;
    }
    //-----------------------------------------------------------------------
    void HardwareBufferManagerBase::destroyVertexDeclaration(VertexDeclaration* decl)
    {
		CM_LOCK_MUTEX(mVertexDeclarationsMutex)
        mVertexDeclarations.erase(decl);
        destroyVertexDeclarationImpl(decl);
    }
    //-----------------------------------------------------------------------
	VertexBufferBinding* HardwareBufferManagerBase::createVertexBufferBinding(void)
	{
		VertexBufferBinding* ret = createVertexBufferBindingImpl();
		CM_LOCK_MUTEX(mVertexBufferBindingsMutex)
		mVertexBufferBindings.insert(ret);
		return ret;
	}
    //-----------------------------------------------------------------------
	void HardwareBufferManagerBase::destroyVertexBufferBinding(VertexBufferBinding* binding)
	{
		CM_LOCK_MUTEX(mVertexBufferBindingsMutex)
		mVertexBufferBindings.erase(binding);
		destroyVertexBufferBindingImpl(binding);
	}
    //-----------------------------------------------------------------------
    VertexDeclaration* HardwareBufferManagerBase::createVertexDeclarationImpl(void)
    {
        return new VertexDeclaration();
    }
    //-----------------------------------------------------------------------
    void HardwareBufferManagerBase::destroyVertexDeclarationImpl(VertexDeclaration* decl)
    {
        delete decl;
    }
    //-----------------------------------------------------------------------
	VertexBufferBinding* HardwareBufferManagerBase::createVertexBufferBindingImpl(void)
	{
		return new VertexBufferBinding();
	}
    //-----------------------------------------------------------------------
	void HardwareBufferManagerBase::destroyVertexBufferBindingImpl(VertexBufferBinding* binding)
	{
		delete binding;
	}
    //-----------------------------------------------------------------------
    void HardwareBufferManagerBase::destroyAllDeclarations(void)
    {
		CM_LOCK_MUTEX(mVertexDeclarationsMutex)
        VertexDeclarationList::iterator decl;
        for (decl = mVertexDeclarations.begin(); decl != mVertexDeclarations.end(); ++decl)
        {
            destroyVertexDeclarationImpl(*decl);
        }
        mVertexDeclarations.clear();
    }
    //-----------------------------------------------------------------------
    void HardwareBufferManagerBase::destroyAllBindings(void)
    {
		CM_LOCK_MUTEX(mVertexBufferBindingsMutex)
        VertexBufferBindingList::iterator bind;
        for (bind = mVertexBufferBindings.begin(); bind != mVertexBufferBindings.end(); ++bind)
        {
            destroyVertexBufferBindingImpl(*bind);
        }
        mVertexBufferBindings.clear();
    }
	//-----------------------------------------------------------------------
    HardwareVertexBufferPtr 
    HardwareBufferManagerBase::allocateVertexBufferCopy(
        const HardwareVertexBufferPtr& sourceBuffer, 
        bool copyData)
    {
		// pre-lock the mVertexBuffers mutex, which would usually get locked in
		//  makeBufferCopy / createVertexBuffer
		// this prevents a deadlock in _notifyVertexBufferDestroyed
		// which locks the same mutexes (via other methods) but in reverse order
		CM_LOCK_MUTEX(mVertexBuffersMutex)
		{
			CM_LOCK_MUTEX(mTempBuffersMutex)
			HardwareVertexBufferPtr vbuf;

			// Locate existing buffer copy in temporary vertex buffers
			// copy buffer, use shadow buffer and make dynamic
			vbuf = makeBufferCopy(
				sourceBuffer, 
				HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE, 
				true);

			// Copy data?
			if (copyData)
			{
				vbuf->copyData(*(sourceBuffer.get()), 0, 0, sourceBuffer->getSizeInBytes(), true);
			}

			return vbuf;
		}

    }
	//-----------------------------------------------------------------------
	void HardwareBufferManagerBase::_notifyVertexBufferDestroyed(HardwareVertexBuffer* buf)
	{
		CM_LOCK_MUTEX(mVertexBuffersMutex)

		VertexBufferList::iterator i = mVertexBuffers.find(buf);
		if (i != mVertexBuffers.end())
		{
            // release vertex buffer copies
			mVertexBuffers.erase(i);
		}
	}
	//-----------------------------------------------------------------------
	void HardwareBufferManagerBase::_notifyIndexBufferDestroyed(HardwareIndexBuffer* buf)
	{
		CM_LOCK_MUTEX(mIndexBuffersMutex)

		IndexBufferList::iterator i = mIndexBuffers.find(buf);
		if (i != mIndexBuffers.end())
		{
			mIndexBuffers.erase(i);
		}
	}
    //-----------------------------------------------------------------------
    HardwareVertexBufferPtr 
    HardwareBufferManagerBase::makeBufferCopy(
        const HardwareVertexBufferPtr& source,
        HardwareBuffer::Usage usage, bool useShadowBuffer)
    {
        return this->createVertexBuffer(
            source->getVertexSize(), 
            source->getNumVertices(),
            usage, useShadowBuffer);
    }
}
