#pragma once

#include "BsCorePrerequisites.h"
#include "BsColor.h"
#include "BsCoreObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Semantics that are used for identifying the meaning of
	 *			vertex buffer elements.
	 */
	enum VertexElementSemantic 
	{
		VES_POSITION = 1,
		VES_BLEND_WEIGHTS = 2,
        VES_BLEND_INDICES = 3,
		VES_NORMAL = 4,
		VES_COLOR = 5,
		VES_TEXCOORD = 6,
        VES_BITANGENT = 7,
        VES_TANGENT = 8,
		VES_POSITIONT = 9,
		VES_PSIZE = 10
	};

	/**
	 * @brief	Types used to identify base types of vertex element contents
	 */
    enum VertexElementType
    {
        VET_FLOAT1 = 0,
        VET_FLOAT2 = 1,
        VET_FLOAT3 = 2,
        VET_FLOAT4 = 3,
		VET_COLOR = 4,
		VET_SHORT1 = 5,
		VET_SHORT2 = 6,
		VET_SHORT3 = 7,
		VET_SHORT4 = 8,
        VET_UBYTE4 = 9,
        VET_COLOR_ARGB = 10,
        VET_COLOR_ABGR = 11,
		VET_UINT4 = 12,
		VET_SINT4 = 13
    };

	/**
	 * @brief	Describes a single vertex element in a vertex declaration.
	 */
	class BS_CORE_EXPORT VertexElement
    {
    public:
		VertexElement() {}
        VertexElement(UINT16 source, UINT32 offset, VertexElementType theType,
			VertexElementSemantic semantic, UINT16 index = 0);

		bool operator== (const VertexElement& rhs) const;
		bool operator!= (const VertexElement& rhs) const;

		/**
		 * @brief	Returns index of the vertex buffer from which this element is stored.
		 */
		UINT16 getStreamIdx() const { return mSource; }

		/**
		 * @brief	Returns an offset into the buffer where this vertex is stored. This value
		 *			might be in bytes but doesn't have to be, it's likely to be render API specific.
		 */
        UINT32 getOffset() const { return mOffset; }

		/**
		 * @brief	Gets the base data type of tis element.
		 */
        VertexElementType getType() const { return mType; }

		/**
		 * @brief	Gets a semantic that describes what this element contains.
		 */
        VertexElementSemantic getSemantic() const { return mSemantic; }

		/**
		 * @brief	Gets an index of this element. Only relevant when you have
		 *			multiple elements with the same semantic, e.g. uv0, uv1.
		 */
		UINT16 getSemanticIdx() const { return mIndex; }

		/**
		 * @brief	Returns the size of this element in bytes.
		 */
		UINT32 getSize() const;

		/**
		 * @brief	Returns the size of a base element type.
		 */
		static UINT32 getTypeSize(VertexElementType etype);

		/**
		 * @brief	Returns the number of values in the provided base element type.
		 *			e.g. float4 has four values.
		 */
		static UINT16 getTypeCount(VertexElementType etype);

		/**
		 * @brief	Gets packed color vertex element type used by the active render system.
		 */
		static VertexElementType getBestColorVertexElementType();

	protected:
		UINT16 mSource;
		UINT32 mOffset;
		VertexElementType mType;
		VertexElementSemantic mSemantic;
		UINT16 mIndex;
    };

	BS_ALLOW_MEMCPY_SERIALIZATION(VertexElement);

	/**
	 * @brief	Describes a set of vertex elements, used for describing contents of
	 *			a vertex buffer or inputs to a vertex GPU program.
	 */
	class BS_CORE_EXPORT VertexDeclaration : public IReflectable, public CoreObject
    {
    public:
        typedef List<VertexElement> VertexElementList;

	public:
        virtual ~VertexDeclaration();

		bool operator== (const VertexDeclaration& rhs) const;
		bool operator!= (const VertexDeclaration& rhs) const;

		/**
		 * @brief	Get the number of elements in the declaration.
		 */
        UINT32 getElementCount() { return (UINT32)mElementList.size(); }
        
		/**
		 * @brief	Returns a list of vertex elements in the declaration.
		 */
		const VertexElementList& getElements() const { return mElementList; }

		/**
		 * @brief	Returns a single vertex element at the specified index.
		 */
        const VertexElement* getElement(UINT16 index);

		/**
		 * @brief	Attempts to find an element by the given semantic and semantic index. If no element
		 *			can be found null is returned.
		 */
		virtual const VertexElement* findElementBySemantic(VertexElementSemantic sem, UINT16 index = 0);

		/**
		 * @brief	Returns a list of all elements that use the provided source index.
		 */
		virtual VertexElementList findElementsBySource(UINT16 source);

		/**
		 * @brief	Returns the total size of all vertex elements using the provided source index.
		 */
		virtual UINT32 getVertexSize(UINT16 source);

    protected:
		friend class HardwareBufferManager;

		VertexDeclaration(const VertexElementList& elements);

	protected:
		VertexElementList mElementList;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class VertexDeclarationRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };
}