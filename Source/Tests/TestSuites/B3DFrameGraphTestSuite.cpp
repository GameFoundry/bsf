//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DFrameGraphTestSuite.h"
#include "Utility/B3DUnitTestGpuPipelines.h"
#include "CoreObject/B3DRenderThread.h"
#include "FrameGraph/B3DFrameGraph.h"
#include "GpuBackend/B3DGpuBackend.h"
#include "GpuBackend/B3DGpuBackendUtility.h"
#include "GpuBackend/B3DGpuDevice.h"
#include "GpuBackend/B3DGpuDeviceCapabilities.h"
#include "GpuBackend/B3DGpuCommandBuffer.h"
#include "GpuBackend/B3DGpuParameterSet.h"
#include "GpuBackend/B3DGpuParameterSetPool.h"
#include "GpuBackend/B3DGpuPipelineState.h"
#include "GpuBackend/B3DGpuPipelineParameterLayout.h"
#include "GpuBackend/B3DGpuTransientResourceAllocator.h"
#include "GpuBackend/B3DGpuWorkContext.h"

using namespace b3d;
using namespace b3d::render;

FrameGraphTestSuite::FrameGraphTestSuite()
	: TestSuite("FrameGraphTestSuite")
{
#if B3D_GPU_EXPLICIT_BARRIERS
	B3D_ADD_TEST(FrameGraphTestSuite::TestSplitBarriers)
	B3D_ADD_TEST(FrameGraphTestSuite::TestCulling)
	B3D_ADD_TEST(FrameGraphTestSuite::TestSplitAliasAcquire)
	B3D_ADD_TEST(FrameGraphTestSuite::TestExtract)
	B3D_ADD_TEST(FrameGraphTestSuite::TestExport)

#if B3D_BUILD_TYPE_DEVELOPMENT
	B3D_ADD_TEST(FrameGraphTestSuite::TestValidation)
#endif
#endif
}

#if B3D_GPU_EXPLICIT_BARRIERS
namespace
{
	constexpr u32 kSize = 16; /**< Width and height of the resources the test dispatch writes. */
	constexpr u32 kDispatchPattern = 0x5A000000; /**< High bits the test dispatch writes into every element. */

	/** D3D12 debug layer hint about the command buffers that only bring resources to rest or make readbacks visible to the host. */
	constexpr const char* kBarrierOnlyWarning = "references command lists that have recorded only Barrier commands";

	/** Resources and helpers the frame graph tests share. Lives on the render thread. */
	struct FrameGraphTestContext
	{
		FrameGraphTestContext(GpuDevice& device, const TShared<GpuComputePipelineState>& pipeline)
			: Device(device), Pipeline(pipeline), Context(GpuWorkContext::Create(device))
		{
			GpuCommandBufferPoolCreateInformation poolInformation = GpuCommandBufferPoolCreateInformation::CreateForThisThread(GQT_GRAPHICS);
			poolInformation.ExplicitBarriers = true;
			Pool = device.CreateGpuCommandBufferPool(poolInformation);

			TextureInformation.Width = kSize;
			TextureInformation.Height = kSize;
			TextureInformation.Format = PF_R32U;
			TextureInformation.Usage = TextureUsageFlag::AllowUnorderedAccessOnTheGPU;

			const ImageSubresourcePitch pitch = device.CreateTexture(TextureInformation)->GetStagingBufferPitchForSubresource(0, 0);
			TextureRowPitch = pitch.RowPitch;
			TextureReadbackSize = pitch.RowPitch * pitch.SliceHeight * sizeof(u32);
		}

		/** Creates a buffer the test dispatch can write. */
		TShared<render::GpuBuffer> CreateBuffer(const char* name) const
		{
			const TShared<render::GpuBuffer> buffer = Device.CreateGpuBuffer(GpuBufferCreateInformation::CreateStructuredStorage(sizeof(u32), kSize * kSize,
				GpuBufferFlag::StoreOnGPU | GpuBufferFlag::AllowUnorderedAccessOnTheGPU));
			buffer->SetName(name);
			return buffer;
		}

		/** Creates a texture the test dispatch can write, and brings it to rest. */
		TShared<render::Texture> CreateTexture(const char* name) const
		{
			TextureCreateInformation information = TextureInformation;
			information.Name = name;

			const TShared<render::Texture> texture = Device.CreateTexture(information);
			const TShared<render::GpuCommandBuffer> commands = Pool->Create(GpuCommandBufferCreateInformation::Create("Bring to rest"));
			const GpuAccessState undefined(GpuStageFlag::None, GpuAccessFlag::None, GpuImageLayout::Undefined);
			commands->IssueExplicitBarriers(GpuExplicitTextureBarrier(texture, undefined, GpuBackendUtility::GetRestingState(*texture, commands->GetQueueType())));
			Context->SubmitCommandBuffer(commands, GpuQueueMask::kNone);

			return texture;
		}

		/** Creates a buffer the host can read @p size bytes from. */
		TShared<render::GpuBuffer> CreateReadback(u32 size) const
		{
			return Device.CreateGpuBuffer(GpuBufferCreateInformation::CreateStagingRead(size));
		}

		/** Adds a pass that dispatches the test pipeline into @p buffer and @p texture. */
		void AddDispatch(FrameGraph& graph, const char* name, const TShared<render::GpuBuffer>& buffer, const TShared<render::Texture>& texture) const
		{
			FrameGraphComputePass pass;
			const TShared<render::GpuParameterSet> parameters = Context->GetParameterSetPool().Create(Pipeline->GetParameterLayout()->GetSet(0), 0);
			pass.AddParameterSet(parameters);
			parameters->SetStorageBuffer("OutputBuffer", buffer);
			parameters->SetStorageTexture("OutputTexture", texture, TextureSurface());

			const TShared<GpuComputePipelineState> pipeline = Pipeline;
			graph.AddComputePass(name, std::move(pass), [pipeline, parameters](FrameGraphPassContext& passContext)
			{
				render::GpuCommandBuffer& commands = passContext.GetCommandBuffer();
				commands.SetGpuComputePipelineState(pipeline);
				commands.SetGpuParameterSet(parameters);
				commands.DispatchCompute(1, 1, 1);
			});
		}

		/** Adds a pass that copies @p buffer and @p texture into @p bufferReadback and @p textureReadback, which the graph imports. */
		void AddReadback(FrameGraph& graph, const char* name, const TShared<render::GpuBuffer>& buffer, const TShared<render::Texture>& texture, const TShared<render::GpuBuffer>& bufferReadback,
			const TShared<render::GpuBuffer>& textureReadback) const
		{
			graph.ImportBuffer(bufferReadback);
			graph.ImportBuffer(textureReadback);

			FrameGraphCopyPass pass;
			pass.AddSource(buffer);
			pass.AddSource(texture);
			pass.AddDestination(bufferReadback);
			pass.AddDestination(textureReadback);

			graph.AddCopyPass(name, std::move(pass), [buffer, texture, bufferReadback, textureReadback](FrameGraphPassContext& passContext)
			{
				render::GpuCommandBuffer& commands = passContext.GetCommandBuffer();
				commands.CopyBufferToBuffer(buffer, bufferReadback, 0, 0, kSize * kSize * sizeof(u32));
				commands.CopyTextureToBuffer(texture, textureReadback, 0, 0, 0);
			});
		}

		/** Makes the GPU writes of the readbacks, which a graph left at rest, visible to the host, and waits for the GPU to finish. */
		void FinishReadbacks(std::initializer_list<TShared<render::GpuBuffer>> readbacks) const
		{
			const TShared<render::GpuCommandBuffer> commands = Pool->Create(GpuCommandBufferCreateInformation::Create("Host visibility"));
			for(const TShared<render::GpuBuffer>& readback : readbacks)
			{
				const GpuAccessState restingState = GpuBackendUtility::GetRestingState(*readback, GQT_GRAPHICS);
				commands->IssueExplicitBarriers(GpuExplicitBufferBarrier(readback, restingState, GpuAccessState(GpuStageFlag::Host, GpuAccessFlag::Read)));
			}

			Context->SubmitCommandBuffer(commands, GpuQueueMask::kNone);
			Device.WaitUntilIdle();
		}

		/** Returns true if @p readback holds the test dispatch's values, laid out as the buffer, or as a texture copy if @p isTexture is true. */
		bool HasDispatchValues(const TShared<render::GpuBuffer>& readback, bool isTexture) const
		{
			const render::GpuBufferMappedScope mapping = readback->Map(GpuMapOption::Read);
			if(!mapping.IsValid())
				return false;

			const u32* elements = static_cast<const u32*>(mapping.GetMappedMemory());
			for(u32 row = 0; row < kSize; row++)
			{
				for(u32 column = 0; column < kSize; column++)
				{
					const u32 elementIndex = isTexture ? row * TextureRowPitch + column : row * kSize + column;
					if(elements[elementIndex] != (kDispatchPattern | (row * kSize + column)))
						return false;
				}
			}

			return true;
		}

		GpuDevice& Device;
		TShared<GpuComputePipelineState> Pipeline;
		TShared<GpuWorkContext> Context;
		TShared<GpuCommandBufferPool> Pool;
		TextureCreateInformation TextureInformation;
		u32 TextureRowPitch = 0;
		u32 TextureReadbackSize = 0;
	};

	/** Returns the device the tests run on, or null if it does not support explicit barriers. */
	TShared<GpuDevice> GetTestDevice()
	{
		GpuBackend& backend = GpuBackend::Instance();
		if(backend.GetDeviceCount() == 0 || !backend.GetDevice(0)->GetCapabilities().HasCapability(RSC_EXPLICIT_BARRIERS))
			return nullptr;

		return backend.GetDevice(0);
	}
} // namespace

void FrameGraphTestSuite::TestSplitBarriers()
{
	const TShared<GpuDevice> device = GetTestDevice();
	const TShared<GpuComputePipelineState> pipeline = device != nullptr ? CreateTestDispatchPipeline(*device) : nullptr;
	if(pipeline == nullptr)
		return;

	LoggingScope logs(*this);
	logs.IgnoreWarning(kBarrierOnlyWarning);

	GetRenderThread().PostCommand([this, &device, &pipeline]()
	{
		const FrameGraphTestContext test(*device, pipeline);
		const TShared<render::GpuBuffer> producedBuffer = test.CreateBuffer("Produced buffer");
		const TShared<render::Texture> producedTexture = test.CreateTexture("Produced texture");
		const TShared<render::GpuBuffer> bufferReadback = test.CreateReadback(kSize * kSize * sizeof(u32));
		const TShared<render::GpuBuffer> textureReadback = test.CreateReadback(test.TextureReadbackSize);

		{
			FrameGraph graph(*test.Context);
			graph.ImportBuffer(producedBuffer);
			graph.ImportTexture(producedTexture);
			test.AddDispatch(graph, "Producer", producedBuffer, producedTexture);

			// Imported resources are observed after the graph, so the unrelated passes survive
			for(const char* name : { "Unrelated 1", "Unrelated 2" })
			{
				const TShared<render::GpuBuffer> buffer = test.CreateBuffer(name);
				const TShared<render::Texture> texture = test.CreateTexture(name);
				graph.ImportBuffer(buffer);
				graph.ImportTexture(texture);
				test.AddDispatch(graph, name, buffer, texture);
			}

			test.AddReadback(graph, "Consumer", producedBuffer, producedTexture, bufferReadback, textureReadback);
			graph.Compile();

			// The producer's writes reach the consumer through one batch of split barriers
			const FrameGraphStatistics& statistics = graph.GetStatistics();
			B3D_TEST_ASSERT(statistics.CulledPassCount == 0)
			B3D_TEST_ASSERT(statistics.SplitBarrierCount >= 2)
			B3D_TEST_ASSERT(statistics.BarrierBatchCount >= 1)

			graph.Execute();
		}

		test.FinishReadbacks({ bufferReadback, textureReadback });
		B3D_TEST_ASSERT_MSG(test.HasDispatchValues(bufferReadback, false), "The consumer does not read what the producer wrote into the buffer.")
		B3D_TEST_ASSERT_MSG(test.HasDispatchValues(textureReadback, true), "The consumer does not read what the producer wrote into the texture.")
		B3D_TEST_ASSERT(producedBuffer->GetFrameGraphResourceIndex() == ~0u && producedTexture->GetFrameGraphResourceIndex() == ~0u)

		device->EndFrame();
	}, "FrameGraphTestSuite::TestSplitBarriers", true);
}

void FrameGraphTestSuite::TestCulling()
{
	const TShared<GpuDevice> device = GetTestDevice();
	const TShared<GpuComputePipelineState> pipeline = device != nullptr ? CreateTestDispatchPipeline(*device) : nullptr;
	if(pipeline == nullptr)
		return;

	LoggingScope logs(*this);
	logs.IgnoreWarning(kBarrierOnlyWarning);

	GetRenderThread().PostCommand([this, &device, &pipeline]()
	{
		const FrameGraphTestContext test(*device, pipeline);

		TextureCreateInformation targetInformation;
		targetInformation.Width = kSize;
		targetInformation.Height = kSize;
		targetInformation.Format = PF_RGBA8;
		targetInformation.Usage = TextureUsageFlag::RenderTarget;
		targetInformation.ClearColor = Color(1.0f, 0.0f, 0.0f, 1.0f);

		const ImageSubresourcePitch pitch = device->CreateTexture(targetInformation)->GetStagingBufferPitchForSubresource(0, 0);
		const TShared<render::GpuBuffer> readback = test.CreateReadback(pitch.RowPitch * pitch.SliceHeight * sizeof(u32));

		{
			FrameGraph graph(*test.Context);
			const TShared<render::Texture> target = graph.CreateTexture("Cleared target", targetInformation);
			B3D_TEST_ASSERT(target != nullptr)
			if(target == nullptr)
				return;

			// The second clear hides the first one from the read
			for(const char* name : { "First clear", "Second clear" })
			{
				FrameGraphRasterPass pass;
				pass.SetColorAttachment(0, target, FrameGraphAttachmentLoad::Clear);
				graph.AddRasterPass(name, std::move(pass), nullptr);
			}

			graph.ImportBuffer(readback);

			FrameGraphCopyPass copyPass;
			copyPass.AddSource(target);
			copyPass.AddDestination(readback);
			graph.AddCopyPass("Read", std::move(copyPass), [target, readback](FrameGraphPassContext& passContext)
			{
				passContext.GetCommandBuffer().CopyTextureToBuffer(target, readback, 0, 0);
			});

			// Nothing reads the transients of this pass, so they are never placed
			test.AddDispatch(graph, "Unobserved", graph.CreateBuffer("Unobserved buffer", GpuBufferCreateInformation::CreateStructuredStorage(sizeof(u32), kSize * kSize,
				GpuBufferFlag::StoreOnGPU | GpuBufferFlag::AllowUnorderedAccessOnTheGPU)), graph.CreateTexture("Unobserved texture", test.TextureInformation));

			graph.Compile();
			B3D_TEST_ASSERT(graph.IsPassCulled(0))
			B3D_TEST_ASSERT(!graph.IsPassCulled(1))
			B3D_TEST_ASSERT(!graph.IsPassCulled(2))
			B3D_TEST_ASSERT(graph.IsPassCulled(3))
			B3D_TEST_ASSERT(graph.GetStatistics().CulledPassCount == 2)
			B3D_TEST_ASSERT(graph.GetStatistics().AliasAcquireCount == 1)

			graph.Execute();
		}

		test.FinishReadbacks({ readback });

		const render::GpuBufferMappedScope mapping = readback->Map(GpuMapOption::Read);
		B3D_TEST_ASSERT(mapping.IsValid())
		if(!mapping.IsValid())
			return;

		bool isCleared = true;
		const u32* pixels = static_cast<const u32*>(mapping.GetMappedMemory());
		for(u32 row = 0; row < kSize; row++)
		{
			for(u32 column = 0; column < kSize; column++)
				isCleared &= pixels[row * pitch.RowPitch + column] == 0xFF0000FF;
		}

		B3D_TEST_ASSERT_MSG(isCleared, "The read does not see the clear of the surviving raster pass.")

		device->EndFrame();
	}, "FrameGraphTestSuite::TestCulling", true);
}

void FrameGraphTestSuite::TestSplitAliasAcquire()
{
	const TShared<GpuDevice> device = GetTestDevice();
	const TShared<GpuComputePipelineState> pipeline = device != nullptr ? CreateTestDispatchPipeline(*device) : nullptr;
	if(pipeline == nullptr)
		return;

	LoggingScope logs(*this);
	logs.IgnoreWarning(kBarrierOnlyWarning);

	GetRenderThread().PostCommand([this, &device, &pipeline]()
	{
		const FrameGraphTestContext test(*device, pipeline);
		const GpuBufferCreateInformation bufferInformation = GpuBufferCreateInformation::CreateStructuredStorage(sizeof(u32), kSize * kSize,
			GpuBufferFlag::StoreOnGPU | GpuBufferFlag::AllowUnorderedAccessOnTheGPU);

		TShared<render::GpuBuffer> readbacks[4];
		for(u32 readbackIndex = 0; readbackIndex < 4; readbackIndex++)
			readbacks[readbackIndex] = test.CreateReadback(readbackIndex % 2 == 0 ? kSize * kSize * (u32)sizeof(u32) : test.TextureReadbackSize);

		const TShared<render::GpuBuffer> unrelatedBuffer = test.CreateBuffer("Unrelated buffer");
		const TShared<render::Texture> unrelatedTexture = test.CreateTexture("Unrelated texture");

		{
			FrameGraph graph(*test.Context);

			// The second pair of transients can take the memory the first pair releases after its read, with a pass in between
			const TShared<render::GpuBuffer> firstBuffer = graph.CreateBuffer("First buffer", bufferInformation);
			const TShared<render::Texture> firstTexture = graph.CreateTexture("First texture", test.TextureInformation);
			test.AddDispatch(graph, "First write", firstBuffer, firstTexture);
			test.AddReadback(graph, "First read", firstBuffer, firstTexture, readbacks[0], readbacks[1]);

			graph.ImportBuffer(unrelatedBuffer);
			graph.ImportTexture(unrelatedTexture);
			test.AddDispatch(graph, "Unrelated", unrelatedBuffer, unrelatedTexture);

			const TShared<render::GpuBuffer> secondBuffer = graph.CreateBuffer("Second buffer", bufferInformation);
			const TShared<render::Texture> secondTexture = graph.CreateTexture("Second texture", test.TextureInformation);
			test.AddDispatch(graph, "Second write", secondBuffer, secondTexture);
			test.AddReadback(graph, "Second read", secondBuffer, secondTexture, readbacks[2], readbacks[3]);

			graph.Compile();
			B3D_TEST_ASSERT(graph.GetStatistics().AliasAcquireCount == 4)
			graph.Execute();

			// When the second pair shares memory with the first, its alias acquires are released after the first read
			const GpuTransientStatistics transientStatistics = test.Context->GetTransientResourceAllocator().GetStatistics();
			if(transientStatistics.LastScope.PeakUsed < transientStatistics.LastScope.RequestedBytes)
				B3D_TEST_ASSERT(graph.GetStatistics().SplitAliasAcquireCount >= 1)
		}

		test.FinishReadbacks({ readbacks[0], readbacks[1], readbacks[2], readbacks[3] });
		B3D_TEST_ASSERT_MSG(test.HasDispatchValues(readbacks[0], false), "The first transient buffer does not hold its values.")
		B3D_TEST_ASSERT_MSG(test.HasDispatchValues(readbacks[1], true), "The first transient texture does not hold its values.")
		B3D_TEST_ASSERT_MSG(test.HasDispatchValues(readbacks[2], false), "The second transient buffer does not hold its values.")
		B3D_TEST_ASSERT_MSG(test.HasDispatchValues(readbacks[3], true), "The second transient texture does not hold its values.")

		device->EndFrame();
	}, "FrameGraphTestSuite::TestSplitAliasAcquire", true);
}

void FrameGraphTestSuite::TestExtract()
{
	const TShared<GpuDevice> device = GetTestDevice();
	const TShared<GpuComputePipelineState> pipeline = device != nullptr ? CreateTestDispatchPipeline(*device) : nullptr;
	if(pipeline == nullptr)
		return;

	LoggingScope logs(*this);
	logs.IgnoreWarning(kBarrierOnlyWarning);

	GetRenderThread().PostCommand([this, &device, &pipeline]()
	{
		const FrameGraphTestContext test(*device, pipeline);
		const GpuBufferCreateInformation bufferInformation = GpuBufferCreateInformation::CreateStructuredStorage(sizeof(u32), kSize * kSize,
			GpuBufferFlag::StoreOnGPU | GpuBufferFlag::AllowUnorderedAccessOnTheGPU);

		TShared<render::GpuBuffer> extractedBuffer;
		TShared<render::Texture> extractedTexture;
		{
			FrameGraph graph(*test.Context);
			extractedBuffer = graph.CreateBuffer("Extracted buffer", bufferInformation);
			extractedTexture = graph.CreateTexture("Extracted texture", test.TextureInformation);
			test.AddDispatch(graph, "Write", extractedBuffer, extractedTexture);
			graph.ExtractBuffer(extractedBuffer);
			graph.ExtractTexture(extractedTexture);

			graph.Compile();
			B3D_TEST_ASSERT(!graph.IsPassCulled(0))
			B3D_TEST_ASSERT(graph.GetStatistics().AliasAcquireCount == 0)
			graph.Execute();
		}

		// Extracted resources live in persistent memory, which no transient shares
		B3D_TEST_ASSERT(!extractedBuffer->HasFixedLocation() && !extractedTexture->HasFixedLocation())
		B3D_TEST_ASSERT(extractedBuffer->GetFrameGraphResourceIndex() == ~0u && extractedTexture->GetFrameGraphResourceIndex() == ~0u)

		device->EndFrame();

		const TShared<render::GpuBuffer> bufferReadback = test.CreateReadback(kSize * kSize * sizeof(u32));
		const TShared<render::GpuBuffer> textureReadback = test.CreateReadback(test.TextureReadbackSize);
		{
			FrameGraph graph(*test.Context);
			graph.ImportBuffer(extractedBuffer);
			graph.ImportTexture(extractedTexture);
			test.AddReadback(graph, "Read", extractedBuffer, extractedTexture, bufferReadback, textureReadback);

			graph.Compile();
			graph.Execute();
		}

		test.FinishReadbacks({ bufferReadback, textureReadback });
		B3D_TEST_ASSERT_MSG(test.HasDispatchValues(bufferReadback, false), "The extracted buffer does not hold the values written before its extraction.")
		B3D_TEST_ASSERT_MSG(test.HasDispatchValues(textureReadback, true), "The extracted texture does not hold the values written before its extraction.")

		device->EndFrame();
	}, "FrameGraphTestSuite::TestExtract", true);
}

void FrameGraphTestSuite::TestExport()
{
	const TShared<GpuDevice> device = GetTestDevice();
	const TShared<GpuComputePipelineState> pipeline = device != nullptr ? CreateTestDispatchPipeline(*device) : nullptr;
	if(pipeline == nullptr)
		return;

	LoggingScope logs(*this);
	logs.IgnoreWarning(kBarrierOnlyWarning);

	GetRenderThread().PostCommand([this, &device, &pipeline]()
	{
		const FrameGraphTestContext test(*device, pipeline);
		const TShared<render::GpuBuffer> buffer = test.CreateBuffer("Buffer at graph end");
		const TShared<render::Texture> texture = test.CreateTexture("Exported texture");

		{
			// The texture returns to rest right after its pass, the buffer at the end of the graph
			FrameGraph graph(*test.Context);
			graph.ImportBuffer(buffer);
			graph.ImportTexture(texture);

			FrameGraphComputePass pass;
			const TShared<render::GpuParameterSet> parameters = test.Context->GetParameterSetPool().Create(pipeline->GetParameterLayout()->GetSet(0), 0);
			pass.AddParameterSet(parameters);
			parameters->SetStorageBuffer("OutputBuffer", buffer);
			parameters->SetStorageTexture("OutputTexture", texture, TextureSurface());
			pass.Export(texture);

			graph.AddComputePass("Write", std::move(pass), [&pipeline, parameters](FrameGraphPassContext& passContext)
			{
				render::GpuCommandBuffer& commands = passContext.GetCommandBuffer();
				commands.SetGpuComputePipelineState(pipeline);
				commands.SetGpuParameterSet(parameters);
				commands.DispatchCompute(1, 1, 1);
			});

			const TShared<render::GpuBuffer> unrelatedBuffer = test.CreateBuffer("Unrelated buffer");
			const TShared<render::Texture> unrelatedTexture = test.CreateTexture("Unrelated texture");
			graph.ImportBuffer(unrelatedBuffer);
			graph.ImportTexture(unrelatedTexture);
			test.AddDispatch(graph, "Unrelated", unrelatedBuffer, unrelatedTexture);
			graph.Compile();
			graph.Execute();
		}

		device->EndFrame();

		// A barrier from rest of a resource in another state is reported by the backends' validation
		const TShared<render::GpuBuffer> bufferReadback = test.CreateReadback(kSize * kSize * sizeof(u32));
		const TShared<render::GpuBuffer> textureReadback = test.CreateReadback(test.TextureReadbackSize);
		{
			FrameGraph graph(*test.Context);
			graph.ImportBuffer(buffer);
			graph.ImportTexture(texture);
			test.AddReadback(graph, "Read", buffer, texture, bufferReadback, textureReadback);

			graph.Compile();
			graph.Execute();
		}

		test.FinishReadbacks({ bufferReadback, textureReadback });
		B3D_TEST_ASSERT_MSG(test.HasDispatchValues(bufferReadback, false), "The buffer returned to rest at the end of the graph does not hold its values.")
		B3D_TEST_ASSERT_MSG(test.HasDispatchValues(textureReadback, true), "The exported texture does not hold its values.")

		device->EndFrame();
	}, "FrameGraphTestSuite::TestExport", true);
}

#if B3D_BUILD_TYPE_DEVELOPMENT
void FrameGraphTestSuite::TestValidation()
{
	const TShared<GpuDevice> device = GetTestDevice();
	const TShared<GpuComputePipelineState> pipeline = device != nullptr ? CreateTestDispatchPipeline(*device) : nullptr;
	if(pipeline == nullptr)
		return;

	const String drawDependency = "The resource was accessed by a draw or dispatch after another draw or dispatch wrote it, without a barrier in between.";

	LoggingScope logs(*this);
	logs.IgnoreWarning(kBarrierOnlyWarning);
	logs.ExpectError("Pass 'Resting write' writes 'Resting buffer' through slot 'OutputBuffer', but it is not a resource of the graph. Import it first.");
	logs.ExpectError("Pass 'Resting write' writes 'Resting texture' through slot 'OutputTexture', but it is not a resource of the graph. Import it first.");
	logs.ExpectError("Pass 'Conflict' accesses the same subresource of 'Conflict texture' in two layouts. No barrier can separate them, so split the pass.");
	logs.ExpectError("Pass 'After export' writes 'Exported buffer' after it was exported.");
	logs.ExpectError(drawDependency + " Buffer: 'Twice buffer'.");
	logs.ExpectError(drawDependency + " Image: 'Twice texture'.");
	logs.ExpectError(drawDependency + " Buffer: 'Undeclared buffer'.");
	logs.ExpectError(drawDependency + " Image: 'Undeclared texture'.");

	GetRenderThread().PostCommand([&device, &pipeline]()
	{
		const FrameGraphTestContext test(*device, pipeline);

		// A storage slot can only hold resources of the graph
		{
			FrameGraph graph(*test.Context);
			test.AddDispatch(graph, "Resting write", test.CreateBuffer("Resting buffer"), test.CreateTexture("Resting texture"));
			graph.Compile();
			graph.Execute();
		}

		// No barrier can separate a copy's source and destination on the same subresource
		{
			FrameGraph graph(*test.Context);
			const TShared<render::Texture> texture = graph.CreateTexture("Conflict texture", test.TextureInformation);

			FrameGraphCopyPass pass;
			pass.AddSource(texture);
			pass.AddDestination(texture);
			graph.AddCopyPass("Conflict", std::move(pass), nullptr);
			graph.Compile();
			graph.Execute();
		}

		// Readers of an exported resource rely on its resting state
		{
			FrameGraph graph(*test.Context);
			const TShared<render::GpuBuffer> buffer = test.CreateBuffer("Exported buffer");
			const TShared<render::Texture> texture = test.CreateTexture("Exported texture");
			graph.ImportBuffer(buffer);
			graph.ImportTexture(texture);

			FrameGraphComputePass pass;
			const TShared<render::GpuParameterSet> parameters = test.Context->GetParameterSetPool().Create(pipeline->GetParameterLayout()->GetSet(0), 0);
			pass.AddParameterSet(parameters);
			parameters->SetStorageBuffer("OutputBuffer", buffer);
			parameters->SetStorageTexture("OutputTexture", texture, TextureSurface());
			pass.Export(buffer);

			const TShared<GpuComputePipelineState> dispatchPipeline = pipeline;
			graph.AddComputePass("Export", std::move(pass), [dispatchPipeline, parameters](FrameGraphPassContext& passContext)
			{
				passContext.GetCommandBuffer().SetGpuComputePipelineState(dispatchPipeline);
				passContext.GetCommandBuffer().SetGpuParameterSet(parameters);
				passContext.GetCommandBuffer().DispatchCompute(1, 1, 1);
			});

			test.AddDispatch(graph, "After export", buffer, texture);
			graph.Compile();
			graph.Execute();
		}

		// Two dispatches of one pass have no barrier between them
		{
			FrameGraph graph(*test.Context);
			const TShared<render::GpuBuffer> buffer = test.CreateBuffer("Twice buffer");
			const TShared<render::Texture> texture = test.CreateTexture("Twice texture");
			graph.ImportBuffer(buffer);
			graph.ImportTexture(texture);

			FrameGraphComputePass pass;
			const TShared<render::GpuParameterSet> parameters = test.Context->GetParameterSetPool().Create(pipeline->GetParameterLayout()->GetSet(0), 0);
			pass.AddParameterSet(parameters);
			parameters->SetStorageBuffer("OutputBuffer", buffer);
			parameters->SetStorageTexture("OutputTexture", texture, TextureSurface());

			const TShared<GpuComputePipelineState> dispatchPipeline = pipeline;
			graph.AddComputePass("Twice", std::move(pass), [dispatchPipeline, parameters](FrameGraphPassContext& passContext)
			{
				render::GpuCommandBuffer& commands = passContext.GetCommandBuffer();
				commands.SetGpuComputePipelineState(dispatchPipeline);
				commands.SetGpuParameterSet(parameters);
				commands.DispatchCompute(1, 1, 1);
				commands.DispatchCompute(1, 1, 1);
			});

			graph.Compile();
			graph.Execute();
		}

		// A set the pass did not create declares nothing, so the graph places no barrier for its resources
		{
			FrameGraph graph(*test.Context);
			const TShared<render::GpuBuffer> buffer = test.CreateBuffer("Undeclared buffer");
			const TShared<render::Texture> texture = test.CreateTexture("Undeclared texture");
			graph.ImportBuffer(buffer);
			graph.ImportTexture(texture);
			test.AddDispatch(graph, "Declared", buffer, texture);

			const TShared<render::GpuParameterSet> undeclared = test.Context->GetParameterSetPool().Create(pipeline->GetParameterLayout()->GetSet(0), 0);
			undeclared->SetStorageBuffer("OutputBuffer", buffer);
			undeclared->SetStorageTexture("OutputTexture", texture, TextureSurface());

			// The pass declares writes of its own resources, so it survives
			const TShared<render::GpuBuffer> otherBuffer = test.CreateBuffer("Other buffer");
			const TShared<render::Texture> otherTexture = test.CreateTexture("Other texture");
			graph.ImportBuffer(otherBuffer);
			graph.ImportTexture(otherTexture);

			FrameGraphComputePass pass;
			const TShared<render::GpuParameterSet> parameters = test.Context->GetParameterSetPool().Create(pipeline->GetParameterLayout()->GetSet(0), 0);
			pass.AddParameterSet(parameters);
			parameters->SetStorageBuffer("OutputBuffer", otherBuffer);
			parameters->SetStorageTexture("OutputTexture", otherTexture, TextureSurface());

			const TShared<GpuComputePipelineState> dispatchPipeline = pipeline;
			graph.AddComputePass("Undeclared", std::move(pass), [dispatchPipeline, parameters, undeclared](FrameGraphPassContext& passContext)
			{
				render::GpuCommandBuffer& commands = passContext.GetCommandBuffer();
				commands.SetGpuComputePipelineState(dispatchPipeline);
				commands.SetGpuParameterSet(parameters);
				commands.DispatchCompute(1, 1, 1);
				commands.SetGpuParameterSet(undeclared);
				commands.DispatchCompute(1, 1, 1);
			});

			graph.Compile();
			graph.Execute();
		}

		device->WaitUntilIdle();
		device->EndFrame();
	}, "FrameGraphTestSuite::TestValidation", true);
}
#endif
#endif
