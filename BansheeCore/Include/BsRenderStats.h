#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Object that stores various render statistics.
	 */
	struct RenderStats
	{
		RenderStats()
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
}