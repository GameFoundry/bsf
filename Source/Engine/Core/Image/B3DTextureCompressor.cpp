//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Image/B3DTextureCompressor.h"
#include "Image/B3DTextureCompressorStages.h"

#include "Image/B3DPixelUtility.h"
#include "Image/B3DPixelData.h"
#include "Image/B3DTexture.h"
#include "Renderer/B3DRenderer.h"
#include "Renderer/B3DGpuUniformBuffer.h"
#include "GpuBackend/B3DGpuDevice.h"
#include "GpuBackend/B3DGpuCommandBuffer.h"
#include "GpuBackend/B3DGpuBuffer.h"
#include "GpuBackend/B3DGpuQueries.h"
#include "CoreObject/B3DRenderThread.h"
#include "Threading/B3DAsyncOp.h"
#include "Utility/B3DConfigVariable.h"
#include "B3DApplication.h"

#include <cmath>
#include <cstring>
#include <cfloat>

using namespace b3d;

namespace b3d::render
{
	namespace
	{
		/** Per-dispatch constants of every texture compression stage (maps to the BSL `cbuffer Parameters`). */
		B3D_UNIFORM_BUFFER_BEGIN(TextureCompressParameterDefinition)
			B3D_UNIFORM_BUFFER_MEMBER(Vector2I, gTextureSize)  // Texture size in pixels
			B3D_UNIFORM_BUFFER_MEMBER(Vector2I, gBlockCount)   // Number of 4x4 blocks along each axis (whole texture)
			B3D_UNIFORM_BUFFER_MEMBER(Vector2I, gBlockOffset)  // Block-grid offset of the dispatched chunk
			B3D_UNIFORM_BUFFER_MEMBER(Vector2I, gScratchBlockCount) // Number of blocks in the dispatched chunk
			B3D_UNIFORM_BUFFER_MEMBER(Vector4I, gStageParameters) // TextureCompressStageRequirements::StageParameters of the dispatched stage
		B3D_UNIFORM_BUFFER_END

		TextureCompressParameterDefinition gTextureCompressParameters;
	}
}

namespace b3d
{
	namespace
	{
#if B3D_BUILD_TYPE_DEVELOPMENT
		TConfigVariable gGpuLogTextureCompressionTimings("gpu.LogTextureCompressionTimings",
			"Log the GPU time of every GPU texture compression stage, summed over the texture. Development builds only.",
			false);
#endif

		/** GPU time a single chunk submission is sized to take. Keeps every submission far below the OS GPU watchdog limit. */
		constexpr float kChunkTimeBudgetMilliseconds = 50.0f;

		/** Number of blocks in the first chunks, compressed before any GPU time has been measured. */
		constexpr u32 kInitialChunkBlockCount = 64;

		/** Number of blocks in every chunk when the GPU time of chunks cannot be measured. */
		constexpr u32 kUnmeasuredChunkBlockCount = 64 * 64;

		/** Maximum factor by which a chunk may be larger than the last chunk whose GPU time was measured. */
		constexpr float kMaximumChunkGrowth = 4.0f;

		/** Number of chunk submissions in flight at once. Each has its own set of scratch resources. */
		constexpr u32 kChunksInFlight = 2;

		/** Maximum number of distinct stage parameters among the stages of a compression, each with its own uniform buffer. */
		constexpr u32 kMaximumParameterVariants = 8;

		/**
		 * Records the GPU compression on the render thread and sets up an asynchronous read-back. Must be called on the
		 * render thread: it creates the shared GPU resources, dispatches the stages chunk by chunk, then queues a
		 * non-blocking read-back of the packed blocks into @p destination. Completes @p op with @p destination once the GPU
		 * finishes and the blocks have been copied back, or with null on failure.
		 */
		void CompressOnRenderThread(const render::TextureCompressStageList& stages, const TShared<PixelData>& source, PixelFormat inputTexFormat, PixelFormat outputTexFormat, const TShared<PixelData>& destination, u32 maxChunkSize, TAsyncOp<TShared<PixelData>> op)
		{
			AssertIfNotRenderThread();

			const TShared<GpuDevice> gpuDevice = GetApplication().GetPrimaryGpuDevice();
			if(gpuDevice == nullptr)
			{
				op.CompleteOperation(nullptr);
				return;
			}

			GpuWorkContext& gpuContext = render::GetRenderer()->GetGpuContext();

			// Compile/fetch every shader variation before allocating or submitting any GPU work
			for(const TShared<render::TextureCompressStage>& stage : stages)
			{
				if(!stage->Prepare())
				{
					op.CompleteOperation(nullptr);
					return;
				}
			}

			const u32 width = source->GetWidth();
			const u32 height = source->GetHeight();
			const u32 blockCountX = Math::DivideAndRoundUp(width, 4u);
			const u32 blockCountY = Math::DivideAndRoundUp(height, 4u);
			const u32 blockCount = blockCountX * blockCountY;
			if(blockCount == 0 || height == 0)
			{
				op.CompleteOperation(nullptr);
				return;
			}

			// Command buffer pool for the whole operation (uploads, chunk dispatches, read-back)
			const TShared<render::GpuCommandBufferPool> pool = gpuDevice->CreateGpuCommandBufferPool(render::GpuCommandBufferPoolCreateInformation::CreateForThisThread());

			// Upload the whole source once into one sampled input texture, read as float4 in the shader. LDR formats use a
			// normalized RGBA8 texture; BC6H uses a full-float RGBA32F texture so HDR values survive. A 2D texture (rather
			// than a flat buffer) lets the texture cache exploit the spatial locality of each 4x4 block's 16 texels.
			TextureCreateInformation inputTextureCreateInformation;
			inputTextureCreateInformation.Type = TEX_TYPE_2D;
			inputTextureCreateInformation.Format = inputTexFormat;
			inputTextureCreateInformation.Width = width;
			inputTextureCreateInformation.Height = height;
			inputTextureCreateInformation.Usage = TextureUsageFlag::StoreOnGPU; // device-local, sampled-only

			const TShared<render::Texture> inputTexture = gpuDevice->CreateTexture(inputTextureCreateInformation);
			if(inputTexture == nullptr)
			{
				op.CompleteOperation(nullptr);
				return;
			}

			// Output texture receives all packed blocks, one block per texel (RG32U for 64-bit blocks, RGBA32U for 128-bit),
			// at the block-grid resolution. Device-local + UAV so the compute kernel can write it; read back to the CPU after.
			TextureCreateInformation outputTextureCreateInformation;
			outputTextureCreateInformation.Type = TEX_TYPE_2D;
			outputTextureCreateInformation.Format = outputTexFormat;
			outputTextureCreateInformation.Width = blockCountX;
			outputTextureCreateInformation.Height = blockCountY;
			outputTextureCreateInformation.Usage = TextureUsageFlag::StoreOnGPU | TextureUsageFlag::AllowUnorderedAccessOnTheGPU;

			const TShared<render::Texture> outputTexture = gpuDevice->CreateTexture(outputTextureCreateInformation);
			if(outputTexture == nullptr)
			{
				op.CompleteOperation(nullptr);
				return;
			}

			const Vector2I fullBlockCount((i32)blockCountX, (i32)blockCountY);
			const Vector2I fullTextureSize((i32)width, (i32)height);

			// The texture is compressed in chunks: rectangles of blocks whose stages are all recorded into a single submission.
			// Some GPUs can only preempt compute work between submissions, so a submission that runs too long trips the OS GPU
			// watchdog (TDR). Each chunk is therefore sized from the measured GPU time of earlier chunks, to take about
			// kChunkTimeBudgetMilliseconds. Chunks are at most maxChunkSize pixels wide and tall, and start on 8-block
			// boundaries, so the 8x8-block thread groups of different chunks never overlap.
			const u32 maximumChunkBlockCount = Math::Max(8u, (maxChunkSize / 4u) / 8u * 8u);
			const u32 scratchBlockCountX = Math::Min(maximumChunkBlockCount, blockCountX);
			const u32 scratchBlockCountY = Math::Min(maximumChunkBlockCount, blockCountY);

			// Union of the resources the stages use
			render::TextureCompressStageRequirements combinedRequirements;
			TInlineArray<Vector4I, kMaximumParameterVariants> stageParameters;
			for(const TShared<render::TextureCompressStage>& stage : stages)
			{
				const render::TextureCompressStageRequirements& requirements = stage->GetRequirements();
				for(u32 bufferIndex = 0; bufferIndex < (u32)render::TextureCompressScratchBuffer::Count; ++bufferIndex)
					combinedRequirements.ScratchElementsPerBlock[bufferIndex] = Math::Max(combinedRequirements.ScratchElementsPerBlock[bufferIndex], requirements.ScratchElementsPerBlock[bufferIndex]);

				for(u32 tableIndex = 0; tableIndex < (u32)render::TextureCompressTable::Count; ++tableIndex)
					combinedRequirements.UsesTable[tableIndex] |= requirements.UsesTable[tableIndex];

				combinedRequirements.UsesBestError |= requirements.UsesBestError;

				if(std::find(stageParameters.begin(), stageParameters.end(), requirements.StageParameters) == stageParameters.end())
				{
					B3D_ASSERT(stageParameters.Size() < kMaximumParameterVariants);
					stageParameters.Add(requirements.StageParameters);
				}
			}

			// Running-best error of every block, for formats that try multiple encodings per block
			TShared<render::Texture> bestErrorTexture;
			TShared<PixelData> bestErrorTextureInitialData;
			if(combinedRequirements.UsesBestError)
			{
				TextureCreateInformation bestErrorTextureCreateInformation;
				bestErrorTextureCreateInformation.Type = TEX_TYPE_2D;
				bestErrorTextureCreateInformation.Format = PF_R32F;
				bestErrorTextureCreateInformation.Width = blockCountX;
				bestErrorTextureCreateInformation.Height = blockCountY;
				bestErrorTextureCreateInformation.Usage = TextureUsageFlag::StoreOnGPU | TextureUsageFlag::AllowUnorderedAccessOnTheGPU;

				bestErrorTexture = gpuDevice->CreateTexture(bestErrorTextureCreateInformation);
				if(bestErrorTexture == nullptr)
				{
					op.CompleteOperation(nullptr);
					return;
				}

				// +inf seed. Fill row by row so any staging row-pitch padding is respected. FLT_MAX is set explicitly (not via
				// Clear/SetColors, which may clamp) so the first encoding of every block always wins.
				bestErrorTextureInitialData = PixelData::Create(blockCountX, blockCountY, 1, PF_R32F);
				for(u32 y = 0; y < blockCountY; ++y)
				{
					float* const row = (float*)(bestErrorTextureInitialData->GetData() + (u64)y * bestErrorTextureInitialData->GetRowPitch());
					for(u32 x = 0; x < blockCountX; ++x)
						row[x] = FLT_MAX;
				}
			}

			TShared<render::GpuBuffer> tables[(u32)render::TextureCompressTable::Count];
			for(u32 tableIndex = 0; tableIndex < (u32)render::TextureCompressTable::Count; ++tableIndex)
			{
				if(!combinedRequirements.UsesTable[tableIndex])
					continue;

				const TArrayView<const u32> tableData = render::GetTextureCompressTableData((render::TextureCompressTable)tableIndex);
				tables[tableIndex] = gpuDevice->CreateGpuBuffer(GpuBufferCreateInformation::CreateStructuredStorage(sizeof(u32), (u32)tableData.Size()));
				if(tables[tableIndex] == nullptr)
				{
					op.CompleteOperation(nullptr);
					return;
				}
			}

			// Resources of one chunk submission in flight. A chunk reuses them only after the previous chunk using them completes.
			struct ChunkSlot
			{
				ChunkSlot() = default;

				TShared<render::GpuBuffer> ScratchBuffers[(u32)render::TextureCompressScratchBuffer::Count];
				TShared<render::GpuQueryPool> TimestampQueryPool;
				TInlineArray<render::GpuQueryId, 33> TimestampQueryIds; // Written before the first stage and after every stage
				u64 FenceValue = 0; // Signalled by the chunk's submission; 0 once the chunk is retired
				u32 BlockCount = 0;
			};

			ChunkSlot chunkSlots[kChunksInFlight];
			bool canMeasureChunks = true;
			for(ChunkSlot& chunkSlot : chunkSlots)
			{
				for(u32 bufferIndex = 0; bufferIndex < (u32)render::TextureCompressScratchBuffer::Count; ++bufferIndex)
				{
					const u32 elementsPerBlock = combinedRequirements.ScratchElementsPerBlock[bufferIndex];
					if(elementsPerBlock == 0)
						continue;

					chunkSlot.ScratchBuffers[bufferIndex] = render::CreateTextureCompressScratchBuffer(*gpuDevice, scratchBlockCountX * scratchBlockCountY * elementsPerBlock);
					if(chunkSlot.ScratchBuffers[bufferIndex] == nullptr)
					{
						op.CompleteOperation(nullptr);
						return;
					}
				}

				render::GpuQueryPoolCreateInformation timestampQueryPoolCreateInformation;
				timestampQueryPoolCreateInformation.Type = render::GpuQueryType::Timestamp;
				timestampQueryPoolCreateInformation.PoolSize = (u32)stages.Size() + 1;

				chunkSlot.TimestampQueryPool = gpuDevice->CreateQueryPool(timestampQueryPoolCreateInformation);
				canMeasureChunks &= chunkSlot.TimestampQueryPool != nullptr;
			}

			// Every submission signals the next value of the fence, so waiting for a chunk doesn't wait for the ones after it.
			// The command buffers are graphics ones, submitted on the first graphics queue.
			const TShared<GpuQueue> queue = gpuDevice->GetQueue(GQT_GRAPHICS, 0);
			const TShared<GpuTimelineFence> fence = gpuDevice->CreateTimelineFence();
			u64 lastFenceValue = 0;

			auto fnSubmit = [&gpuContext, &fence, &lastFenceValue](const TShared<render::GpuCommandBuffer>& commandBuffer)
			{
				GpuSubmissionInformation submissionInformation;
				submissionInformation.CommandBuffer = commandBuffer;
				submissionInformation.SignalFences.Add(GpuTimelineFenceAndValue{ fence, ++lastFenceValue });

				gpuContext.SubmitCommandBuffer(submissionInformation);
				return lastFenceValue;
			};
			float chunkBlockBudget = (float)(canMeasureChunks ? kInitialChunkBlockCount : kUnmeasuredChunkBlockCount);

			double stageMilliseconds[32];
			for(double& milliseconds : stageMilliseconds)
				milliseconds = 0.0;

			// Waits for the chunk last submitted from the slot to complete, and sizes the following chunks from its GPU time
			auto fnRetireChunkSlot = [&gpuDevice, &queue, &fence, &chunkBlockBudget, &stageMilliseconds, canMeasureChunks](ChunkSlot& chunkSlot)
			{
				if(chunkSlot.FenceValue == 0)
					return;

				queue->WaitUntilFence(fence, chunkSlot.FenceValue);
				chunkSlot.FenceValue = 0;

				const u32 timestampCount = (u32)chunkSlot.TimestampQueryIds.Size();
				if(!canMeasureChunks || timestampCount < 2 || !chunkSlot.TimestampQueryPool->TryResolve(false))
					return;

				for(const render::GpuQueryId& queryId : chunkSlot.TimestampQueryIds)
				{
					if(!queryId.IsValid())
						return;
				}

				for(u32 stageIndex = 0; stageIndex + 1 < timestampCount; ++stageIndex)
				{
					const u64 stageBegin = chunkSlot.TimestampQueryPool->GetQueryResult(chunkSlot.TimestampQueryIds[stageIndex]);
					const u64 stageEnd = chunkSlot.TimestampQueryPool->GetQueryResult(chunkSlot.TimestampQueryIds[stageIndex + 1]);
					if(stageEnd > stageBegin)
						stageMilliseconds[stageIndex] += gpuDevice->ConvertTimestampToMilliseconds(stageEnd - stageBegin);
				}

				const u64 chunkBegin = chunkSlot.TimestampQueryPool->GetQueryResult(chunkSlot.TimestampQueryIds[0]);
				const u64 chunkEnd = chunkSlot.TimestampQueryPool->GetQueryResult(chunkSlot.TimestampQueryIds[timestampCount - 1]);
				if(chunkEnd <= chunkBegin)
					return;

				// Grow gradually, as small chunks underestimate the cost of large ones whose work no longer fits the GPU at once
				const float chunkMilliseconds = Math::Max(gpuDevice->ConvertTimestampToMilliseconds(chunkEnd - chunkBegin), 1e-3f);
				const float chunkBlockCount = (float)chunkSlot.BlockCount;
				chunkBlockBudget = Math::Clamp(chunkBlockCount / chunkMilliseconds * kChunkTimeBudgetMilliseconds, (float)kInitialChunkBlockCount, Math::Max(chunkBlockCount * kMaximumChunkGrowth, (float)kInitialChunkBlockCount));
			};

			render::TextureCompressBindings bindings;
			bindings.Input = inputTexture;
			bindings.Output = outputTexture;
			bindings.BestError = bestErrorTexture;
			for(u32 tableIndex = 0; tableIndex < (u32)render::TextureCompressTable::Count; ++tableIndex)
				bindings.Tables[tableIndex] = tables[tableIndex];

			// Chunks are laid out in strips of block rows, with every strip and chunk sized to the current block budget
			u32 chunkIndex = 0;
			u32 stripStartRow = 0;
			while(stripStartRow < blockCountY)
			{
				const u32 stripRowBudget = (u32)std::sqrt(chunkBlockBudget) / 8u * 8u;
				const u32 stripRowCount = Math::Min(Math::Clamp(stripRowBudget, 8u, maximumChunkBlockCount), blockCountY - stripStartRow);

				u32 chunkStartColumn = 0;
				while(chunkStartColumn < blockCountX)
				{
					ChunkSlot& chunkSlot = chunkSlots[chunkIndex % kChunksInFlight];
					fnRetireChunkSlot(chunkSlot);

					const u32 chunkColumnBudget = (u32)(chunkBlockBudget / (float)stripRowCount) / 8u * 8u;
					const u32 chunkColumnCount = Math::Min(Math::Clamp(chunkColumnBudget, 8u, maximumChunkBlockCount), blockCountX - chunkStartColumn);
					const Vector2I chunkBlockCount((i32)chunkColumnCount, (i32)stripRowCount);
					const Vector2I chunkBlockOffset((i32)chunkStartColumn, (i32)stripStartRow);

					render::GpuCommandBufferCreateInformation commandBufferCreateInformation;
					commandBufferCreateInformation.Name = "TextureCompress";

					const TShared<render::GpuCommandBuffer> commandBuffer = pool->FindOrCreate(commandBufferCreateInformation);

					// The first chunk uploads the data shared by all chunks. The dispatch barriers make it visible to the stages.
					if(chunkIndex == 0)
					{
						render::TextureUtility::Write(gpuContext, inputTexture, *source, 0, 0, render::TextureWriteFlag::Normal, commandBuffer);

						if(bestErrorTexture != nullptr)
							render::TextureUtility::Write(gpuContext, bestErrorTexture, *bestErrorTextureInitialData, 0, 0, render::TextureWriteFlag::Normal, commandBuffer);

						for(u32 tableIndex = 0; tableIndex < (u32)render::TextureCompressTable::Count; ++tableIndex)
						{
							if(tables[tableIndex] == nullptr)
								continue;

							const TArrayView<const u32> tableData = render::GetTextureCompressTableData((render::TextureCompressTable)tableIndex);
							render::GpuBufferUtility::Write(gpuContext, tables[tableIndex], 0, (u32)(tableData.Size() * sizeof(u32)), tableData.Data(), render::GpuBufferWriteFlag::Normal, commandBuffer);
						}
					}

					chunkSlot.TimestampQueryIds.Clear();
					if(canMeasureChunks)
					{
						commandBuffer->ResetQueries(chunkSlot.TimestampQueryPool);
						chunkSlot.TimestampQueryIds.Add(chunkSlot.TimestampQueryPool->AllocateQuery());
						commandBuffer->WriteTimestamp(chunkSlot.TimestampQueryIds.Back(), chunkSlot.TimestampQueryPool);
					}

					// Stages only differ in their stage parameters, so stages with the same ones share a uniform buffer
					render::GpuBufferMappedScope chunkParameters[kMaximumParameterVariants];
					for(u32 variantIndex = 0; variantIndex < stageParameters.Size(); ++variantIndex)
					{
						const TShared<render::GpuBuffer> parameterBuffer = render::gTextureCompressParameters.CreateScratchBuffer(gpuContext);
						if(parameterBuffer == nullptr)
						{
							op.CompleteOperation(nullptr);
							return;
						}

						render::GpuBufferMappedScope& parameters = chunkParameters[variantIndex];
						parameters = parameterBuffer->Map(GpuMapOption::Write);
						render::gTextureCompressParameters.gTextureSize.Set(parameters, fullTextureSize);
						render::gTextureCompressParameters.gBlockCount.Set(parameters, fullBlockCount);
						render::gTextureCompressParameters.gBlockOffset.Set(parameters, chunkBlockOffset);
						render::gTextureCompressParameters.gScratchBlockCount.Set(parameters, chunkBlockCount);
						render::gTextureCompressParameters.gStageParameters.Set(parameters, stageParameters[variantIndex]);
						parameters.Unmap();
					}

					bindings.ChunkBlockCount = chunkBlockCount;
					for(u32 bufferIndex = 0; bufferIndex < (u32)render::TextureCompressScratchBuffer::Count; ++bufferIndex)
						bindings.ScratchBuffers[bufferIndex] = chunkSlot.ScratchBuffers[bufferIndex];

					for(const TShared<render::TextureCompressStage>& stage : stages)
					{
						const u32 variantIndex = (u32)(std::find(stageParameters.begin(), stageParameters.end(), stage->GetRequirements().StageParameters) - stageParameters.begin());
						bindings.Parameters = &chunkParameters[variantIndex];

						stage->Dispatch(*commandBuffer, bindings);

						if(canMeasureChunks)
						{
							chunkSlot.TimestampQueryIds.Add(chunkSlot.TimestampQueryPool->AllocateQuery());
							commandBuffer->WriteTimestamp(chunkSlot.TimestampQueryIds.Back(), chunkSlot.TimestampQueryPool);
						}
					}

					chunkSlot.FenceValue = fnSubmit(commandBuffer);
					chunkSlot.BlockCount = chunkColumnCount * stripRowCount;

					chunkStartColumn += chunkColumnCount;
					++chunkIndex;
				}

				stripStartRow += stripRowCount;
			}

#if B3D_BUILD_TYPE_DEVELOPMENT
			if(gGpuLogTextureCompressionTimings.Get())
			{
				for(u32 slotOffset = 0; slotOffset < kChunksInFlight; ++slotOffset)
					fnRetireChunkSlot(chunkSlots[(chunkIndex + slotOffset) % kChunksInFlight]);

				double totalMilliseconds = 0.0;
				for(u32 stageIndex = 0; stageIndex < stages.Size(); ++stageIndex)
				{
					B3D_LOG(Info, LogTexture, "[TextureCompress] stage {0} {1}: {2} ms", stageIndex, stages[stageIndex]->GetName(), stageMilliseconds[stageIndex]);
					totalMilliseconds += stageMilliseconds[stageIndex];
				}

				B3D_LOG(Info, LogTexture, "[TextureCompress] {0}x{1}: {2} stages, {3} chunks, {4} ms GPU time", width, height, stages.Size(), chunkIndex, totalMilliseconds);
			}
#endif

			// Final command buffer: a single non-blocking read-back of the whole output texture
			render::GpuCommandBufferCreateInformation readbackInfo;
			readbackInfo.Name = "TextureCompressReadback";

			const TShared<render::GpuCommandBuffer> readbackCommandBuffer = pool->FindOrCreate(readbackInfo);

			const u32 readSize = destination->GetConsecutiveSize();
			TAsyncOp<TShared<PixelData>> readOp = render::TextureUtility::ReadAsync(gpuContext, outputTexture, *readbackCommandBuffer);

			// Copy the packed blocks into the destination surface and complete the op once the GPU finishes
			readbackCommandBuffer->OnDidComplete.Connect(
				[op, readOp, destination, readSize]() mutable
				{
					const TShared<PixelData> data = readOp.GetReturnValue();
					if(data == nullptr)
					{
						op.CompleteOperation(nullptr);
						return;
					}

					const u32 rows = data->GetHeight();
					const u32 destinationRowByteCount = (rows != 0) ? (readSize / rows) : readSize; // tightly-packed block row
					const u32 sourceRowPitch = data->GetRowPitch();
					const u8* const sourceData = (const u8*)data->GetData();

					u8* const destinationData = destination->GetData();
					for(u32 y = 0; y < rows; ++y)
						memcpy(destinationData + (u64)y * destinationRowByteCount, sourceData + (u64)y * sourceRowPitch, destinationRowByteCount);

					op.CompleteOperation(destination);
				});

			const u64 readbackFenceValue = fnSubmit(readbackCommandBuffer);

			// Retire the remaining chunks and the read-back before the command buffer pool is destroyed on return. A pool
			// destroyed with buffers still executing waits for the whole device to go idle, stalling unrelated work on
			// every queue. This only waits for this compression's own submissions, and completes the op.
			queue->WaitUntilFence(fence, readbackFenceValue);
		}

	} // anonymous namespace

	bool GpuTextureCompressor::IsFormatSupported(PixelFormat format)
	{
		PixelFormat blockTextureFormat;
		bool isHDR;

		return render::GetTextureCompressFormatInformation(format, blockTextureFormat, isHDR);
	}

	TAsyncOp<TShared<PixelData>> GpuTextureCompressor::Compress(const TShared<PixelData>& source, const TShared<PixelData>& destination, const CompressionOptions& options)
	{
		// On failure we still return a *completed* op (with null) so a caller blocking on it never hangs.
		auto fnFailed = []()
		{
			TAsyncOp<TShared<PixelData>> op;
			op.CompleteOperation(nullptr);
			return op;
		};

		if(source == nullptr || destination == nullptr)
			return fnFailed();

		PixelFormat blockTextureFormat;
		bool isHDR;
		if(!render::GetTextureCompressFormatInformation(options.Format, blockTextureFormat, isHDR))
			return fnFailed();

		if(GetApplication().GetPrimaryGpuDevice() == nullptr)
			return fnFailed();

		// Convert the source to a surface the compute shader can sample: RGBA8 for LDR formats, RGBA32F for HDR (BC6H).
		const PixelFormat interimFormat = isHDR ? PF_RGBA32F : PF_RGBA8;
		const TShared<PixelData> convertedSource = PixelData::Create(source->GetWidth(), source->GetHeight(), 1, interimFormat);
		PixelUtility::BulkPixelConversion(*source, *convertedSource);

		const render::TextureCompressStageList stages = render::GetTextureCompressStages(options.Format, options.Quality);

		// Run inline if we are already on the render thread, otherwise marshal it across.
		TAsyncOp<TShared<PixelData>> op;
		auto fnGpuWork = [stages, convertedSource, interimFormat, blockTextureFormat, destination, maxChunkSize = options.MaxChunkSize, op]() mutable
		{
			CompressOnRenderThread(stages, convertedSource, interimFormat, blockTextureFormat, destination, maxChunkSize, op);
		};

		if(B3D_CURRENT_THREAD_ID == GetRenderThread().GetThreadId())
			fnGpuWork();
		else
			GetRenderThread().PostCommand(std::move(fnGpuWork), "GPU texture compression");

		return op;
	}

} // namespace b3d
