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
#include "CmVertexDeclaration.h"

namespace CamelotEngine {
    //-----------------------------------------------------------------------
    HardwareBufferManager::HardwareBufferManager()
    {
    }
    //-----------------------------------------------------------------------
    HardwareBufferManager::~HardwareBufferManager()
    {
        // Clear vertex/index buffer list first, avoid destroyed notify do
        // unnecessary work, and we'll destroy everything here.
		mVertexBuffers.clear();
		mIndexBuffers.clear();

        // Destroy everything
        destroyAllBindings();
        // No need to destroy main buffers - they will be destroyed by removal of bindings

        // No need to destroy temp buffers - they will be destroyed automatically.
    }
    //-----------------------------------------------------------------------
    VertexDeclarationPtr HardwareBufferManager::createVertexDeclaration(void)
    {
        VertexDeclarationPtr decl = createVertexDeclarationImpl();
        return decl;
    }
    //-----------------------------------------------------------------------
	VertexBufferBinding* HardwareBufferManager::createVertexBufferBinding(void)
	{
		VertexBufferBinding* ret = createVertexBufferBindingImpl();
		mVertexBufferBindings.insert(ret);
		return ret;
	}
    //-----------------------------------------------------------------------
	void HardwareBufferManager::destroyVertexBufferBinding(VertexBufferBinding* binding)
	{
		mVertexBufferBindings.erase(binding);
		destroyVertexBufferBindingImpl(binding);
	}
    //-----------------------------------------------------------------------
    VertexDeclarationPtr HardwareBufferManager::createVertexDeclarationImpl(void)
    {
        return VertexDeclarationPtr(new VertexDeclaration());
    }
    //-----------------------------------------------------------------------
	VertexBufferBinding* HardwareBufferManager::createVertexBufferBindingImpl(void)
	{
		return new VertexBufferBinding();
	}
    //-----------------------------------------------------------------------
	void HardwareBufferManager::destroyVertexBufferBindingImpl(VertexBufferBinding* binding)
	{
		delete binding;
	}
    //-----------------------------------------------------------------------
    void HardwareBufferManager::destroyAllBindings(void)
    {
        VertexBufferBindingList::iterator bind;
        for (bind = mVertexBufferBindings.begin(); bind != mVertexBufferBindings.end(); ++bind)
        {
            destroyVertexBufferBindingImpl(*bind);
        }
        mVertexBufferBindings.clear();
    }
	//-----------------------------------------------------------------------
	void HardwareBufferManager::_notifyVertexBufferDestroyed(VertexBuffer* buf)
	{
		VertexBufferList::iterator i = mVertexBuffers.find(buf);
		if (i != mVertexBuffers.end())
		{
            // release vertex buffer copies
			mVertexBuffers.erase(i);
		}
	}
	//-----------------------------------------------------------------------
	void HardwareBufferManager::_notifyIndexBufferDestroyed(IndexBuffer* buf)
	{
		IndexBufferList::iterator i = mIndexBuffers.find(buf);
		if (i != mIndexBuffers.end())
		{
			mIndexBuffers.erase(i);
		}
	}
}
