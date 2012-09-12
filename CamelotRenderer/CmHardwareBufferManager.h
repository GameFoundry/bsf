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
#ifndef __HardwareBufferManager__
#define __HardwareBufferManager__

// Precompiler options
#include "OgrePrerequisites.h"

#include "OgreSingleton.h"
#include "CmHardwareVertexBuffer.h"
#include "CmHardwareIndexBuffer.h"

namespace CamelotEngine {
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/

	/** Base definition of a hardware buffer manager.
	@remarks
		This class is deliberately not a Singleton, so that multiple types can 
		exist at once. The Singleton is wrapped via the Decorator pattern
		in HardwareBufferManager, below. Each concrete implementation should
		provide a subclass of HardwareBufferManagerBase, which does the actual
		work, and also a very simple subclass of HardwareBufferManager which 
		simply constructs the instance of the HardwareBufferManagerBase subclass 
		and passes it to the HardwareBufferManager superclass as a delegate. 
		This subclass must also delete the implementation instance it creates.
	*/
	class CM_EXPORT HardwareBufferManagerBase
	{
    protected:
        /** WARNING: The following two members should place before all other members.
            Members destruct order is very important here, because destructing other
            members will cause notify back to this class, and then will access to this
            two members.
        */
        typedef set<HardwareVertexBuffer*>::type VertexBufferList;
        typedef set<HardwareIndexBuffer*>::type IndexBufferList;
        VertexBufferList mVertexBuffers;
        IndexBufferList mIndexBuffers;


        typedef set<VertexDeclaration*>::type VertexDeclarationList;
		typedef set<VertexBufferBinding*>::type VertexBufferBindingList;
        VertexDeclarationList mVertexDeclarations;
		VertexBufferBindingList mVertexBufferBindings;

		// Mutexes
		CM_MUTEX(mVertexBuffersMutex)
		CM_MUTEX(mIndexBuffersMutex)
		CM_MUTEX(mVertexDeclarationsMutex)
		CM_MUTEX(mVertexBufferBindingsMutex)

        /// Internal method for destroys all vertex declarations
        virtual void destroyAllDeclarations(void);
        /// Internal method for destroys all vertex buffer bindings
        virtual void destroyAllBindings(void);

        /// Internal method for creates a new vertex declaration, may be overridden by certain rendering APIs
        virtual VertexDeclaration* createVertexDeclarationImpl(void);
        /// Internal method for destroys a vertex declaration, may be overridden by certain rendering APIs
        virtual void destroyVertexDeclarationImpl(VertexDeclaration* decl);

		/// Internal method for creates a new VertexBufferBinding, may be overridden by certain rendering APIs
		virtual VertexBufferBinding* createVertexBufferBindingImpl(void);
		/// Internal method for destroys a VertexBufferBinding, may be overridden by certain rendering APIs
		virtual void destroyVertexBufferBindingImpl(VertexBufferBinding* binding);

    protected:

        /// Creates  a new buffer as a copy of the source, does not copy data
        virtual HardwareVertexBufferPtr makeBufferCopy(
            const HardwareVertexBufferPtr& source, 
            HardwareBuffer::Usage usage, bool useShadowBuffer);

    public:
        HardwareBufferManagerBase();
        virtual ~HardwareBufferManagerBase();
		/** Create a hardware vertex buffer.
        @remarks
            This method creates a new vertex buffer; this will act as a source of geometry
            data for rendering objects. Note that because the meaning of the contents of
            the vertex buffer depends on the usage, this method does not specify a
            vertex format; the user of this buffer can actually insert whatever data 
            they wish, in any format. However, in order to use this with a RenderOperation,
            the data in this vertex buffer will have to be associated with a semantic element
            of the rendering pipeline, e.g. a position, or texture coordinates. This is done 
            using the VertexDeclaration class, which itself contains VertexElement structures
            referring to the source data.
        @remarks Note that because vertex buffers can be shared, they are reference
            counted so you do not need to worry about destroying themm this will be done
            automatically.
        @param vertexSize The size in bytes of each vertex in this buffer; you must calculate
            this based on the kind of data you expect to populate this buffer with.
        @param numVerts The number of vertices in this buffer.
        @param usage One or more members of the HardwareBuffer::Usage enumeration; you are
            strongly advised to use HBU_STATIC_WRITE_ONLY wherever possible, if you need to 
            update regularly, consider HBU_DYNAMIC_WRITE_ONLY and useShadowBuffer=true.
		@param useShadowBuffer If set to true, this buffer will be 'shadowed' by one stored in 
            system memory rather than GPU or AGP memory. You should set this flag if you intend 
            to read data back from the vertex buffer, because reading data from a buffer
			in the GPU or AGP memory is very expensive, and is in fact impossible if you
            specify HBU_WRITE_ONLY for the main buffer. If you use this option, all 
            reads and writes will be done to the shadow buffer, and the shadow buffer will
            be synchronised with the real buffer at an appropriate time.
        */
		virtual HardwareVertexBufferPtr 
            createVertexBuffer(size_t vertexSize, size_t numVerts, HardwareBuffer::Usage usage, 
			bool useShadowBuffer = false) = 0;
		/** Create a hardware index buffer.
        @remarks Note that because buffers can be shared, they are reference
            counted so you do not need to worry about destroying them this will be done
            automatically.
		@param itype The type in index, either 16- or 32-bit, depending on how many vertices
			you need to be able to address
		@param numIndexes The number of indexes in the buffer
        @param usage One or more members of the HardwareBuffer::Usage enumeration.
		@param useShadowBuffer If set to true, this buffer will be 'shadowed' by one stored in 
            system memory rather than GPU or AGP memory. You should set this flag if you intend 
            to read data back from the index buffer, because reading data from a buffer
			in the GPU or AGP memory is very expensive, and is in fact impossible if you
            specify HBU_WRITE_ONLY for the main buffer. If you use this option, all 
            reads and writes will be done to the shadow buffer, and the shadow buffer will
            be synchronised with the real buffer at an appropriate time.
        */
		virtual HardwareIndexBufferPtr 
            createIndexBuffer(HardwareIndexBuffer::IndexType itype, size_t numIndexes, 
			HardwareBuffer::Usage usage, bool useShadowBuffer = false) = 0;

        /** Creates a new vertex declaration. */
        virtual VertexDeclaration* createVertexDeclaration(void);
        /** Destroys a vertex declaration. */
        virtual void destroyVertexDeclaration(VertexDeclaration* decl);

		/** Creates a new VertexBufferBinding. */
		virtual VertexBufferBinding* createVertexBufferBinding(void);
		/** Destroys a VertexBufferBinding. */
		virtual void destroyVertexBufferBinding(VertexBufferBinding* binding);

        /** Allocates a copy of a given vertex buffer.
        @remarks
            This method allocates a temporary copy of an existing vertex buffer.
            This buffer is subsequently stored and can be made available for 
            other purposes later without incurring the cost of construction / 
            destruction.
        @param sourceBuffer The source buffer to use as a copy
        @param licenseType The type of license required on this buffer - automatic
            release causes this class to release licenses every frame so that 
            they can be reallocated anew.
        @param licensee Pointer back to the class requesting the copy, which must
            implement HardwareBufferLicense in order to be notified when the license
            expires.
        @param copyData If true, the current data is copied as well as the 
            structure of the buffer
        */
        virtual HardwareVertexBufferPtr allocateVertexBufferCopy(
            const HardwareVertexBufferPtr& sourceBuffer,
            bool copyData = false);

		/// Notification that a hardware vertex buffer has been destroyed
		void _notifyVertexBufferDestroyed(HardwareVertexBuffer* buf);
		/// Notification that a hardware index buffer has been destroyed
		void _notifyIndexBufferDestroyed(HardwareIndexBuffer* buf);

	};

    /** Singleton wrapper for hardware buffer manager. */
    class CM_EXPORT HardwareBufferManager : public HardwareBufferManagerBase, public Singleton<HardwareBufferManager>
    {
    protected:
		HardwareBufferManagerBase* mImpl;
	public:
		HardwareBufferManager(HardwareBufferManagerBase* imp);
		~HardwareBufferManager();

		/** @copydoc HardwareBufferManagerInterface::createVertexBuffer */
		HardwareVertexBufferPtr 
            createVertexBuffer(size_t vertexSize, size_t numVerts, HardwareBuffer::Usage usage, 
			bool useShadowBuffer = false)
		{
			return mImpl->createVertexBuffer(vertexSize, numVerts, usage, useShadowBuffer);
		}
		/** @copydoc HardwareBufferManagerInterface::createIndexBuffer */
		HardwareIndexBufferPtr 
            createIndexBuffer(HardwareIndexBuffer::IndexType itype, size_t numIndexes, 
			HardwareBuffer::Usage usage, bool useShadowBuffer = false)
		{
			return mImpl->createIndexBuffer(itype, numIndexes, usage, useShadowBuffer);
		}

		/** @copydoc HardwareBufferManagerInterface::createVertexDeclaration */
		virtual VertexDeclaration* createVertexDeclaration(void)
		{
			return mImpl->createVertexDeclaration();
		}
		/** @copydoc HardwareBufferManagerInterface::destroyVertexDeclaration */
        virtual void destroyVertexDeclaration(VertexDeclaration* decl)
		{
			mImpl->destroyVertexDeclaration(decl);
		}

		/** @copydoc HardwareBufferManagerInterface::createVertexBufferBinding */
		virtual VertexBufferBinding* createVertexBufferBinding(void)
		{
			return mImpl->createVertexBufferBinding();
		}
		/** @copydoc HardwareBufferManagerInterface::destroyVertexBufferBinding */
		virtual void destroyVertexBufferBinding(VertexBufferBinding* binding)
		{
			mImpl->destroyVertexBufferBinding(binding);
		}
		/** @copydoc HardwareBufferManagerInterface::allocateVertexBufferCopy */
        virtual HardwareVertexBufferPtr allocateVertexBufferCopy(
            const HardwareVertexBufferPtr& sourceBuffer,
            bool copyData = false)
		{
			return mImpl->allocateVertexBufferCopy(sourceBuffer, copyData);
		}
		/** @copydoc HardwareBufferManagerInterface::_notifyVertexBufferDestroyed */
		void _notifyVertexBufferDestroyed(HardwareVertexBuffer* buf)
		{
			mImpl->_notifyVertexBufferDestroyed(buf);
		}
		/** @copydoc HardwareBufferManagerInterface::_notifyIndexBufferDestroyed */
		void _notifyIndexBufferDestroyed(HardwareIndexBuffer* buf)
		{
			mImpl->_notifyIndexBufferDestroyed(buf);
		}


        /** Override standard Singleton retrieval.
        @remarks
        Why do we do this? Well, it's because the Singleton
        implementation is in a .h file, which means it gets compiled
        into anybody who includes it. This is needed for the
        Singleton template to work, but we actually only want it
        compiled into the implementation of the class based on the
        Singleton, not all of them. If we don't change this, we get
        link errors when trying to use the Singleton-based class from
        an outside dll.
        @par
        This method just delegates to the template version anyway,
        but the implementation stays in this single compilation unit,
        preventing link errors.
        */
        static HardwareBufferManager& getSingleton(void);
        /** Override standard Singleton retrieval.
        @remarks
        Why do we do this? Well, it's because the Singleton
        implementation is in a .h file, which means it gets compiled
        into anybody who includes it. This is needed for the
        Singleton template to work, but we actually only want it
        compiled into the implementation of the class based on the
        Singleton, not all of them. If we don't change this, we get
        link errors when trying to use the Singleton-based class from
        an outside dll.
        @par
        This method just delegates to the template version anyway,
        but the implementation stays in this single compilation unit,
        preventing link errors.
        */
        static HardwareBufferManager* getSingletonPtr(void);
            
    };

	/** @} */
	/** @} */
}

#endif

