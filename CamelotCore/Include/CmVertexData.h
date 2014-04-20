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
#ifndef __VertexIndexData_H__
#define __VertexIndexData_H__

#include "CmPrerequisites.h"
#include "CmVertexDeclaration.h"
#include "CmVertexBuffer.h"

namespace BansheeEngine {
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/

	/** Summary class collecting together vertex source information. */
	class CM_EXPORT VertexData
	{
    public:
		/** Constructor.
		@note 
			This constructor creates the VertexDeclaration and VertexBufferBinding
			automatically, and arranges for their deletion afterwards.
		@param mgr Optional HardwareBufferManager from which to create resources
		*/
        VertexData(HardwareBufferManager* mgr = 0);
		/** Constructor.
		@note 
		This constructor receives the VertexDeclaration from the caller.
		@param dcl The VertexDeclaration to use
		*/
		VertexData(VertexDeclarationPtr dcl);
        ~VertexData();

		/** Declaration of the vertex to be used in this operation. 
		@remarks Note that this is created for you on construction.
		*/
		VertexDeclarationPtr vertexDeclaration;

		/// The number of vertices used in this operation
		UINT32 vertexCount;

		void setBuffer(UINT32 index, VertexBufferPtr buffer);
		/// Gets the buffer bound to the given source index
		VertexBufferPtr getBuffer(UINT32 index) const;
		const UnorderedMap<UINT32, VertexBufferPtr>::type& getBuffers() const { return mVertexBuffers; }
		/// Gets whether a buffer is bound to the given source index
		bool isBufferBound(UINT32 index) const;

		UINT32 getBufferCount(void) const { return (UINT32)mVertexBuffers.size(); }
		UINT32 getMaxBufferIndex(void) const { return (UINT32)mVertexBuffers.size(); }

		/** Convert all packed colour values (VET_COLOUR_*) in buffers used to
			another type.
		@param srcType The source colour type to assume if the ambiguous VET_COLOUR
			is encountered.
		@param destType The destination colour type, must be VET_COLOUR_ABGR or
			VET_COLOUR_ARGB.
		*/
		void convertPackedColour(VertexElementType srcType, VertexElementType destType);

    private:
        /// Protected copy constructor, to prevent misuse
        VertexData(const VertexData& rhs); /* do nothing, should not use */
        /// Protected operator=, to prevent misuse
        VertexData& operator=(const VertexData& rhs); /* do not use */

		HardwareBufferManager* mMgr;
		bool mOwnsDeclaration;

		/** The vertex buffer bindings to be used. 
		@remarks Note that this is created for you on construction.
		*/
		UnorderedMap<UINT32, VertexBufferPtr>::type mVertexBuffers;
	};

	/** Vertex cache profiler.
	@remarks
		Utility class for evaluating the effectiveness of the use of the vertex
		cache by a given index buffer.
	*/
	class CM_EXPORT VertexCacheProfiler
    {
		public:
			enum CacheType {
				FIFO, LRU
			};

			VertexCacheProfiler(unsigned int cachesize = 16, CacheType cachetype = FIFO )
				: size ( cachesize ), type ( cachetype ), tail (0), buffersize (0), hit (0), miss (0)
			{
				cache = (UINT32*)malloc(sizeof(UINT32) * size);
			}

			~VertexCacheProfiler()
			{
				free(cache);
			}

			void profile(const IndexBufferPtr& indexBuffer);
			void reset() { hit = 0; miss = 0; tail = 0; buffersize = 0; }
			void flush() { tail = 0; buffersize = 0; }

			unsigned int getHits() { return hit; }
			unsigned int getMisses() { return miss; }
			unsigned int getSize() { return size; }
		private:
			unsigned int size;
			UINT32 *cache;
			CacheType type;

			unsigned int tail, buffersize;
			unsigned int hit, miss;

			bool inCache(unsigned int index);
	};
	/** @} */
	/** @} */
}
#endif

