#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmVertexDeclaration.h"

namespace CamelotEngine
{
	class D3D11VertexDeclaration : public VertexDeclaration
	{
	protected:
		mutable bool mNeedsRebuild;

		map<D3D11Device*, ID3D11InputLayout*>::type mLayoutPerDevice;
	public:
		D3D11VertexDeclaration();
		~D3D11VertexDeclaration();

		/**
		 * @copydoc VertexDeclaration::addElement
		 */
		const VertexElement& addElement(unsigned short source, UINT32 offset, VertexElementType theType,
			VertexElementSemantic semantic, unsigned short index = 0);

		/**
		 * @copydoc VertexDeclaration::insertElement
		 */
		const VertexElement& insertElement(unsigned short atPosition,
			unsigned short source, UINT32 offset, VertexElementType theType,
			VertexElementSemantic semantic, unsigned short index = 0);

		/**
		 * @copydoc VertexDeclaration::removeElement(unsigned short)
		 */
		void removeElement(unsigned short elem_index);

		/**
		 * @copydoc VertexDeclaration::removeElement(VertexElementSemantic, unsigned short)
		 */
		void removeElement(VertexElementSemantic semantic, unsigned short index = 0);

		/**
		 * @copydoc VertexDeclaration::removeAllElements
		 */
		void removeAllElements(void);

		/**
		 * @copydoc VertexDeclaration::modifyElement
		 */
		void modifyElement(unsigned short elem_index, unsigned short source, UINT32 offset, VertexElementType theType,
			VertexElementSemantic semantic, unsigned short index = 0);

		/**
		 * @brief	Gets the D3D11 input layout.
		 * 			
		 * @note	Recreates the layout if it way previously modified or if this is the first time
		 * 			this method is called.
		 */
		ID3D11InputLayout* getD3DLayout(D3D11Device& device, D3D11HLSLProgram& programToBindTo);
	};

}