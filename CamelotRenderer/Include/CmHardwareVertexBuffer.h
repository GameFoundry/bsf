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
#include "CmIReflectable.h"
#include "CmColor.h"

namespace CamelotEngine {
	class HardwareBufferManagerBase;

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/
	/** Specialisation of HardwareBuffer for a vertex buffer. */
    class CM_EXPORT HardwareVertexBuffer : public HardwareBuffer
    {
	    protected:
			HardwareBufferManagerBase* mMgr;
		    UINT32 mNumVertices;
            UINT32 mVertexSize;

	    public:
		    /// Should be called by HardwareBufferManager
		    HardwareVertexBuffer(HardwareBufferManagerBase* mgr, UINT32 vertexSize, UINT32 numVertices,
                HardwareBuffer::Usage usage, bool useSystemMemory);
            ~HardwareVertexBuffer();
			/// Return the manager of this buffer, if any
			HardwareBufferManagerBase* getManager() const { return mMgr; }
            /// Gets the size in bytes of a single vertex in this buffer
            UINT32 getVertexSize(void) const { return mVertexSize; }
            /// Get the number of vertices in this buffer
            UINT32 getNumVertices(void) const { return mNumVertices; }
    };

	typedef std::shared_ptr<HardwareVertexBuffer> HardwareVertexBufferPtr;

	/** Records the state of all the vertex buffer bindings required to provide a vertex declaration
		with the input data it needs for the vertex elements.
	@remarks
		Why do we have this binding list rather than just have VertexElement referring to the
		vertex buffers direct? Well, in the underlying APIs, binding the vertex buffers to an
		index (or 'stream') is the way that vertex data is linked, so this structure better
		reflects the realities of that. In addition, by separating the vertex declaration from
		the list of vertex buffer bindings, it becomes possible to reuse bindings between declarations
		and vice versa, giving opportunities to reduce the state changes required to perform rendering.
	@par
		Like the other classes in this functional area, these binding maps should be created and
		destroyed using the HardwareBufferManager.
	*/
	class CM_EXPORT VertexBufferBinding
	{
	public:
		/// Defines the vertex buffer bindings used as source for vertex declarations
		typedef map<unsigned short, HardwareVertexBufferPtr>::type VertexBufferBindingMap;
	protected:
		VertexBufferBindingMap mBindingMap;
		mutable unsigned short mHighIndex;
	public:
		/// Constructor, should not be called direct, use HardwareBufferManager::createVertexBufferBinding
		VertexBufferBinding();
		virtual ~VertexBufferBinding();
		/** Set a binding, associating a vertex buffer with a given index.
		@remarks
			If the index is already associated with a vertex buffer,
            the association will be replaced. This may cause the old buffer
            to be destroyed if nothing else is referring to it.
			You should assign bindings from 0 and not leave gaps, although you can
			bind them in any order.
		*/
		virtual void setBinding(unsigned short index, const HardwareVertexBufferPtr& buffer);
		/** Removes an existing binding. */
		virtual void unsetBinding(unsigned short index);

        /** Removes all the bindings. */
        virtual void unsetAllBindings(void);

		/// Gets a read-only version of the buffer bindings
		virtual const VertexBufferBindingMap& getBindings(void) const;

		/// Gets the buffer bound to the given source index
		virtual const HardwareVertexBufferPtr& getBuffer(unsigned short index) const;
		/// Gets whether a buffer is bound to the given source index
		virtual bool isBufferBound(unsigned short index) const;

        virtual UINT32 getBufferCount(void) const { return (UINT32)mBindingMap.size(); }

		/** Gets the highest index which has already been set, plus 1.
		@remarks
			This is to assist in binding the vertex buffers such that there are
			not gaps in the list.
		*/
		virtual unsigned short getNextIndex(void) const { return mHighIndex++; }

        /** Gets the last bound index.
        */
        virtual unsigned short getLastBoundIndex(void) const;

        typedef map<UINT16, UINT16>::type BindingIndexMap;

        /** Check whether any gaps in the bindings.
        */
        virtual bool hasGaps(void) const;

        /** Remove any gaps in the bindings.
        @remarks
            This is useful if you've removed vertex buffer from this vertex buffer
            bindings and want to remove any gaps in the bindings. Note, however,
            that if this bindings is already being used with a VertexDeclaration,
            you will need to alter that too. This method is mainly useful when
            reorganising buffers manually.
        @param
            bindingIndexMap To be retrieve the binding index map that used to
            translation old index to new index; will be cleared by this method
            before fill-in.
        */
        virtual void closeGaps(BindingIndexMap& bindingIndexMap);


	};
	/** @} */
	/** @} */



}
#endif

