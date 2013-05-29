#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmVertexDeclaration.h"

namespace CamelotFramework
{
	class D3D11InputLayoutManager
	{
	public:
		struct VertexDeclarationKey
		{
			size_t bufferDeclHash;
			UINT32 vertexProgramId;

			list<VertexElement>::type* bufferDeclElements;
		};

		class HashFunc 
		{
		public:
			::std::size_t operator()(const VertexDeclarationKey &key) const;
		};

		class EqualFunc
		{
		public:
			bool operator()(const VertexDeclarationKey &a, const VertexDeclarationKey &b) const;
		};

		struct InputLayoutEntry
		{
			InputLayoutEntry() {}

			ID3D11InputLayout* inputLayout;
			UINT32 lastUsedIdx;
		};

	public:
		D3D11InputLayoutManager();
		~D3D11InputLayoutManager();

		/**
		 * @brief	Finds an existing one or creates a new D3D11 input layout.
		 * 			
		 *			The created declaration will try to match the input to the provided shader with the
		 *			data provided in the vertex buffer (described by the vertex buffer declaration).
		 *			
		 *			Error may be thrown if the vertex buffer doesn't provide all the necessary data.
		 *			(TODO: Depends on how the driver handles missing data)
		 */
		ID3D11InputLayout* retrieveInputLayout(VertexDeclarationPtr vertexShaderDecl, VertexDeclarationPtr vertexBufferDecl, D3D11HLSLProgram& vertexProgram);

	private:
		static const int DECLARATION_BUFFER_SIZE = 1024;
		static const int NUM_ELEMENTS_TO_PRUNE = 64;

		unordered_map<VertexDeclarationKey, InputLayoutEntry*, HashFunc, EqualFunc>::type mInputLayoutMap;

		bool mWarningShown;
		UINT32 mLastUsedCounter;

		void addNewInputLayout(VertexDeclarationPtr vertexShaderDecl, VertexDeclarationPtr vertexBufferDecl, D3D11HLSLProgram& vertexProgram);
		void removeLeastUsed();

		bool areCompatible(VertexDeclarationPtr vertexShaderDecl, VertexDeclarationPtr vertexBufferDecl);
	};
}