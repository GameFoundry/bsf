#pragma once

#include "CmPrerequisites.h"
#include "CmColor.h"
#include "CmCoreGpuObject.h"

namespace CamelotEngine
{
	/// Vertex element semantics, used to identify the meaning of vertex buffer contents
	enum VertexElementSemantic {
		/// Position, 3 reals per vertex
		VES_POSITION = 1,
		/// Blending weights
		VES_BLEND_WEIGHTS = 2,
        /// Blending indices
        VES_BLEND_INDICES = 3,
		/// Normal, 3 reals per vertex
		VES_NORMAL = 4,
		/// Diffuse colours
		VES_COLOR = 5,
		/// Specular colours
		VES_SPECULAR = 6,
		/// Texture coordinates
		VES_TEXCOORD = 7,
        /// Binormal (Y axis if normal is Z)
        VES_BITANGENT = 8,
        /// Tangent (X axis if normal is Z)
        VES_TANGENT = 9,
		/// Transformed vertex position
		VES_POSITIONT = 10,
		/// Point size
		VES_PSIZE = 11
	};

    /// Vertex element type, used to identify the base types of the vertex contents
    enum VertexElementType
    {
        VET_FLOAT1 = 0,
        VET_FLOAT2 = 1,
        VET_FLOAT3 = 2,
        VET_FLOAT4 = 3,
        /// alias to more specific color type - use the current rendersystem's color packing
		VET_COLOR = 4,
		VET_SHORT1 = 5,
		VET_SHORT2 = 6,
		VET_SHORT3 = 7,
		VET_SHORT4 = 8,
        VET_UBYTE4 = 9,
        /// D3D style compact color
        VET_COLOR_ARGB = 10,
        /// GL style compact color
        VET_COLOR_ABGR = 11,
		VET_UINT4 = 12,
		VET_SINT4 = 13
    };

    /** This class declares the usage of a single vertex buffer as a component
        of a complete VertexDeclaration.
        @remarks
        Several vertex buffers can be used to supply the input geometry for a
        rendering operation, and in each case a vertex buffer can be used in
        different ways for different operations; the buffer itself does not
        define the semantics (position, normal etc), the VertexElement
        class does.
    */
	class CM_EXPORT VertexElement
    {
    protected:
        /// The source vertex buffer
        unsigned short mSource;
        /// The offset in the buffer that this element starts at
        UINT32 mOffset;
        /// The type of element
        VertexElementType mType;
        /// The meaning of the element
        VertexElementSemantic mSemantic;
        /// Index of the item, only applicable for some elements like texture coords
        unsigned short mIndex;
    public:
		/// Constructor, should not be called directly, only needed because of list
		VertexElement() {}
        /// Constructor, should not be called directly, call VertexDeclaration::addElement
        VertexElement(unsigned short source, UINT32 offset, VertexElementType theType,
            VertexElementSemantic semantic, unsigned short index = 0);
        /// Gets the vertex buffer index from where this element draws it's values
        unsigned short getSource(void) const { return mSource; }
        /// Gets the offset into the buffer where this element starts
        UINT32 getOffset(void) const { return mOffset; }
        /// Gets the data format of this element
        VertexElementType getType(void) const { return mType; }
        /// Gets the meaning of this element
        VertexElementSemantic getSemantic(void) const { return mSemantic; }
        /// Gets the index of this element, only applicable for repeating elements
        unsigned short getIndex(void) const { return mIndex; }
		/// Gets the size of this element in bytes
		UINT32 getSize(void) const;
		/// Utility method for helping to calculate offsets
		static UINT32 getTypeSize(VertexElementType etype);
		/// Utility method which returns the count of values in a given type
		static unsigned short getTypeCount(VertexElementType etype);
		/** Simple converter function which will turn a single-value type into a
			multi-value type based on a parameter.
		*/
		static VertexElementType multiplyTypeCount(VertexElementType baseType, unsigned short count);
		/** Simple converter function which will a type into it's single-value
			equivalent - makes switches on type easier.
		*/
		static VertexElementType getBaseType(VertexElementType multiType);

		/** Utility method for converting colour from
			one packed 32-bit colour type to another.
		@param srcType The source type
		@param dstType The destination type
		@param ptr Read / write value to change
		*/
		static void convertColourValue(VertexElementType srcType,
			VertexElementType dstType, UINT32* ptr);

		/** Utility method for converting colour to
			a packed 32-bit colour type.
		@param src source colour
		@param dst The destination type
		*/
		static UINT32 convertColourValue(const Color& src,
			VertexElementType dst);

		/** Utility method to get the most appropriate packed colour vertex element format. */
		static VertexElementType getBestColourVertexElementType(void);

        inline bool operator== (const VertexElement& rhs) const
        {
            if (mType != rhs.mType ||
                mIndex != rhs.mIndex ||
                mOffset != rhs.mOffset ||
                mSemantic != rhs.mSemantic ||
                mSource != rhs.mSource)
                return false;
            else
                return true;
        }

		inline bool operator!= (const VertexElement& rhs) const
		{
			return !(*this == rhs);
		}

        /** Adjusts a pointer to the base of a vertex to point at this element.
        @remarks
            This variant is for void pointers, passed as a parameter because we can't
            rely on covariant return types.
        @param pBase Pointer to the start of a vertex in this buffer.
        @param pElem Pointer to a pointer which will be set to the start of this element.
        */
        inline void baseVertexPointerToElement(void* pBase, void** pElem) const
        {
            // The only way we can do this is to cast to char* in order to use byte offset
            // then cast back to void*.
            *pElem = static_cast<void*>(
            	static_cast<unsigned char*>(pBase) + mOffset);
        }
        /** Adjusts a pointer to the base of a vertex to point at this element.
        @remarks
            This variant is for float pointers, passed as a parameter because we can't
            rely on covariant return types.
        @param pBase Pointer to the start of a vertex in this buffer.
        @param pElem Pointer to a pointer which will be set to the start of this element.
        */
        inline void baseVertexPointerToElement(void* pBase, float** pElem) const
        {
            // The only way we can do this is to cast to char* in order to use byte offset
            // then cast back to float*. However we have to go via void* because casting
            // directly is not allowed
            *pElem = static_cast<float*>(
                static_cast<void*>(
                    static_cast<unsigned char*>(pBase) + mOffset));
        }

        /** Adjusts a pointer to the base of a vertex to point at this element.
        @remarks
            This variant is for RGBA pointers, passed as a parameter because we can't
            rely on covariant return types.
        @param pBase Pointer to the start of a vertex in this buffer.
        @param pElem Pointer to a pointer which will be set to the start of this element.
        */
        inline void baseVertexPointerToElement(void* pBase, RGBA** pElem) const
        {
            *pElem = static_cast<RGBA*>(
                static_cast<void*>(
                    static_cast<unsigned char*>(pBase) + mOffset));
        }
        /** Adjusts a pointer to the base of a vertex to point at this element.
        @remarks
            This variant is for char pointers, passed as a parameter because we can't
            rely on covariant return types.
        @param pBase Pointer to the start of a vertex in this buffer.
        @param pElem Pointer to a pointer which will be set to the start of this element.
        */
        inline void baseVertexPointerToElement(void* pBase, unsigned char** pElem) const
        {
            *pElem = static_cast<unsigned char*>(pBase) + mOffset;
        }

        /** Adjusts a pointer to the base of a vertex to point at this element.
        @remarks
        This variant is for UINT16 pointers, passed as a parameter because we can't
        rely on covariant return types.
        @param pBase Pointer to the start of a vertex in this buffer.
        @param pElem Pointer to a pointer which will be set to the start of this element.
        */
        inline void baseVertexPointerToElement(void* pBase, unsigned short** pElem) const
        {
			*pElem = static_cast<unsigned short*>(
				static_cast<void*>(
					static_cast<unsigned char*>(pBase) + mOffset));
        }

		/**
		 * @brief	Calculates a hash value for this element.
		 */
		size_t calculateHash() const;

    };
    /** This class declares the format of a set of vertex inputs, which
        can be issued to the rendering API through a RenderOperation.
	@remarks
	You should be aware that the ordering and structure of the
	VertexDeclaration can be very important on DirectX with older
	cards,so if you want to maintain maximum compatibility with
	all render systems and all cards you should be careful to follow these
	rules:<ol>
	<li>VertexElements should be added in the following order, and the order of the
	elements within a shared buffer should be as follows:
	position, blending weights, normals, diffuse colours, specular colours,
            texture coordinates (in order, with no gaps)</li>
	<li>You must not have unused gaps in your buffers which are not referenced
	by any VertexElement</li>
	<li>You must not cause the buffer & offset settings of 2 VertexElements to overlap</li>
	</ol>
	Whilst GL and more modern graphics cards in D3D will allow you to defy these rules,
	sticking to them will ensure that your buffers have the maximum compatibility.
	@par
	Like the other classes in this functional area, these declarations should be created and
	destroyed using the HardwareBufferManager.
    */
	class CM_EXPORT VertexDeclaration : public IReflectable, public CoreGpuObject
    {
    public:
		/// Defines the list of vertex elements that makes up this declaration
        typedef list<VertexElement>::type VertexElementList;
        /// Sort routine for vertex elements
        static bool vertexElementLess(const VertexElement& e1, const VertexElement& e2);

        virtual ~VertexDeclaration();

        /** Get the number of elements in the declaration. */
        UINT32 getElementCount(void) { return (UINT32)mElementList.size(); }
        /** Gets read-only access to the list of vertex elements. */
        const VertexElementList& getElements(void) const;
        /** Get a single element. */
        const VertexElement* getElement(unsigned short index);

		/**
		 * @brief	Returns a hash value generated by all of the elements in the declaration.
		 */
		size_t getHash() { return mHash; }

        /** Sorts the elements in this list to be compatible with the maximum
            number of rendering APIs / graphics cards.
        @remarks
            Older graphics cards require vertex data to be presented in a more
            rigid way, as defined in the main documentation for this class. As well
            as the ordering being important, where shared source buffers are used, the
            declaration must list all the elements for each source in turn.
        */
        void sort(void);

        /** Remove any gaps in the source buffer list used by this declaration.
        @remarks
            This is useful if you've modified a declaration and want to remove
            any gaps in the list of buffers being used. Note, however, that if this
            declaration is already being used with a VertexBufferBinding, you will
            need to alter that too. This method is mainly useful when reorganising
            buffers based on an altered declaration.
        @note
            This will cause the vertex declaration to be re-sorted.
        */
        void closeGapsInSource(void);

        /** Gets the index of the highest source value referenced by this declaration. */
        unsigned short getMaxSource(void) const;



        /** Adds a new VertexElement to this declaration.
        @remarks
            This method adds a single element (positions, normals etc) to the end of the
            vertex declaration. <b>Please read the information in VertexDeclaration about
	    the importance of ordering and structure for compatibility with older D3D drivers</b>.
	    @param source The binding index of HardwareVertexBuffer which will provide the source for this element.
			See VertexBufferBindingState for full information.
        @param offset The offset in bytes where this element is located in the buffer
        @param theType The data format of the element (3 floats, a colour etc)
        @param semantic The meaning of the data (position, normal, diffuse colour etc)
        @param index Optional index for multi-input elements like texture coordinates
		@returns A reference to the VertexElement added.
        */
        virtual const VertexElement& addElement(unsigned short source, UINT32 offset, VertexElementType theType,
            VertexElementSemantic semantic, unsigned short index = 0);
        /** Inserts a new VertexElement at a given position in this declaration.
        @remarks
        This method adds a single element (positions, normals etc) at a given position in this
        vertex declaration. <b>Please read the information in VertexDeclaration about
        the importance of ordering and structure for compatibility with older D3D drivers</b>.
        @param source The binding index of HardwareVertexBuffer which will provide the source for this element.
        See VertexBufferBindingState for full information.
        @param offset The offset in bytes where this element is located in the buffer
        @param theType The data format of the element (3 floats, a colour etc)
        @param semantic The meaning of the data (position, normal, diffuse colour etc)
        @param index Optional index for multi-input elements like texture coordinates
        @returns A reference to the VertexElement added.
        */
        virtual const VertexElement& insertElement(unsigned short atPosition,
            unsigned short source, UINT32 offset, VertexElementType theType,
            VertexElementSemantic semantic, unsigned short index = 0);

        /** Remove the element at the given index from this declaration. */
        virtual void removeElement(unsigned short elem_index);

        /** Remove the element with the given semantic and usage index.
        @remarks
            In this case 'index' means the usage index for repeating elements such
            as texture coordinates. For other elements this will always be 0 and does
            not refer to the index in the vector.
        */
        virtual void removeElement(VertexElementSemantic semantic, unsigned short index = 0);

		/** Remove all elements. */
		virtual void removeAllElements(void);

        /** Modify an element in-place, params as addElement.
	   @remarks
	   <b>Please read the information in VertexDeclaration about
	    the importance of ordering and structure for compatibility with older D3D drivers</b>.
	 */
        virtual void modifyElement(unsigned short elem_index, unsigned short source, UINT32 offset, VertexElementType theType,
            VertexElementSemantic semantic, unsigned short index = 0);

		/** Finds a VertexElement with the given semantic, and index if there is more than
			one element with the same semantic.
        @remarks
            If the element is not found, this method returns null.
		*/
		virtual const VertexElement* findElementBySemantic(VertexElementSemantic sem, unsigned short index = 0);
		/** Based on the current elements, gets the size of the vertex for a given buffer source.
		@param source The buffer binding index for which to get the vertex size.
		*/

		/** Gets a list of elements which use a given source.
		@remarks
			Note that the list of elements is returned by value therefore is separate from
			the declaration as soon as this method returns.
		*/
		virtual VertexElementList findElementsBySource(unsigned short source);

		/** Gets the vertex size defined by this declaration for a given source. */
        virtual UINT32 getVertexSize(unsigned short source);

        /** Clones this declaration. 
		@param mgr Optional HardwareBufferManager to use for creating the clone
			(if null, use the current default).
		*/
        virtual VertexDeclarationPtr clone(HardwareBufferManager* mgr = 0);

        inline bool operator== (const VertexDeclaration& rhs) const
        {
            if (mElementList.size() != rhs.mElementList.size())
                return false;

            VertexElementList::const_iterator i, iend, rhsi, rhsiend;
            iend = mElementList.end();
            rhsiend = rhs.mElementList.end();
            rhsi = rhs.mElementList.begin();
            for (i = mElementList.begin(); i != iend && rhsi != rhsiend; ++i, ++rhsi)
            {
                if ( !(*i == *rhsi) )
                    return false;
            }

            return true;
        }
        inline bool operator!= (const VertexDeclaration& rhs) const
        {
            return !(*this == rhs);
        }

    protected:
		friend class HardwareBufferManager;

		VertexDeclaration();

		/**
		 * @brief	Generates a hash value based on all elements in the declaration.
		 */
		void recalculateHash();

	protected:
		VertexElementList mElementList;
		size_t mHash;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class VertexDeclarationRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };
}