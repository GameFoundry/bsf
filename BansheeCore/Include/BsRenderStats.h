#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Common types to track resource statistics for.
	 */
	enum RenderStatResourceType
	{
		RenderStatObject_IndexBuffer,
		RenderStatObject_VertexBuffer,
		RenderStatObject_GpuBuffer,
		RenderStatObject_GpuParamBuffer,
		RenderStatObject_Texture,
		RenderStatObject_GpuProgram,
		RenderStatObject_Query
	};

	/**
	 * @brief	Object that stores various render statistics.
	 */
	struct BS_CORE_EXPORT RenderStatsData
	{
		RenderStatsData()
		: numDrawCalls(0), numRenderTargetChanges(0), numPresents(0), numClears(0),
		  numVertices(0), numPrimitives(0), numBlendStateChanges(0), numRasterizerStateChanges(0), 
		  numDepthStencilStateChanges(0), numTextureBinds(0), numSamplerBinds(0), numVertexBufferBinds(0), 
		  numIndexBufferBinds(0), numGpuParamBufferBinds(0), numGpuProgramBinds(0)
		{ }

		UINT64 numDrawCalls;
		UINT64 numRenderTargetChanges;
		UINT64 numPresents;
		UINT64 numClears;

		UINT64 numVertices;
		UINT64 numPrimitives;

		UINT64 numBlendStateChanges; 
		UINT64 numRasterizerStateChanges; 
		UINT64 numDepthStencilStateChanges;

		UINT64 numTextureBinds; 
		UINT64 numSamplerBinds; 
		UINT64 numVertexBufferBinds; 
		UINT64 numIndexBufferBinds;
		UINT64 numGpuParamBufferBinds;
		UINT64 numGpuProgramBinds; 

		UINT64 numResourceWrites;
		UINT64 numResourceReads;

		UINT64 numObjectsCreated; 
		UINT64 numObjectsDestroyed;
	};

	/**
	 * @brief	Tracks various render system statistics.
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT RenderStats : public Module<RenderStats>
	{
	public:
		/** Increments draw call counter indicating how many times were
		 *  render system API Draw methods called. */
		void incNumDrawCalls() { mData.numDrawCalls++; }

		/** Increments render target change counter indicating how many
		 *  times did the active render target change. */
		void incNumRenderTargetChanges() { mData.numRenderTargetChanges++; }

		/** Increments render target present counter indicating how many
		 *  times did the buffer swap happen. */
		void incNumPresents() { mData.numPresents++; }

		/** Increments render target clear counter indicating how many
		 *  times did the target the cleared, entirely or partially. */
		void incNumClears() { mData.numClears++; }

		/** Increments vertex draw counter indicating how many
		 *  vertices were sent to the pipeline. */
		void addNumVertices(UINT32 count) { mData.numVertices += count; }

		/** Increments primitive draw counter indicating how many
		 *  primitives were sent to the pipeline. */
		void addNumPrimitives(UINT32 count) { mData.numPrimitives += count; }

		/** Increments blend state change counter indicating how many
		 *  times was a blend state bound to the pipeline. */
		void incNumBlendStateChanges() { mData.numBlendStateChanges++; }

		/** Increments rasterizer state change counter indicating how many
		 *  times was a rasterizer state bound to the pipeline. */
		void incNumRasterizerStateChanges() { mData.numRasterizerStateChanges++; }

		/** Increments depth/stencil state change counter indicating how many
		 *  times was a depth/stencil state bound to the pipeline. */
		void incNumDepthStencilStateChanges() { mData.numDepthStencilStateChanges++; }

		/** Increments texture change counter indicating how many
		 *  times was a texture bound to the pipeline. */
		void incNumTextureBinds() { mData.numTextureBinds++; }

		/** Increments sampler state change counter indicating how many
		 *  times was a sampler state bound to the pipeline. */
		void incNumSamplerBinds() { mData.numSamplerBinds++; }

		/** Increments vertex buffer change counter indicating how many
		 *  times was a vertex buffer bound to the pipeline. */
		void incNumVertexBufferBinds() { mData.numVertexBufferBinds++; }

		/** Increments index buffer change counter indicating how many
		 *  times was a index buffer bound to the pipeline. */
		void incNumIndexBufferBinds() { mData.numIndexBufferBinds++; }

		/** Increments GPU parameter buffer change counter indicating how many
		 *  times was a GPU parameter buffer bound to the pipeline. */
		void incNumGpuParamBufferBinds() { mData.numGpuParamBufferBinds++; }

		/** Increments GPU program change counter indicating how many
		 *  times was a GPU program bound to the pipeline. */
		void incNumGpuProgramBinds() { mData.numGpuProgramBinds++; }

		/**
		 * Increments created GPU resource counter. 
		 *
		 * @param	category	Category of the resource.
		 */
		void incResCreated(UINT32 category) 
		{
			// TODO - I'm ignoring resourceType for now. Later I will want to
			// count object creation/destruction/read/write per type. I will
			// also want to allow the caller to assign names to specific "resourceType" id.
			// (Since many types will be RenderSystem specific).

			// TODO - I should also track number of active GPU objects using this method, instead
			// of just keeping track of how many were created and destroyed during the frame.

			mData.numObjectsCreated++;
		}

		/**
		 * Increments destroyed GPU resource counter. 
		 *
		 * @param	category	Category of the resource.
		 */
		void incResDestroyed(UINT32 category) { mData.numObjectsDestroyed++; }

		/**
		 * Increments GPU resource read counter. 
		 *
		 * @param	category	Category of the resource.
		 */
		void incResRead(UINT32 category) { mData.numResourceReads++; }

		/**
		 * Increments GPU resource write counter. 
		 *
		 * @param	category	Category of the resource.
		 */
		void incResWrite(UINT32 category) { mData.numResourceWrites++; }

		/**
		 * Returns an object containing various rendering statistics.
		 *			
		 * @note	Do not modify the returned state unless you know what you are doing, it will
		 *			change the actual internal object.
		 */
		RenderStatsData& getData() { return mData; }

	private:
		RenderStatsData mData;
	};

#if BS_PROFILING_ENABLED
	#define BS_INC_RENDER_STAT_CAT(Stat, Category) RenderStats::instance().inc##Stat##((UINT32)##Category##)
	#define BS_INC_RENDER_STAT(Stat) RenderStats::instance().inc##Stat##()
	#define BS_ADD_RENDER_STAT(Stat, Count) RenderStats::instance().add##Stat##(##Count##)
#else
	#define BS_INC_RENDER_STAT_CAT(Stat, Category)
	#define BS_INC_RENDER_STAT(Stat)
	#define BS_ADD_RENDER_STAT(Stat, Count)
#endif
}