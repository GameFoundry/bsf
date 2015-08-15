#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsVertexDeclaration.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation and caching of DirectX 11 input layout objects.
	 */
	class D3D11InputLayoutManager
	{
	public:
		/**
		 * @brief	Key uniquely identifying vertex declaration and vertex shader combination.
		 */
		struct VertexDeclarationKey
		{
			UINT64 vertxDeclId;
			UINT32 vertexProgramId;
		};

		/**
		 * @brief	Creates a hash from vertex declaration key.
		 */
		class HashFunc 
		{
		public:
			::std::size_t operator()(const VertexDeclarationKey &key) const;
		};

		/**
		 * @brief	Compares two vertex declaration keys.
		 */
		class EqualFunc
		{
		public:
			bool operator()(const VertexDeclarationKey &a, const VertexDeclarationKey &b) const;
		};

		/**
		 * @brief	Contains data about a single instance of DX11 input layout object.
		 */
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
		 * Finds an existing or creates a new D3D11 input layout. Input layout maps a vertex declaration
		 * from a vertex buffer to vertex program input declaration
		 *
		 * @param	vertexShaderDecl	Vertex declaration describing vertex program input parameters.
		 * @param	vertexBufferDecl	Vertex declaration describing structure of a vertex buffer to be bound as input
		 *								to the GPU program.
		 * @param	vertexProgram		Instance of the vertex program we are creating input layout for.
		 *
		 * @note	Error will be thrown if the vertex buffer doesn't provide all the necessary data that the shader expects.
		 */
		ID3D11InputLayout* retrieveInputLayout(const SPtr<VertexDeclarationCore>& vertexShaderDecl,
			const SPtr<VertexDeclarationCore>& vertexBufferDecl, D3D11GpuProgramCore& vertexProgram);

	private:
		/**
		 * @brief	Creates a new input layout using the specified parameters and stores it in the input layout map.
		 */
		void addNewInputLayout(const SPtr<VertexDeclarationCore>& vertexShaderDecl, const SPtr<VertexDeclarationCore>& vertexBufferDecl,
			D3D11GpuProgramCore& vertexProgram);

		/**
		 * @brief	Destroys least used input layout.
		 */
		void removeLeastUsed();

		/**
		 * @brief	Checks are the specified vertex shader input and vertex buffer declarations compatible.
		 *			Vertex buffer must provide all inputs to the shader in order for them to be compatible
		 *			(extra data is allowed).
		 */
		bool areCompatible(const SPtr<VertexDeclarationCore>& vertexShaderDecl, const SPtr<VertexDeclarationCore>& vertexBufferDecl);

	private:
		static const int DECLARATION_BUFFER_SIZE = 1024;
		static const int NUM_ELEMENTS_TO_PRUNE = 64;

		UnorderedMap<VertexDeclarationKey, InputLayoutEntry*, HashFunc, EqualFunc> mInputLayoutMap;

		bool mWarningShown;
		UINT32 mLastUsedCounter;
	};
}