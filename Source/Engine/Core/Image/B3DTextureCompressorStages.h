//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "B3DPrerequisites.h"
#include "Image/B3DPixelUtility.h"
#include "Math/B3DVector2.h"
#include "Math/B3DVector4I.h"
#include "GpuBackend/B3DGpuBuffer.h"
#include "GpuBackend/B3DGpuParameter.h"

namespace b3d::render
{
	class RendererMaterialBase;
	class FrameGraphPass;
	class FrameGraphPassContext;

	/** @addtogroup Image-Internal
	 *  @{
	 */

	/** Per-chunk scratch buffer through which the stages of a texture compression pass data to later stages. */
	enum class TextureCompressScratchBuffer : u32
	{
		/**
		 * Partitions the BC7 two-subset finalize stages fully encode, best first, one u32 each. Per block, the list of
		 * mode 1 is followed by the lists of modes 3 and 7.
		 */
		BC7TwoSubsetCandidates,

		/**
		 * Partitions the BC7 three-subset finalize stages fully encode, best first, one u32 each. Per block, the list of
		 * mode 0 is followed by the list of mode 2.
		 */
		BC7ThreeSubsetCandidates,

		/** Partitions the BC6H two-region stages fully encode, best first, one u32 each. Shared by all ten two-region modes. */
		BC6HCandidates,

		Count
	};

	/** Constant table uploaded once per texture compression and read by its stages. */
	enum class TextureCompressTable : u32
	{
		/** Two-subset BC7 partitions, one u32 each: subset 1 texel mask | (subset 1 anchor texel << 16). */
		BC7TwoSubsetPartitions,

		/**
		 * Three-subset BC7 partitions, two u32 each: subset 1 texel mask | (subset 2 texel mask << 16), then subset 1
		 * anchor texel | (subset 2 anchor texel << 8).
		 */
		BC7ThreeSubsetPartitions,

		Count
	};

	/** Resources a stage uses besides the input and output textures. */
	struct TextureCompressStageRequirements
	{
		/** Number of elements per block of every scratch buffer the stage reads or writes. 0 if it doesn't use the buffer. */
		u32 ScratchElementsPerBlock[(u32)TextureCompressScratchBuffer::Count] = {};

		/** Tables the stage reads. */
		bool UsesTable[(u32)TextureCompressTable::Count] = {};

		/** True if the stage compares its blocks against the running-best error of every block, and updates it. */
		bool UsesBestError = false;

		/** Stage-specific constants, set as `gStageParameters` in the `Parameters` uniform buffer. */
		Vector4I StageParameters = Vector4I(0, 0, 0, 0);
	};

	/** Resources bound by one dispatch of a stage. */
	struct TextureCompressBindings
	{
		TShared<Texture> Input;
		TShared<Texture> Output;

		/** Running-best error of every block. Null unless a stage of the compression uses it. */
		TShared<Texture> BestError;

		/** Scratch buffers of the chunk. Null for buffers that no stage of the compression uses. */
		TShared<GpuBuffer> ScratchBuffers[(u32)TextureCompressScratchBuffer::Count];

		/** Tables of the compression. Null for tables that no stage of the compression uses. */
		TShared<GpuBuffer> Tables[(u32)TextureCompressTable::Count];

		/** Contents of the `Parameters` uniform buffer, for the stage's parameters. */
		const GpuBufferMappedScope* Parameters = nullptr;

		/** Number of blocks in the chunk along each axis. */
		Vector2I ChunkBlockCount;
	};

	/**
	 * One compute kernel of a texture compression. A format is compressed by running its stages in order on every chunk of
	 * blocks. Each stage dispatches groups that cover a fixed rectangle of blocks.
	 */
	class TextureCompressStage
	{
	public:
		using GetMaterialFunction = std::function<RendererMaterialBase*()>;

		/**
		 * @param	name				Name the stage is logged with.
		 * @param	fnGetMaterial		Returns the material (with its shader variation) the stage dispatches.
		 * @param	blocksPerGroup		Number of blocks along each axis one thread group of the kernel compresses.
		 * @param	requirements		Resources the stage uses besides the input and output textures.
		 */
		TextureCompressStage(String name, GetMaterialFunction fnGetMaterial, const Vector2I& blocksPerGroup, const TextureCompressStageRequirements& requirements);

		/**
		 * Fetches the stage's material, compiling its shader variation if needed, and looks up its parameters. Returns false
		 * if the shader is not available. Must be called on the render thread, before PrepareDispatch().
		 */
		bool Prepare();

		/**
		 * Creates the parameter set of @p pass, which dispatches the stage over one chunk of blocks with @p bindings. Returns
		 * the set to pass to Dispatch().
		 */
		TShared<GpuParameterSet> PrepareDispatch(FrameGraphPass& pass, const TextureCompressBindings& bindings) const;

		/** Binds @p set, from PrepareDispatch(), and records the dispatch of the stage over a chunk of @p chunkBlockCount blocks. */
		void Dispatch(FrameGraphPassContext& context, const TShared<GpuParameterSet>& set, const Vector2I& chunkBlockCount) const;

		/** Name the stage is logged with. */
		const String& GetName() const { return mName; }

		/** Resources the stage uses besides the input and output textures. */
		const TextureCompressStageRequirements& GetRequirements() const { return mRequirements; }

	private:
		String mName;
		GetMaterialFunction mFnGetMaterial;
		Vector2I mBlocksPerGroup;
		TextureCompressStageRequirements mRequirements;
		RendererMaterialBase* mMaterial = nullptr;

		GpuParameterSampledTexture mInputParameter;
		GpuParameterStorageTexture mOutputParameter;
		GpuParameterStorageTexture mBestErrorParameter;
		GpuParameterStorageBuffer mScratchBufferParameters[(u32)TextureCompressScratchBuffer::Count];
		GpuParameterStorageBuffer mTableParameters[(u32)TextureCompressTable::Count];
		GpuParameterUniformBuffer mParametersBuffer;
	};

	/** Stages a format is compressed with. */
	using TextureCompressStageList = TInlineArray<TShared<TextureCompressStage>, 32>;

	/**
	 * Describes how a block-compressed format is compressed on the GPU.
	 *
	 * @param	format						Block-compressed format.
	 * @param	outBlockTextureFormat		Format of the texture the stages write the packed blocks into, one block per texel:
	 *										RG32U for 64-bit blocks and RGBA32U for 128-bit blocks.
	 * @param	outIsHDR					True if the stages read the source as RGBA32F rather than RGBA8.
	 * @return								False if the format can't be compressed on the GPU.
	 */
	bool GetTextureCompressFormatInformation(PixelFormat format, PixelFormat& outBlockTextureFormat, bool& outIsHDR);

	/**
	 * Returns the stages @p format is compressed with, in the order they run. Empty if the format isn't supported. Higher
	 * @p quality makes the partitioned BC6H and BC7 modes fully encode more candidate partitions per block.
	 */
	TextureCompressStageList GetTextureCompressStages(PixelFormat format, CompressionQuality quality);

	/** Creates a scratch buffer with @p elementCount elements. Every scratch buffer holds u32 elements. */
	TShared<GpuBuffer> CreateTextureCompressScratchBuffer(GpuDevice& device, u32 elementCount);

	/** Returns the contents of a table. */
	TArrayView<const u32> GetTextureCompressTableData(TextureCompressTable table);

	/** @} */
} // namespace b3d::render
