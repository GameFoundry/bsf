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
#include "CmCommandQueue.h"
#include "CmDeferredRenderContext.h"
#include "boost/bind.hpp"

#if CM_DEBUG_MODE
#define THROW_IF_INVALID_CONTEXT throwIfInvalidContextThread();
#define THROW_IF_NOT_RENDER_THREAD throwIfNotRenderThread();
#else
#define THROW_IF_INVALID_CONTEXT 
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
        shutdown_internal();

		delete mCurrentCapabilities;
		mCurrentCapabilities = 0;
    }
	//-----------------------------------------------------------------------
	void RenderSystem::startUp()
	{
		mRenderThreadId = CM_THREAD_CURRENT_ID;
		mCommandQueue = new CommandQueue(CM_THREAD_CURRENT_ID);

		initRenderThread();

		queueCommand(boost::bind(&RenderSystem::startUp_internal, this));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::startUp_internal()
	{
		THROW_IF_NOT_RENDER_THREAD;

		mVertexProgramBound = false;
		mGeometryProgramBound = false;
		mFragmentProgramBound = false;
	}
	//-----------------------------------------------------------------------
	void RenderSystem::shutdown(void)
	{
		queueCommand(boost::bind(&RenderSystem::shutdown_internal, this), true);
		// TODO - What if something gets queued between these two calls?
		shutdownRenderThread();
	}
	//-----------------------------------------------------------------------
	void RenderSystem::shutdown_internal(void)
	{
		// TODO - I should probably sync this up to make sure no other threads are doing anything while shutdown is in progress

		// Remove all the render targets.
		// (destroy primary target last since others may depend on it)
		RenderTarget* primary = 0;
		for (auto it = mRenderTargets.begin(); it != mRenderTargets.end(); ++it)
		{
			if (!primary && (*it)->isPrimary())
				primary = *it;
			else
				delete *it;
		}
		delete primary;
		mRenderTargets.clear();

		mPrioritisedRenderTargets.clear();

		if(mCommandQueue != nullptr)
		{
			delete mCommandQueue;
			mCommandQueue = nullptr;
		}
	}
	//-----------------------------------------------------------------------
	void RenderSystem::swapAllRenderTargetBuffers(bool waitForVSync)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::swapAllRenderTargetBuffers_internal, this, waitForVSync));
	}
    //-----------------------------------------------------------------------
    void RenderSystem::swapAllRenderTargetBuffers_internal(bool waitForVSync)
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
	RenderWindow* RenderSystem::createRenderWindow(const String &name, unsigned int width, unsigned int height, 
		bool fullScreen, const NameValuePairList *miscParams)
	{
		AsyncOp op;

		if(miscParams != nullptr)
			op = queueReturnCommand(boost::bind(&RenderSystem::createRenderWindow_internal, this, name, width, height, fullScreen, *miscParams, _1), true);
		else
			op = queueReturnCommand(boost::bind(&RenderSystem::createRenderWindow_internal, this, name, width, height, fullScreen, NameValuePairList(), _1), true);

		return op.getReturnValue<RenderWindow*>();
	}
    //---------------------------------------------------------------------------------------------
    void RenderSystem::destroyRenderWindow_internal(RenderWindow* renderWindow)
    {
		THROW_IF_NOT_RENDER_THREAD;

        destroyRenderTarget_internal(renderWindow);
    }
    //---------------------------------------------------------------------------------------------
    void RenderSystem::destroyRenderTexture_internal(RenderTexture* renderTexture)
    {
		THROW_IF_NOT_RENDER_THREAD;

        destroyRenderTarget_internal(renderTexture);
    }
    //---------------------------------------------------------------------------------------------
    void RenderSystem::destroyRenderTarget_internal(RenderTarget* renderTarget)
    {
		THROW_IF_NOT_RENDER_THREAD;

        detachRenderTarget_internal(*renderTarget);
        delete renderTarget;
    }
    //---------------------------------------------------------------------------------------------
    void RenderSystem::attachRenderTarget_internal( RenderTarget &target )
    {
		THROW_IF_NOT_RENDER_THREAD;

		assert( target.getPriority() < CM_NUM_RENDERTARGET_GROUPS );

        mRenderTargets.push_back(&target);
        mPrioritisedRenderTargets.insert(
            RenderTargetPriorityMap::value_type(target.getPriority(), &target ));
    }
	//---------------------------------------------------------------------------------------------
	void RenderSystem::detachRenderTarget_internal(RenderTarget& renderTarget)
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
	const RenderSystemCapabilities* RenderSystem::getCapabilities_internal(void) const 
	{ 
		THROW_IF_NOT_RENDER_THREAD;

		return mCurrentCapabilities; 
	}
	//---------------------------------------------------------------------------------------------
	const DriverVersion& RenderSystem::getDriverVersion_internal(void) const 
	{ 
		THROW_IF_NOT_RENDER_THREAD;

		return mDriverVersion; 
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setViewport(const Viewport& vp)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setViewport_internal, this, vp));
	}
    //-----------------------------------------------------------------------
    Viewport RenderSystem::getViewport_internal(void)
    {
		THROW_IF_NOT_RENDER_THREAD;

        return mActiveViewport;
    }
    //-----------------------------------------------------------------------
    void RenderSystem::setTextureUnitSettings(UINT16 texUnit, const TexturePtr& tex, const SamplerState& samplerState)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setTextureUnitSettings_internal, this, texUnit, tex, samplerState));
	}
	//-----------------------------------------------------------------------
    void RenderSystem::setTextureUnitSettings_internal(UINT16 texUnit, const TexturePtr& tex, const SamplerState& tl)
    {
		THROW_IF_NOT_RENDER_THREAD;

        // This method is only ever called to set a texture unit to valid details
        // The method _disableTextureUnit is called to turn a unit off

		// Vertex texture binding?
		if (mCurrentCapabilities->hasCapability(RSC_VERTEX_TEXTURE_FETCH) &&
			!mCurrentCapabilities->getVertexTextureUnitsShared())
		{
			if (tl.getBindingType() == SamplerState::BT_VERTEX)
			{
				// Bind vertex texture
				setVertexTexture_internal(texUnit, tex);
				// bind nothing to fragment unit (hardware isn't shared but fragment
				// unit can't be using the same index
				setTexture_internal(texUnit, true, sNullTexPtr);
			}
			else
			{
				// vice versa
				setVertexTexture_internal(texUnit, sNullTexPtr);
				setTexture_internal(texUnit, true, tex);
			}
		}
		else
		{
			// Shared vertex / fragment textures or no vertex texture support
			// Bind texture (may be blank)
			setTexture_internal(texUnit, true, tex);
		}

        // Set texture layer filtering
        setTextureFiltering_internal(texUnit, 
            tl.getTextureFiltering(FT_MIN), 
            tl.getTextureFiltering(FT_MAG), 
            tl.getTextureFiltering(FT_MIP));

        // Set texture layer filtering
        setTextureAnisotropy_internal(texUnit, tl.getTextureAnisotropy());

		// Set mipmap biasing
		setTextureMipmapBias_internal(texUnit, tl.getTextureMipmapBias());

        // Texture addressing mode
        const SamplerState::UVWAddressingMode& uvw = tl.getTextureAddressingMode();
        setTextureAddressingMode_internal(texUnit, uvw);
    }
	//-----------------------------------------------------------------------
	void RenderSystem::setTexture(UINT16 unit, bool enabled, const TexturePtr &texPtr)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setTexture_internal, this, unit, enabled, texPtr));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setVertexTexture(UINT16 unit, const TexturePtr& tex)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setVertexTexture_internal, this, unit, tex));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setVertexTexture_internal(UINT16 unit, const TexturePtr& tex)
	{
		THROW_IF_NOT_RENDER_THREAD;

		CM_EXCEPT(NotImplementedException, 
			"This rendersystem does not support separate vertex texture samplers, "
			"you should use the regular texture samplers which are shared between "
			"the vertex and fragment units.");
	}
	//-----------------------------------------------------------------------
	void RenderSystem::disableTextureUnit(UINT16 texUnit)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::disableTextureUnit_internal, this, texUnit));
	}
    //-----------------------------------------------------------------------
    void RenderSystem::disableTextureUnit_internal(UINT16 texUnit)
    {
		THROW_IF_NOT_RENDER_THREAD;

        setTexture_internal(texUnit, false, sNullTexPtr);
    }
	//---------------------------------------------------------------------
	void RenderSystem::disableTextureUnitsFrom(UINT16 texUnit)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::disableTextureUnitsFrom_internal, this, texUnit));
	}
    //---------------------------------------------------------------------
    void RenderSystem::disableTextureUnitsFrom_internal(UINT16 texUnit)
    {
		THROW_IF_NOT_RENDER_THREAD;

        UINT16 disableTo = CM_MAX_TEXTURE_LAYERS;
        if (disableTo > mDisabledTexUnitsFrom)
            disableTo = mDisabledTexUnitsFrom;
        mDisabledTexUnitsFrom = texUnit;
        for (UINT16 i = texUnit; i < disableTo; ++i)
        {
            disableTextureUnit_internal(i);
        }
    }
	//-----------------------------------------------------------------------
	void RenderSystem::setTextureFiltering(UINT16 unit, FilterOptions minFilter,
		FilterOptions magFilter, FilterOptions mipFilter)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setTextureFiltering_internal, this, unit, minFilter, magFilter, mipFilter));
	}
    //-----------------------------------------------------------------------
    void RenderSystem::setTextureFiltering_internal(UINT16 unit, FilterOptions minFilter,
            FilterOptions magFilter, FilterOptions mipFilter)
    {
		THROW_IF_NOT_RENDER_THREAD;

        setTextureFiltering_internal(unit, FT_MIN, minFilter);
        setTextureFiltering_internal(unit, FT_MAG, magFilter);
        setTextureFiltering_internal(unit, FT_MIP, mipFilter);
    }
	//-----------------------------------------------------------------------
	void RenderSystem::setTextureAnisotropy(UINT16 unit, unsigned int maxAnisotropy)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setTextureAnisotropy_internal, this, unit, maxAnisotropy));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setTextureAddressingMode(UINT16 unit, const SamplerState::UVWAddressingMode& uvw)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setTextureAddressingMode_internal, this, unit, uvw));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setTextureBorderColor(UINT16 unit, const Color& color)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setTextureBorderColor_internal, this, unit, color));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setTextureMipmapBias(UINT16 unit, float bias)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setTextureMipmapBias_internal, this, unit, bias));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setPointParameters(float size, bool attenuationEnabled, 
		float constant, float linear, float quadratic, float minSize, float maxSize)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setPointParameters_internal, this, size, attenuationEnabled, constant, linear, quadratic, minSize, maxSize));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setSceneBlending_internal, this, sourceFactor, destFactor, op));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setSeparateSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceFactorAlpha, 
		SceneBlendFactor destFactorAlpha, SceneBlendOperation op, SceneBlendOperation alphaOp)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setSeparateSceneBlending_internal, this, sourceFactor, destFactor, sourceFactorAlpha, destFactorAlpha, op, alphaOp));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setAlphaRejectSettings(CompareFunction func, unsigned char value, bool alphaToCoverage)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setAlphaRejectSettings_internal, this, func, value, alphaToCoverage));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setScissorTest(bool enabled, UINT32 left, UINT32 top, UINT32 right, UINT32 bottom)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setScissorTest_internal, this, enabled, left, top, right, bottom));
	}
	//-----------------------------------------------------------------------
	bool RenderSystem::getWaitForVerticalBlank(void) const
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		return mActiveContext->waitForVerticalBlank;
	}
	//-----------------------------------------------------------------------
	bool RenderSystem::getWaitForVerticalBlank_internal(void) const
	{
		THROW_IF_NOT_RENDER_THREAD;

		return mVsync;
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setWaitForVerticalBlank(bool enabled)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->waitForVerticalBlank = enabled;
		mActiveContext->queueCommand(boost::bind(&RenderSystem::setWaitForVerticalBlank_internal, this, enabled));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setWaitForVerticalBlank_internal(bool enabled)
	{
		THROW_IF_NOT_RENDER_THREAD;

		mVsync = enabled;
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setInvertVertexWinding(bool invert)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->invertVertexWinding = invert;
		mActiveContext->queueCommand(boost::bind(&RenderSystem::setInvertVertexWinding_internal, this, invert));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setInvertVertexWinding_internal(bool invert)
	{
		THROW_IF_NOT_RENDER_THREAD;

		mInvertVertexWinding = invert;
	}
	//-----------------------------------------------------------------------
	bool RenderSystem::getInvertVertexWinding(void) const
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		return mActiveContext->invertVertexWinding;
	}
	//-----------------------------------------------------------------------
	bool RenderSystem::getInvertVertexWinding_internal(void) const
	{
		THROW_IF_NOT_RENDER_THREAD;

		return mInvertVertexWinding;
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setDepthBufferParams(bool depthTest, bool depthWrite, CompareFunction depthFunction)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setDepthBufferParams_internal, this, depthTest, depthWrite, depthFunction));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setDepthBufferCheckEnabled(bool enabled)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setDepthBufferCheckEnabled_internal, this, enabled));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setDepthBufferWriteEnabled(bool enabled)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setDepthBufferWriteEnabled_internal, this, enabled));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setDepthBufferFunction(CompareFunction func)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setDepthBufferFunction_internal, this, func));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setColorBufferWriteEnabled_internal, this, red, green, blue, alpha));
	}
	//---------------------------------------------------------------------
	void RenderSystem::setDepthBias(float constantBias, float slopeScaleBias)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setDepthBias_internal, this, constantBias, slopeScaleBias));
	}
	//---------------------------------------------------------------------
	void RenderSystem::setPolygonMode(PolygonMode level)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setPolygonMode_internal, this, level));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setStencilCheckEnabled(bool enabled)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setStencilCheckEnabled_internal, this, enabled));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setStencilBufferParams(CompareFunction func, UINT32 refValue, UINT32 mask, StencilOperation stencilFailOp, 
		StencilOperation depthFailOp, StencilOperation passOp, bool twoSidedOperation)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setStencilBufferParams_internal, this, func, refValue, mask, stencilFailOp, depthFailOp, passOp, twoSidedOperation));
	}
	//-----------------------------------------------------------------------
	CullingMode RenderSystem::getCullingMode(void) const
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		return mActiveContext->cullingMode;
	}
	//-----------------------------------------------------------------------
	CullingMode RenderSystem::getCullingMode_internal(void) const
	{
		THROW_IF_NOT_RENDER_THREAD;
		return mCullingMode;
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setCullingMode(CullingMode mode)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->cullingMode = mode;

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setCullingMode_internal, this, mode));
	}
	//---------------------------------------------------------------------
	void RenderSystem::addClipPlane(const Plane &p)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::addClipPlane_internal, this, p));
	}
	//---------------------------------------------------------------------
	void RenderSystem::addClipPlane_internal (const Plane &p)
	{
		THROW_IF_NOT_RENDER_THREAD;

		mClipPlanes.push_back(p);
		mClipPlanesDirty = true;
	}
	//---------------------------------------------------------------------
	void RenderSystem::addClipPlane(float A, float B, float C, float D)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::addClipPlane_internal, this, A, B, C, D));
	}
	//---------------------------------------------------------------------
	void RenderSystem::addClipPlane_internal (float A, float B, float C, float D)
	{
		THROW_IF_NOT_RENDER_THREAD;

		addClipPlane_internal(Plane(A, B, C, D));
	}
	//---------------------------------------------------------------------
	void RenderSystem::setClipPlanes(const PlaneList& clipPlanes)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setClipPlanes_internal, this, clipPlanes));
	}
	//---------------------------------------------------------------------
	void RenderSystem::setClipPlanes_internal(const PlaneList& clipPlanes)
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
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::resetClipPlanes_internal, this));
	}
	//---------------------------------------------------------------------
	void RenderSystem::resetClipPlanes_internal()
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
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::bindGpuProgram_internal, this, prg));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::bindGpuProgram_internal(GpuProgramHandle prg)
	{
		THROW_IF_NOT_RENDER_THREAD;

		switch(prg->getBindingDelegate_internal()->getType())
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
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

	    mActiveContext->queueCommand(boost::bind(&RenderSystem::unbindGpuProgram_internal, this, gptype));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::unbindGpuProgram_internal(GpuProgramType gptype)
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
	void RenderSystem::bindGpuProgramParameters(GpuProgramType gptype, 
		GpuProgramParametersSharedPtr params, UINT16 variabilityMask)
	{
		THROW_IF_INVALID_CONTEXT;

		GpuProgramParametersSharedPtr paramCopy = GpuProgramParametersSharedPtr(new GpuProgramParameters(*params));

		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::bindGpuProgramParameters_internal, this, gptype, paramCopy, variabilityMask));
	}
	//-----------------------------------------------------------------------
	bool RenderSystem::isGpuProgramBound_internal(GpuProgramType gptype)
	{
		THROW_IF_NOT_RENDER_THREAD;

	    switch(gptype)
	    {
        case GPT_VERTEX_PROGRAM:
            return mActiveContext->vertexProgramBound;
        case GPT_GEOMETRY_PROGRAM:
            return mActiveContext->geometryProgramBound;
        case GPT_FRAGMENT_PROGRAM:
            return mActiveContext->fragmentProgramBound;
	    }
        // Make compiler happy
        return false;
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setRenderTarget(RenderTarget *target)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setRenderTarget_internal, this, target));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::clearFrameBuffer(unsigned int buffers, const Color& color, float depth, unsigned short stencil)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::clearFrameBuffer_internal, this, buffers, color, depth, stencil));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::beginFrame()
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::beginFrame_internal, this));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::endFrame()
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::endFrame_internal, this));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::render(const RenderOperation& op)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::render_internal, this, op));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::render_internal(const RenderOperation& op)
	{
		THROW_IF_NOT_RENDER_THREAD;

		// sort out clip planes
		// have to do it here in case of matrix issues
		if (mClipPlanesDirty)
		{
			setClipPlanesImpl(mClipPlanes);
			mClipPlanesDirty = false;
		}
	}

	/************************************************************************/
	/* 								PRIVATE		                     		*/
	/************************************************************************/

	void RenderSystem::initRenderThread()
	{
		mRenderThreadFunc = new RenderWorkerFunc(this);

#if CM_THREAD_SUPPORT
		CM_THREAD_CREATE(t, *mRenderThreadFunc);
		mRenderThread = t;

		CM_LOCK_MUTEX_NAMED(mRenderThreadStartMutex, lock)
		CM_THREAD_WAIT(mRenderThreadStartCondition, mRenderThreadStartMutex, lock)

#else
		CM_EXCEPT(InternalErrorException, "Attempting to start a render thread but Camelot isn't compiled with thread support.");
#endif
	}

	void RenderSystem::runRenderThread()
	{
		mRenderThreadId = CM_THREAD_CURRENT_ID;

		CM_THREAD_NOTIFY_ALL(mRenderThreadStartCondition)

		while(true)
		{
			if(mRenderThreadShutdown)
				return;

			// Wait until we get some ready commands
			vector<CommandQueue::Command>::type* commands = nullptr;
			{
				CM_LOCK_MUTEX_NAMED(mCommandQueueMutex, lock)

				while(mCommandQueue->isEmpty())
					CM_THREAD_WAIT(mCommandReadyCondition, mCommandQueueMutex, lock)

				commands = mCommandQueue->flush();
			}

			// Play commands
			mCommandQueue->playback(commands, boost::bind(&RenderSystem::commandCompletedNotify, this, _1)); 
		}

	}

	void RenderSystem::shutdownRenderThread()
	{
		mRenderThreadShutdown = true;

		// Wake all threads. They will quit after they see the shutdown flag
		CM_THREAD_NOTIFY_ALL(mCommandReadyCondition)

		mRenderThread->join();
		CM_THREAD_DESTROY(mRenderThread);

		mRenderThread = nullptr;
		mRenderThreadId = CM_THREAD_CURRENT_ID;
	}

	DeferredRenderContextPtr RenderSystem::createDeferredContext()
	{
		return DeferredRenderContextPtr(new DeferredRenderContext(this, CM_THREAD_CURRENT_ID));
	}
	
	AsyncOp RenderSystem::queueReturnCommand(boost::function<void(AsyncOp&)> commandCallback, bool blockUntilComplete)
	{
#ifdef CM_DEBUG_MODE
		if(CM_THREAD_CURRENT_ID == getRenderThreadId())
			CM_EXCEPT(InternalErrorException, "You are not allowed to call this method on the render thread!");
#endif

		AsyncOp op;
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
#ifdef CM_DEBUG_MODE
		if(CM_THREAD_CURRENT_ID == getRenderThreadId())
			CM_EXCEPT(InternalErrorException, "You are not allowed to call this method on the render thread!");
#endif

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
		if(CM_THREAD_CURRENT_ID != getRenderThreadId())
			CM_EXCEPT(InternalErrorException, "Calling the render system from a non-render thread!");
	}

	void RenderSystem::throwIfInvalidContextThread() const
	{
		if(CM_THREAD_CURRENT_ID != mActiveContext->getThreadId())
			CM_EXCEPT(InternalErrorException, "Active context is called from a thread it was not initialized on!");
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

#undef THROW_IF_INVALID_CONTEXT
#undef THROW_IF_NOT_RENDER_THREAD