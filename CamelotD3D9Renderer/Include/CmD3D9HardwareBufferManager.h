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
#ifndef __D3D9HARWAREBUFFERMANAGER_H__
#define __D3D9HARWAREBUFFERMANAGER_H__

#include "CmD3D9Prerequisites.h"
#include "CmHardwareBufferManager.h"

namespace CamelotEngine {

    /** Implementation of HardwareBufferManager for D3D9. */
    class CM_D3D9_EXPORT D3D9HardwareBufferManager : public HardwareBufferManager
    {
    protected:     
        /// Internal method for creates a new vertex declaration, may be overridden by certain rendering APIs
        VertexDeclarationPtr createVertexDeclarationImpl(void);
        /// Internal method for destroys a vertex declaration, may be overridden by certain rendering APIs
        void destroyVertexDeclarationImpl(VertexDeclaration* decl);

    public:
        D3D9HardwareBufferManager();
        ~D3D9HardwareBufferManager();

		/**
		 * @copydoc HardwareBufferManager::createVertexBuffer
		 */
		VertexBufferPtr createVertexBuffer(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false);

		/**
		 * @copydoc HardwareBufferManager::createIndexBuffer
		 */
		IndexBufferPtr createIndexBuffer(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage);

		/** @copydoc HardwareBufferManager::createGpuParamBlockBuffer */
		GpuParamBlockBufferPtr createGpuParamBlockBuffer(const GpuParamBlockDesc& paramDesc);

		/**
		 * @copydoc HardwareBufferManager::createGenericBuffer
		 *
		 * DirectX 9 does not support generic buffers so this method will return a dummy instance.
		 */
		GpuBufferPtr createGpuBuffer(UINT32 elementCount, UINT32 elementSize, 
			GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);
    };
}


#endif
