#include "BsGPUProfiler.h"
#include "CmRenderSystem.h"
#include "CmTimerQuery.h"
#include "CmOcclusionQuery.h"
#include "CmException.h"

namespace BansheeEngine
{
	GPUProfiler::GPUProfiler()
		:mNumActiveSamples(0), mIsFrameActive(false)
	{ }

	void GPUProfiler::beginFrame()
	{
		if (mIsFrameActive)
			BS_EXCEPT(InvalidStateException, "Cannot begin a frame because another frame is active.");

		mActiveFrame = ActiveFrame();

		mActiveFrame.frameSample.sampleName = "Frame";
		beginSampleInternal(mActiveFrame.frameSample);

		mIsFrameActive = true;
	}

	void GPUProfiler::endFrame()
	{
		if (mNumActiveSamples > 0)
			BS_EXCEPT(InvalidStateException, "Attempting to end a frame while a sample is active.");

		if (!mIsFrameActive)
			return;

		endSampleInternal(mActiveFrame.frameSample);

		mUnresolvedFrames.push(mActiveFrame);
		mIsFrameActive = false;
	}

	void GPUProfiler::beginSample(const ProfilerString& name)
	{
		if (!mIsFrameActive)
			BS_EXCEPT(InvalidStateException, "Cannot begin a sample because no frame is active.");

		mActiveFrame.samples.push_back(ActiveSample());
		ActiveSample& sample = mActiveFrame.samples.back();

		sample.sampleName = name;
		beginSampleInternal(sample);
		mNumActiveSamples++;
	}

	void GPUProfiler::endSample(const ProfilerString& name)
	{
		if (mNumActiveSamples == 0)
			return;

		ActiveSample& sample = mActiveFrame.samples.back();
		if (sample.sampleName != name)
		{
			String errorStr = "Attempting to end a sample that doesn't match. Got: " + 
				String(name.c_str()) + ". Expected: " + String(sample.sampleName.c_str());

			BS_EXCEPT(InvalidStateException, errorStr);
		}

		endSampleInternal(sample);
		mNumActiveSamples--;
	}

	UINT32 GPUProfiler::getNumAvailableReports()
	{
		return (UINT32)mReadyReports.size();
	}

	GPUProfilerReport GPUProfiler::getNextReport()
	{
		if (mReadyReports.empty())
			BS_EXCEPT(InvalidStateException, "No reports are available.")

		GPUProfilerReport report = mReadyReports.front();
		mReadyReports.pop();

		return report;
	}

	void GPUProfiler::_update()
	{
		while (!mUnresolvedFrames.empty())
		{
			ActiveFrame& frame = mUnresolvedFrames.front();

			// Frame sample timer query is the last query we issued
			// so if it is complete, we may assume all queries are complete.
			if (frame.frameSample.activeTimeQuery->isReady())
			{
				GPUProfilerReport report = resolveFrame(frame);
				mUnresolvedFrames.pop();

				mReadyReports.push(report);
			}
			else
				break;
		}
	}

	GPUProfilerReport GPUProfiler::resolveFrame(ActiveFrame& frame)
	{
		GPUProfilerReport report;
		
		resolveSample(frame.frameSample, report.frameSample);

		for (auto& sample : frame.samples)
		{
			report.samples.push_back(GPUProfileSample());
			GPUProfileSample& newSample = report.samples.back();

			resolveSample(sample, newSample);
		}

		return report;
	}

	void GPUProfiler::resolveSample(const ActiveSample& sample, GPUProfileSample& reportSample)
	{
		reportSample.name = String(sample.sampleName.c_str());
		reportSample.timeMs = sample.activeTimeQuery->getTimeMs();
		reportSample.numDrawnSamples = sample.activeOcclusionQuery->getNumSamples();

		reportSample.numDrawCalls = (UINT32)(sample.endStats.numDrawCalls - sample.startStats.numDrawCalls);
		reportSample.numRenderTargetChanges = (UINT32)(sample.endStats.numRenderTargetChanges - sample.startStats.numRenderTargetChanges);
		reportSample.numPresents = (UINT32)(sample.endStats.numPresents - sample.startStats.numPresents);
		reportSample.numClears = (UINT32)(sample.endStats.numClears - sample.startStats.numClears);

		reportSample.numVertices = (UINT32)(sample.endStats.numVertices - sample.startStats.numVertices);
		reportSample.numPrimitives = (UINT32)(sample.endStats.numPrimitives - sample.startStats.numPrimitives);
		
		reportSample.numBlendStateChanges = (UINT32)(sample.endStats.numBlendStateChanges - sample.startStats.numBlendStateChanges);
		reportSample.numRasterizerStateChanges = (UINT32)(sample.endStats.numRasterizerStateChanges - sample.startStats.numRasterizerStateChanges);
		reportSample.numDepthStencilStateChanges = (UINT32)(sample.endStats.numDepthStencilStateChanges - sample.startStats.numDepthStencilStateChanges);

		reportSample.numTextureBinds = (UINT32)(sample.endStats.numTextureBinds - sample.startStats.numTextureBinds);
		reportSample.numSamplerBinds = (UINT32)(sample.endStats.numSamplerBinds - sample.startStats.numSamplerBinds);
		reportSample.numVertexBufferBinds = (UINT32)(sample.endStats.numVertexBufferBinds - sample.startStats.numVertexBufferBinds);
		reportSample.numIndexBufferBinds = (UINT32)(sample.endStats.numIndexBufferBinds - sample.startStats.numIndexBufferBinds);
		reportSample.numGpuParamBufferBinds = (UINT32)(sample.endStats.numGpuParamBufferBinds - sample.startStats.numGpuParamBufferBinds);
		reportSample.numGpuProgramBinds = (UINT32)(sample.endStats.numGpuProgramBinds - sample.startStats.numGpuProgramBinds);

		reportSample.numResourceWrites = (UINT32)(sample.endStats.numResourceWrites - sample.startStats.numResourceWrites);
		reportSample.numResourceReads = (UINT32)(sample.endStats.numResourceReads - sample.startStats.numResourceReads);

		reportSample.numObjectsCreated = (UINT32)(sample.endStats.numObjectsCreated - sample.startStats.numObjectsCreated);
		reportSample.numObjectsDestroyed = (UINT32)(sample.endStats.numObjectsDestroyed - sample.startStats.numObjectsDestroyed);

		mFreeTimerQueries.push(sample.activeTimeQuery);
		mFreeOcclusionQueries.push(sample.activeOcclusionQuery);
	}

	void GPUProfiler::beginSampleInternal(ActiveSample& sample)
	{
		sample.startStats = RenderSystem::instance().getRenderStats();
		sample.activeTimeQuery = getTimerQuery();
		sample.activeTimeQuery->begin();

		sample.activeOcclusionQuery = getOcclusionQuery();
		sample.activeOcclusionQuery->begin();
	}

	void GPUProfiler::endSampleInternal(ActiveSample& sample)
	{
		sample.endStats = RenderSystem::instance().getRenderStats();
		sample.activeOcclusionQuery->end();
		sample.activeTimeQuery->end();
	}

	TimerQueryPtr GPUProfiler::getTimerQuery() const
	{
		if (!mFreeTimerQueries.empty())
		{
			TimerQueryPtr timerQuery = mFreeTimerQueries.top();
			mFreeTimerQueries.pop();

			return timerQuery;
		}

		return TimerQuery::create();
	}

	OcclusionQueryPtr GPUProfiler::getOcclusionQuery() const
	{
		if (!mFreeOcclusionQueries.empty())
		{
			OcclusionQueryPtr occlusionQuery = mFreeOcclusionQueries.top();
			mFreeOcclusionQueries.pop();

			return occlusionQuery;
		}

		return OcclusionQuery::create(false);
	}
}