/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
// RenderSystem implementation
// Note that most of this class is abstract since
//  we cannot know how to implement the behaviour without
//  being aware of the 3D API. However there are a few
//  simple functions which can have a base implementation

#include "CmRenderSystem.h"

#include "CmViewport.h"
#include "CmException.h"
#include "CmRenderTarget.h"
#include "CmRenderWindow.h"
#include "CmPixelBuffer.h"
#include "CmOcclusionQuery.h"
#include "CmCommandQueue.h"
#include "CmDeferredRenderContext.h"
#include "boost/bind.hpp"

#if CM_DEBUG_MODE
#define THROW_IF_NOT_RENDER_THREAD throwIfNotRenderThread();
#else
#define THROW_IF_NOT_RENDER_THREAD 
#endif

namespace CamelotEngine {

    static const TexturePtr sNullTexPtr;

    //-----------------------------------------------------------------------
    RenderSystem::RenderSystem()
        : mActiveRenderTarget(0)
        , mCullingMode(CULL_CLOCKWISE)
		, mVsync(false)
		, mVSyncInterval(1)
        , mInvertVertexWinding(false)
        , mDisabledTexUnitsFrom(0)
        , mVertexProgramBound(false)
		, mGeometryProgramBound(false)
        , mFragmentProgramBound(false)
		, mClipPlanesDirty(true)
		, mCurrentCapabilities(nullptr)
		, mRenderThreadFunc(nullptr)
		, mRenderThreadShutdown(false)
		, mCommandQueue(nullptr)
		, mMaxCommandNotifyId(0)
    {
    }

    //-----------------------------------------------------------------------
    RenderSystem::~RenderSystem()
    {
		// Base classes need to call virtual destroy_internal method (queue it on render thread)

		// TODO - What if something gets queued between the queued call to destroy_internal and this!?
		shutdownRenderThread();

		if(mCommandQueue != nullptr)
		{
			delete mCommandQueue;
			mCommandQueue = nullptr;
		}

		delete mCurrentCapabilities;
		mCurrentCapabilities = 0;
    }
	//-----------------------------------------------------------------------
	void RenderSystem::initialize()
	{
		mRenderThreadId = CM_THREAD_CURRENT_ID;
		mCommandQueue = new CommandQueue(CM_THREAD_CURRENT_ID);

		initRenderThread();

		queueCommand(boost::bind(&RenderSystem::initialize_internal, this), true);
	}
	//-----------------------------------------------------------------------
	void RenderSystem::initialize_internal()
	{
		THROW_IF_NOT_RENDER_THREAD;

		mVertexProgramBound = false;
		mGeometryProgramBound = false;
		mFragmentProgramBound = false;
	}
	//-----------------------------------------------------------------------
	void RenderSystem::destroy_internal()
	{
		// Remove all the render targets.
		mRenderTargets.clear();

		mPrioritisedRenderTargets.clear();
	}
    //-----------------------------------------------------------------------
    void RenderSystem::swapAllRenderTargetBuffers(bool waitForVSync)
    {
		THROW_IF_NOT_RENDER_THREAD;

        // Update all in order of priority
        // This ensures render-to-texture targets get updated before render windows
		RenderTargetPriorityMap::iterator itarg, itargend;
		itargend = mPrioritisedRenderTargets.end();
		for( itarg = mPrioritisedRenderTargets.begin(); itarg != itargend; ++itarg )
		{
			if( itarg->second->isActive())
				itarg->second->swapBuffers(waitForVSync);
		}
    }
    //---------------------------------------------------------------------------------------------
    void RenderSystem::destroyRenderWindow(RenderWindow* renderWindow)
    {
		THROW_IF_NOT_RENDER_THREAD;

        destroyRenderTarget(renderWindow);
    }
    //---------------------------------------------------------------------------------------------
    void RenderSystem::destroyRenderTexture(RenderTexture* renderTexture)
    {
		THROW_IF_NOT_RENDER_THREAD;

        destroyRenderTarget(renderTexture);
    }
    //---------------------------------------------------------------------------------------------
    void RenderSystem::destroyRenderTarget(RenderTarget* renderTarget)
    {
		THROW_IF_NOT_RENDER_THREAD;

        detachRenderTarget(*renderTarget);
        delete renderTarget;
    }
    //---------------------------------------------------------------------------------------------
    void RenderSystem::attachRenderTarget( RenderTarget &target )
    {
		THROW_IF_NOT_RENDER_THREAD;

		assert( target.getPriority() < CM_NUM_RENDERTARGET_GROUPS );

        mRenderTargets.push_back(&target);
        mPrioritisedRenderTargets.insert(
            RenderTargetPriorityMap::value_type(target.getPriority(), &target ));
    }
	//---------------------------------------------------------------------------------------------
	void RenderSystem::detachRenderTarget(RenderTarget& renderTarget)
	{
		THROW_IF_NOT_RENDER_THREAD;

		auto it = std::find(mRenderTargets.begin(), mRenderTargets.end(), &renderTarget);
		RenderTarget* foundRT = nullptr;

		if( it != mRenderTargets.end() )
		{
			foundRT = *it;

			/* Remove the render target from the priority groups. */
			RenderTargetPriorityMap::iterator itarg, itargend;
			itargend = mPrioritisedRenderTargets.end();
			for( itarg = mPrioritisedRenderTargets.begin(); itarg != itargend; ++itarg )
			{
				if( itarg->second == *it ) {
					mPrioritisedRenderTargets.erase( itarg );
					break;
				}
			}

			mRenderTargets.erase( it );
		}
		/// If detached render target is the active render target, reset active render target
		if(foundRT == mActiveRenderTarget)
			mActiveRenderTarget = 0;
	}
	//---------------------------------------------------------------------------------------------
	const RenderSystemCapabilities* RenderSystem::getCapabilities(void) const 
	{ 
		THROW_IF_NOT_RENDER_THREAD;

		return mCurrentCapabilities; 
	}
	//---------------------------------------------------------------------------------------------
	const DriverVersion& RenderSystem::getDriverVersion(void) const 
	{ 
		THROW_IF_NOT_RENDER_THREAD;

		return mDriverVersion; 
	}
    //-----------------------------------------------------------------------
    Viewport RenderSystem::getViewport(void)
    {
		THROW_IF_NOT_RENDER_THREAD;

        return mActiveViewport;
    }
    //-----------------------------------------------------------------------
    void RenderSystem::disableTextureUnit(GpuProgramType gptype, UINT16 texUnit)
    {
		THROW_IF_NOT_RENDER_THREAD;

        setTexture(gptype, texUnit, false, sNullTexPtr);
    }
	//-----------------------------------------------------------------------
	bool RenderSystem::getWaitForVerticalBlank(void) const
	{
		THROW_IF_NOT_RENDER_THREAD;

		return mVsync;
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setWaitForVerticalBlank(bool enabled)
	{
		THROW_IF_NOT_RENDER_THREAD;

		mVsync = enabled;
	}
	//---------------------------------------------------------------------
	void RenderSystem::addClipPlane (const Plane &p)
	{
		THROW_IF_NOT_RENDER_THREAD;

		mClipPlanes.push_back(p);
		mClipPlanesDirty = true;
	}
	//---------------------------------------------------------------------
	void RenderSystem::addClipPlane (float A, float B, float C, float D)
	{
		THROW_IF_NOT_RENDER_THREAD;

		addClipPlane(Plane(A, B, C, D));
	}
	//---------------------------------------------------------------------
	void RenderSystem::setClipPlanes(const PlaneList& clipPlanes)
	{
		THROW_IF_NOT_RENDER_THREAD;

		if (clipPlanes != mClipPlanes)
		{
			mClipPlanes = clipPlanes;
			mClipPlanesDirty = true;
		}
	}
	//---------------------------------------------------------------------
	void RenderSystem::resetClipPlanes()
	{
		THROW_IF_NOT_RENDER_THREAD;

		if (!mClipPlanes.empty())
		{
			mClipPlanes.clear();
			mClipPlanesDirty = true;
		}
	}
	//-----------------------------------------------------------------------
	void RenderSystem::bindGpuProgram(GpuProgramHandle prg)
	{
		THROW_IF_NOT_RENDER_THREAD;

		switch(prg->getBindingDelegate()->getType())
		{
		case GPT_VERTEX_PROGRAM:
			// mark clip planes dirty if changed (programmable can change space)
			if (!mVertexProgramBound && !mClipPlanes.empty())
				mClipPlanesDirty = true;

			mVertexProgramBound = true;
			break;
		case GPT_GEOMETRY_PROGRAM:
			mGeometryProgramBound = true;
			break;
		case GPT_FRAGMENT_PROGRAM:
			mFragmentProgramBound = true;
			break;
		}
	}
	//-----------------------------------------------------------------------
	void RenderSystem::unbindGpuProgram(GpuProgramType gptype)
	{
		THROW_IF_NOT_RENDER_THREAD;

		switch(gptype)
		{
		case GPT_VERTEX_PROGRAM:
			// mark clip planes dirty if changed (programmable can change space)
			if (mVertexProgramBound && !mClipPlanes.empty())
				mClipPlanesDirty = true;
			mVertexProgramBound = false;
			break;
		case GPT_GEOMETRY_PROGRAM:
			mGeometryProgramBound = false;
			break;
		case GPT_FRAGMENT_PROGRAM:
			mFragmentProgramBound = false;
			break;
		}
	}
	//-----------------------------------------------------------------------
	bool RenderSystem::isGpuProgramBound(GpuProgramType gptype)
	{
		THROW_IF_NOT_RENDER_THREAD;

	    switch(gptype)
	    {
        case GPT_VERTEX_PROGRAM:
            return mVertexProgramBound;
        case GPT_GEOMETRY_PROGRAM:
            return mGeometryProgramBound;
        case GPT_FRAGMENT_PROGRAM:
            return mFragmentProgramBound;
	    }
        // Make compiler happy
        return false;
	}
	//-----------------------------------------------------------------------
	void RenderSystem::render(const RenderOperation& op)
	{
		THROW_IF_NOT_RENDER_THREAD;

		// sort out clip planes
		// have to do it here in case of matrix issues
		if (mClipPlanesDirty)
		{
			setClipPlanesImpl(mClipPlanes);
			mClipPlanesDirty = false;
		}

		setVertexDeclaration(op.vertexData->vertexDeclaration);
		auto vertexBuffers = op.vertexData->getBuffers();

		for(auto iter = vertexBuffers.begin(); iter != vertexBuffers.end() ; ++iter)
			setVertexBuffer(iter->first, iter->second);

		setDrawOperation(op.operationType);

		if (op.useIndexes)
		{
			setIndexBuffer(op.indexData->indexBuffer);
			drawIndexed(op.indexData->indexStart, op.indexData->indexCount, op.vertexData->vertexCount);
		}
		else
			draw(op.vertexData->vertexCount);
	}

	/************************************************************************/
	/* 								PRIVATE		                     		*/
	/************************************************************************/

	void RenderSystem::initRenderThread()
	{
#if !CM_FORCE_SINGLETHREADED_RENDERING
		mRenderThreadFunc = new RenderWorkerFunc(this);

#if CM_THREAD_SUPPORT
		CM_THREAD_CREATE(t, *mRenderThreadFunc);
		mRenderThread = t;

		CM_LOCK_MUTEX_NAMED(mRenderThreadStartMutex, lock)
		CM_THREAD_WAIT(mRenderThreadStartCondition, mRenderThreadStartMutex, lock)

#else
		CM_EXCEPT(InternalErrorException, "Attempting to start a render thread but Camelot isn't compiled with thread support.");
#endif
#endif
	}

	void RenderSystem::runRenderThread()
	{
#if !CM_FORCE_SINGLETHREADED_RENDERING
		mRenderThreadId = CM_THREAD_CURRENT_ID;

		CM_THREAD_NOTIFY_ALL(mRenderThreadStartCondition)

		while(true)
		{
			// Wait until we get some ready commands
			vector<CommandQueue::Command>::type* commands = nullptr;
			{
				CM_LOCK_MUTEX_NAMED(mCommandQueueMutex, lock)

				while(mCommandQueue->isEmpty())
				{
					if(mRenderThreadShutdown)
						return;

					CM_THREAD_WAIT(mCommandReadyCondition, mCommandQueueMutex, lock);
				}

				commands = mCommandQueue->flush();
			}

			// Play commands
			mCommandQueue->playback(commands, boost::bind(&RenderSystem::commandCompletedNotify, this, _1)); 
		}
#endif
	}

	void RenderSystem::shutdownRenderThread()
	{
#if !CM_FORCE_SINGLETHREADED_RENDERING

		{
			CM_LOCK_MUTEX(mCommandQueueMutex);
			mRenderThreadShutdown = true;
		}

		// Wake all threads. They will quit after they see the shutdown flag
		CM_THREAD_NOTIFY_ALL(mCommandReadyCondition);

		mRenderThread->join();
		CM_THREAD_DESTROY(mRenderThread);

		mRenderThread = nullptr;
		mRenderThreadId = CM_THREAD_CURRENT_ID;
#endif
	}

	DeferredRenderContextPtr RenderSystem::createDeferredContext()
	{
		return DeferredRenderContextPtr(new DeferredRenderContext(this, CM_THREAD_CURRENT_ID));
	}
	
	AsyncOp RenderSystem::queueReturnCommand(boost::function<void(AsyncOp&)> commandCallback, bool blockUntilComplete)
	{
		AsyncOp op;

		if(CM_THREAD_CURRENT_ID == getRenderThreadId())
		{
			commandCallback(op); // Execute immediately
			return op;
		}

		UINT32 commandId = -1;
		{
			CM_LOCK_MUTEX(mCommandQueueMutex);

			if(blockUntilComplete)
			{
				commandId = mMaxCommandNotifyId++;
				op = mCommandQueue->queueReturn(commandCallback, true, commandId);
			}
			else
				op = mCommandQueue->queueReturn(commandCallback);
		}

		CM_THREAD_NOTIFY_ALL(mCommandReadyCondition);

		if(blockUntilComplete)
			blockUntilCommandCompleted(commandId);

		return op;
	}

	void RenderSystem::queueCommand(boost::function<void()> commandCallback, bool blockUntilComplete)
	{
		if(CM_THREAD_CURRENT_ID == getRenderThreadId())
		{
			commandCallback(); // Execute immediately
			return;
		}

		UINT32 commandId = -1;
		{
			CM_LOCK_MUTEX(mCommandQueueMutex);

			if(blockUntilComplete)
			{
				commandId = mMaxCommandNotifyId++;
				mCommandQueue->queue(commandCallback, true, commandId);
			}
			else
				mCommandQueue->queue(commandCallback);
		}

		CM_THREAD_NOTIFY_ALL(mCommandReadyCondition);

		if(blockUntilComplete)
			blockUntilCommandCompleted(commandId);
	}

	void RenderSystem::blockUntilCommandCompleted(UINT32 commandId)
	{
#if !CM_FORCE_SINGLETHREADED_RENDERING
		CM_LOCK_MUTEX_NAMED(mCommandNotifyMutex, lock);

		while(true)
		{
			// Check if our command id is in the completed list
			auto iter = mCommandsCompleted.begin();
			for(; iter != mCommandsCompleted.end(); ++iter)
			{
				if(*iter == commandId)
					break;
			}

			if(iter != mCommandsCompleted.end())
			{
				mCommandsCompleted.erase(iter);
				break;
			}

			CM_THREAD_WAIT(mCommandCompleteCondition, mCommandNotifyMutex, lock);
		}
#endif
	}

	void RenderSystem::commandCompletedNotify(UINT32 commandId)
	{
		{
			CM_LOCK_MUTEX(mCommandNotifyMutex);

			mCommandsCompleted.push_back(commandId);
		}

		CM_THREAD_NOTIFY_ALL(mCommandCompleteCondition);
	}

	void RenderSystem::throwIfNotRenderThread() const
	{
#if !CM_FORCE_SINGLETHREADED_RENDERING
		if(CM_THREAD_CURRENT_ID != getRenderThreadId())
			CM_EXCEPT(InternalErrorException, "Calling the render system from a non-render thread!");
#endif
	}

	/************************************************************************/
	/* 								THREAD WORKER                      		*/
	/************************************************************************/

	RenderSystem::RenderWorkerFunc::RenderWorkerFunc(RenderSystem* rs)
		:mRS(rs)
	{
		assert(mRS != nullptr);
	}

	void RenderSystem::RenderWorkerFunc::operator()()
	{
		mRS->runRenderThread();
	}
}

#undef THROW_IF_NOT_RENDER_THREAD