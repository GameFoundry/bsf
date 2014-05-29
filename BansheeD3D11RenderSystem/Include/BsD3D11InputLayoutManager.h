#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsVertexDeclaration.h"

namespace BansheeEngine
{
	class D3D11InputLayoutManager
	{
	public:
		struct VertexDeclarationKey
		{
			UINT64 vertxDeclId;
			UINT32 vertexProgramId;
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
		ID3D11InputLayout* retrieveInputLayout(VertexDeclarationPtr vertexShaderDecl, VertexDeclarationPtr vertexBufferDecl, D3D11GpuProgram& vertexProgram);

	private:
		static const int DECLARATION_BUFFER_SIZE = 1024;
		static const int NUM_ELEMENTS_TO_PRUNE = 64;

		UnorderedMap<VertexDeclarationKey, InputLayoutEntry*, HashFunc, EqualFunc> mInputLayoutMap;

		bool mWarningShown;
		UINT32 mLastUsedCounter;

		void addNewInputLayout(VertexDeclarationPtr vertexShaderDecl, VertexDeclarationPtr vertexBufferDecl, D3D11GpuProgram& vertexProgram);
		void removeLeastUsed();

		bool areCompatible(VertexDeclarationPtr vertexShaderDecl, VertexDeclarationPtr vertexBufferDecl);
	};
}