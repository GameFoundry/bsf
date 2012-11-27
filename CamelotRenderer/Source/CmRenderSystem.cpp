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
#include "CmHardwarePixelBuffer.h"
#include "CmHardwareOcclusionQuery.h"
#include "CmRenderSystemContext.h"
#include "boost/bind.hpp"

namespace CamelotEngine {

    static const TexturePtr sNullTexPtr;

    //-----------------------------------------------------------------------
    RenderSystem::RenderSystem()
        : mActiveRenderTarget(0)
        // This means CULL clockwise vertices, i.e. front of poly is counter-clockwise
        // This makes it the same as OpenGL and other right-handed systems
        , mCullingMode(CULL_CLOCKWISE)
        , mVSync(true)
		, mVSyncInterval(1)
        , mInvertVertexWinding(false)
        , mDisabledTexUnitsFrom(0)
        , mVertexProgramBound(false)
		, mGeometryProgramBound(false)
        , mFragmentProgramBound(false)
		, mClipPlanesDirty(true)
		, mRealCapabilities(0)
		, mCurrentCapabilities(0)
		, mUseCustomCapabilities(false)
		, mRenderThreadFunc(nullptr)
		, mRenderThreadShutdown(false)
    {
    }

    //-----------------------------------------------------------------------
    RenderSystem::~RenderSystem()
    {
        shutdown();
		delete mRealCapabilities;
		mRealCapabilities = 0;
		// Current capabilities managed externally
		mCurrentCapabilities = 0;
    }
    //-----------------------------------------------------------------------
    void RenderSystem::swapAllRenderTargetBuffers(bool waitForVSync)
    {
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
    //-----------------------------------------------------------------------
    void RenderSystem::startUp()
    {
		mRenderThreadId = CM_THREAD_CURRENT_ID;
		mResourceContext = 	createRenderSystemContext();

		initRenderThread(); // TODO - Move render thread to the outside of the RS

		{
			CM_LOCK_MUTEX(mResourceContextMutex)
			mResourceContext->queueCommand(boost::bind(&RenderSystem::startUp_internal, this, _1));
		}
    }
	//---------------------------------------------------------------------------------------------
	RenderWindow* RenderSystem::createRenderWindow(const String &name, unsigned int width, unsigned int height, 
		bool fullScreen, const NameValuePairList *miscParams)
	{
		AsyncOp op;
		{
			CM_LOCK_MUTEX(mResourceContextMutex)

			if(miscParams != nullptr)
				op = mResourceContext->queueCommand(boost::bind(&RenderSystem::createRenderWindow_internal, this, name, width, height, fullScreen, *miscParams, _1));
			else
				op = mResourceContext->queueCommand(boost::bind(&RenderSystem::createRenderWindow_internal, this, name, width, height, fullScreen, NameValuePairList(), _1));
		}

		submitToGpu(mResourceContext, true);
		return op.getReturnValue<RenderWindow*>();
	}
	//---------------------------------------------------------------------------------------------
	void RenderSystem::useCustomRenderSystemCapabilities(RenderSystemCapabilities* capabilities)
	{
		if (mRealCapabilities != 0)
		{
			CM_EXCEPT(InternalErrorException, 
				"Custom render capabilities must be set before the RenderSystem is initialised.");
		}

		mCurrentCapabilities = capabilities;
		mUseCustomCapabilities = true;
	}
    //---------------------------------------------------------------------------------------------
    void RenderSystem::destroyRenderWindow(const String& name)
    {
        destroyRenderTarget(name);
    }
    //---------------------------------------------------------------------------------------------
    void RenderSystem::destroyRenderTexture(const String& name)
    {
        destroyRenderTarget(name);
    }
    //---------------------------------------------------------------------------------------------
    void RenderSystem::destroyRenderTarget(const String& name)
    {
        RenderTarget* rt = detachRenderTarget(name);
        delete rt;
    }
    //---------------------------------------------------------------------------------------------
    void RenderSystem::attachRenderTarget( RenderTarget &target )
    {
		assert( target.getPriority() < OGRE_NUM_RENDERTARGET_GROUPS );

        mRenderTargets.insert( RenderTargetMap::value_type( target.getName(), &target ) );
        mPrioritisedRenderTargets.insert(
            RenderTargetPriorityMap::value_type(target.getPriority(), &target ));
    }

    //---------------------------------------------------------------------------------------------
    RenderTarget * RenderSystem::getRenderTarget( const String &name )
    {
        RenderTargetMap::iterator it = mRenderTargets.find( name );
        RenderTarget *ret = NULL;

        if( it != mRenderTargets.end() )
        {
            ret = it->second;
        }

        return ret;
    }

    //---------------------------------------------------------------------------------------------
    RenderTarget * RenderSystem::detachRenderTarget( const String &name )
    {
        RenderTargetMap::iterator it = mRenderTargets.find( name );
        RenderTarget *ret = NULL;

        if( it != mRenderTargets.end() )
        {
            ret = it->second;
			
			/* Remove the render target from the priority groups. */
            RenderTargetPriorityMap::iterator itarg, itargend;
            itargend = mPrioritisedRenderTargets.end();
			for( itarg = mPrioritisedRenderTargets.begin(); itarg != itargend; ++itarg )
            {
				if( itarg->second == ret ) {
					mPrioritisedRenderTargets.erase( itarg );
					break;
				}
            }

            mRenderTargets.erase( it );
        }
        /// If detached render target is the active render target, reset active render target
        if(ret == mActiveRenderTarget)
            mActiveRenderTarget = 0;

        return ret;
    }
    //-----------------------------------------------------------------------
    Viewport RenderSystem::getViewport(void)
    {
        return mActiveViewport;
    }
    //-----------------------------------------------------------------------
    void RenderSystem::setTextureUnitSettings(size_t texUnit, const TexturePtr& tex, const SamplerState& tl)
    {
        // This method is only ever called to set a texture unit to valid details
        // The method _disableTextureUnit is called to turn a unit off

		// Vertex texture binding?
		if (mCurrentCapabilities->hasCapability(RSC_VERTEX_TEXTURE_FETCH) &&
			!mCurrentCapabilities->getVertexTextureUnitsShared())
		{
			if (tl.getBindingType() == SamplerState::BT_VERTEX)
			{
				// Bind vertex texture
				setVertexTexture(texUnit, tex);
				// bind nothing to fragment unit (hardware isn't shared but fragment
				// unit can't be using the same index
				setTexture(texUnit, true, sNullTexPtr);
			}
			else
			{
				// vice versa
				setVertexTexture(texUnit, sNullTexPtr);
				setTexture(texUnit, true, tex);
			}
		}
		else
		{
			// Shared vertex / fragment textures or no vertex texture support
			// Bind texture (may be blank)
			setTexture(texUnit, true, tex);
		}

        // Set texture layer filtering
        setTextureFiltering(texUnit, 
            tl.getTextureFiltering(FT_MIN), 
            tl.getTextureFiltering(FT_MAG), 
            tl.getTextureFiltering(FT_MIP));

        // Set texture layer filtering
        setTextureAnisotropy(texUnit, tl.getTextureAnisotropy());

		// Set mipmap biasing
		setTextureMipmapBias(texUnit, tl.getTextureMipmapBias());

        // Texture addressing mode
        const SamplerState::UVWAddressingMode& uvw = tl.getTextureAddressingMode();
        setTextureAddressingMode(texUnit, uvw);
    }
	//-----------------------------------------------------------------------
	void RenderSystem::setVertexTexture(size_t unit, const TexturePtr& tex)
	{
		CM_EXCEPT(NotImplementedException, 
			"This rendersystem does not support separate vertex texture samplers, "
			"you should use the regular texture samplers which are shared between "
			"the vertex and fragment units.");
	}
    //-----------------------------------------------------------------------
    void RenderSystem::disableTextureUnit(size_t texUnit)
    {
        setTexture(texUnit, false, sNullTexPtr);
    }
    //---------------------------------------------------------------------
    void RenderSystem::disableTextureUnitsFrom(size_t texUnit)
    {
        size_t disableTo = CM_MAX_TEXTURE_LAYERS;
        if (disableTo > mDisabledTexUnitsFrom)
            disableTo = mDisabledTexUnitsFrom;
        mDisabledTexUnitsFrom = texUnit;
        for (size_t i = texUnit; i < disableTo; ++i)
        {
            disableTextureUnit(i);
        }
    }
    //-----------------------------------------------------------------------
    void RenderSystem::setTextureFiltering(size_t unit, FilterOptions minFilter,
            FilterOptions magFilter, FilterOptions mipFilter)
    {
        setTextureFiltering(unit, FT_MIN, minFilter);
        setTextureFiltering(unit, FT_MAG, magFilter);
        setTextureFiltering(unit, FT_MIP, mipFilter);
    }
    //-----------------------------------------------------------------------
    CullingMode RenderSystem::getCullingMode(void) const
    {
        return mCullingMode;
    }
    //-----------------------------------------------------------------------
    bool RenderSystem::getWaitForVerticalBlank(void) const
    {
        return mVSync;
    }
    //-----------------------------------------------------------------------
    void RenderSystem::setWaitForVerticalBlank(bool enabled)
    {
        mVSync = enabled;
    }
    //-----------------------------------------------------------------------
    void RenderSystem::shutdown(void)
    {
        // Remove all the render targets.
		// (destroy primary target last since others may depend on it)
		RenderTarget* primary = 0;
		for (RenderTargetMap::iterator it = mRenderTargets.begin(); it != mRenderTargets.end(); ++it)
		{
			if (!primary && it->second->isPrimary())
				primary = it->second;
			else
				delete it->second;
		}
		delete primary;
		mRenderTargets.clear();

		mPrioritisedRenderTargets.clear();

		shutdownRenderThread();
    }
    //-----------------------------------------------------------------------
    void RenderSystem::beginGeometryCount(void)
    {
        mBatchCount = mFaceCount = mVertexCount = 0;

    }
    //-----------------------------------------------------------------------
    unsigned int RenderSystem::getFaceCount(void) const
    {
        return static_cast< unsigned int >( mFaceCount );
    }
    //-----------------------------------------------------------------------
    unsigned int RenderSystem::getBatchCount(void) const
    {
        return static_cast< unsigned int >( mBatchCount );
    }
    //-----------------------------------------------------------------------
    unsigned int RenderSystem::getVertexCount(void) const
    {
        return static_cast< unsigned int >( mVertexCount );
    }
    //-----------------------------------------------------------------------
	void RenderSystem::convertColorValue(const Color& colour, UINT32* pDest)
	{
		*pDest = VertexElement::convertColourValue(colour, getColorVertexElementType());

	}
    //-----------------------------------------------------------------------
    void RenderSystem::render(const RenderOperation& op)
    {
        // Update stats
        size_t val;

        if (op.useIndexes)
            val = op.indexData->indexCount;
        else
            val = op.vertexData->vertexCount;

        switch(op.operationType)
        {
		case RenderOperation::OT_TRIANGLE_LIST:
            mFaceCount += val / 3;
            break;
        case RenderOperation::OT_TRIANGLE_STRIP:
        case RenderOperation::OT_TRIANGLE_FAN:
            mFaceCount += val - 2;
            break;
	    case RenderOperation::OT_POINT_LIST:
	    case RenderOperation::OT_LINE_LIST:
	    case RenderOperation::OT_LINE_STRIP:
	        break;
	    }

        mVertexCount += op.vertexData->vertexCount;
        mBatchCount += 1;

		// sort out clip planes
		// have to do it here in case of matrix issues
		if (mClipPlanesDirty)
		{
			setClipPlanesImpl(mClipPlanes);
			mClipPlanesDirty = false;
		}
    }
    //-----------------------------------------------------------------------
    void RenderSystem::setInvertVertexWinding(bool invert)
    {
        mInvertVertexWinding = invert;
    }
	//-----------------------------------------------------------------------
	bool RenderSystem::getInvertVertexWinding(void) const
	{
		return mInvertVertexWinding;
	}
	//---------------------------------------------------------------------
	void RenderSystem::addClipPlane (const Plane &p)
	{
		mClipPlanes.push_back(p);
		mClipPlanesDirty = true;
	}
	//---------------------------------------------------------------------
	void RenderSystem::addClipPlane (float A, float B, float C, float D)
	{
		addClipPlane(Plane(A, B, C, D));
	}
	//---------------------------------------------------------------------
	void RenderSystem::setClipPlanes(const PlaneList& clipPlanes)
	{
		if (clipPlanes != mClipPlanes)
		{
			mClipPlanes = clipPlanes;
			mClipPlanesDirty = true;
		}
	}
	//---------------------------------------------------------------------
	void RenderSystem::resetClipPlanes()
	{
		if (!mClipPlanes.empty())
		{
			mClipPlanes.clear();
			mClipPlanesDirty = true;
		}
	}
	//-----------------------------------------------------------------------
	void RenderSystem::bindGpuProgram(GpuProgramRef prg)
	{
	    switch(prg->_getBindingDelegate()->getType())
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

	void RenderSystem::initRenderThread()
	{
		mRenderThreadFunc = new RenderWorkerFunc(this);

#if CM_THREAD_SUPPORT
		CM_THREAD_CREATE(t, *mRenderThreadFunc);
		mRenderThread = t;

		CM_LOCK_MUTEX_NAMED(mRSContextInitMutex, lock)
		CM_THREAD_WAIT(mRSContextInitCondition, mRSContextInitMutex, lock)

#else
		CM_EXCEPT(InternalErrorException, "Attempting to start a render thread but Camelot isn't compiled with thread support.");
#endif
	}

	void RenderSystem::runRenderThread()
	{
		mRenderThreadId = CM_THREAD_CURRENT_ID;

		CM_THREAD_NOTIFY_ALL(mRSContextInitCondition)

		while(true)
		{
			if(mRenderThreadShutdown)
				return;

			vector<RenderSystemContextPtr>::type renderSystemContextsCopy;
			{
				CM_LOCK_MUTEX(mRSContextMutex);

				for(auto iter = mRenderSystemContexts.begin(); iter != mRenderSystemContexts.end(); ++iter)
				{
					renderSystemContextsCopy.push_back(*iter);
				}
			}


			// Wait until we get some ready commands
			{
				CM_LOCK_MUTEX_NAMED(mRSContextMutex, lock)

				bool anyCommandsReady = false;
				for(auto iter = renderSystemContextsCopy.begin(); iter != renderSystemContextsCopy.end(); ++iter)
				{
					if((*iter)->hasReadyCommands())
					{
						anyCommandsReady = true;
						break;
					}
				}

				if(!anyCommandsReady)
					CM_THREAD_WAIT(mRSContextReadyCondition, mRSContextMutex, lock)
			}

			{
				CM_LOCK_MUTEX(mRSRenderCallbackMutex)

				if(!PreRenderThreadUpdateCallback.empty())
					PreRenderThreadUpdateCallback();
			}

			// Play commands
			for(auto iter = renderSystemContextsCopy.begin(); iter != renderSystemContextsCopy.end(); ++iter)
			{
				(*iter)->playbackCommands();
			}

			{
				CM_LOCK_MUTEX(mRSRenderCallbackMutex)

				if(!PostRenderThreadUpdateCallback.empty())
					PostRenderThreadUpdateCallback();
			}
		}

	}

	void RenderSystem::shutdownRenderThread()
	{
		mRenderThreadShutdown = true;

		// Wake all threads. They will quit after they see the shutdown flag
		CM_THREAD_NOTIFY_ALL(mRSContextReadyCondition)

		mRenderThread->join();
		CM_THREAD_DESTROY(mRenderThread);

		mRenderThread = nullptr;
		mRenderThreadId = CM_THREAD_CURRENT_ID;
		mRenderSystemContexts.clear();
	}

	RenderSystemContextPtr RenderSystem::createRenderSystemContext()
	{
		RenderSystemContextPtr newContext = RenderSystemContextPtr(
			new RenderSystemContext(CM_THREAD_CURRENT_ID)
			);

		{
			CM_LOCK_MUTEX(mRSContextMutex);
			mRenderSystemContexts.push_back(newContext);
		}

		return newContext;
	}

	void RenderSystem::addPreRenderThreadUpdateCallback(boost::function<void()> callback)
	{
		CM_LOCK_MUTEX(mRSRenderCallbackMutex)

		PreRenderThreadUpdateCallback.connect(callback);
	}

	void RenderSystem::addPostRenderThreadUpdateCallback(boost::function<void()> callback)
	{
		CM_LOCK_MUTEX(mRSRenderCallbackMutex)

		PostRenderThreadUpdateCallback.connect(callback);
	}

	void RenderSystem::submitToGpu(RenderSystemContextPtr context, bool blockUntilComplete)
	{
		{
			CM_LOCK_MUTEX(mRSContextMutex);

			context->submitToGpu();
		}

		CM_THREAD_NOTIFY_ALL(mRSContextReadyCondition);

		if(blockUntilComplete)
			context->blockUntilExecuted();
	}

	void RenderSystem::update()
	{
		{
			CM_LOCK_MUTEX(mRSContextMutex);

			for(auto iter = mRenderSystemContexts.begin(); iter != mRenderSystemContexts.end(); ++iter)
			{
				(*iter)->submitToGpu();
			}
		}

		CM_THREAD_NOTIFY_ALL(mRSContextReadyCondition)
	}

	void RenderSystem::throwIfInvalidThread()
	{
		if(CM_THREAD_CURRENT_ID != getRenderThreadId())
			CM_EXCEPT(InternalErrorException, "Calling the render system from a non-render thread!");
	}

	/************************************************************************/
	/* 							INTERNAL CALLBACKS                     		*/
	/************************************************************************/

	void RenderSystem::startUp_internal(AsyncOp& asyncOp)
	{
		mVertexProgramBound = false;
		mGeometryProgramBound = false;
		mFragmentProgramBound = false;

		asyncOp.completeOperation();
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

