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
#ifndef __HardwareVertexBuffer__
#define __HardwareVertexBuffer__

// Precompiler options
#include "CmPrerequisites.h"
#include "CmHardwareBuffer.h"
#include "CmCoreObject.h"
#include "CmColor.h"

namespace CamelotFramework 
{
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/
	/** Specialisation of HardwareBuffer for a vertex buffer. */
    class CM_EXPORT VertexBuffer : public HardwareBuffer, public CoreObject
    {
	public:
        ~VertexBuffer();

		/// Return the manager of this buffer, if any
		HardwareBufferManager* getManager() const { return mMgr; }
        /// Gets the size in bytes of a single vertex in this buffer
        UINT32 getVertexSize(void) const { return mVertexSize; }
        /// Get the number of vertices in this buffer
        UINT32 getNumVertices(void) const { return mNumVertices; }

		/**
		 * @brief	Some render systems expect vertex color bits in an order different than
		 * 			RGBA, in which case override this to flip the RGB order.
		 */
		virtual bool vertexColorReqRGBFlip() { return false; }

		static const int MAX_SEMANTIC_IDX = 8;
	protected:
		HardwareBufferManager* mMgr;
		UINT32 mNumVertices;
		UINT32 mVertexSize;

		VertexBuffer(HardwareBufferManager* mgr, UINT32 vertexSize, UINT32 numVertices, GpuBufferUsage usage, bool useSystemMemory);
    };
}
#endif

