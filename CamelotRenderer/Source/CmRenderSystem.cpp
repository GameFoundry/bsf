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
        // This means CULL clockwise vertices, i.e. front of poly is counter-clockwise
        // This makes it the same as OpenGL and other right-handed systems
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
    {
    }

    //-----------------------------------------------------------------------
    RenderSystem::~RenderSystem()
    {
        shutdown();

		delete mCurrentCapabilities;
		mCurrentCapabilities = 0;
    }
	//-----------------------------------------------------------------------
	void RenderSystem::startUp()
	{
		mRenderThreadId = CM_THREAD_CURRENT_ID;
		mResourceContext = createRenderSystemContext();
		mPrimaryContext = createRenderSystemContext();
		mActiveContext = mPrimaryContext;

		initRenderThread(); // TODO - Move render thread to the outside of the RS

		{
			CM_LOCK_MUTEX(mResourceContextMutex);
			mResourceContext->queueCommand(boost::bind(&RenderSystem::startUp_internal, this));
		}
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
		// TODO - I should probably sync this up to make sure no other threads are doing anything while shutdown is in progress

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
		{
			CM_LOCK_MUTEX(mResourceContextMutex)

			if(miscParams != nullptr)
				op = mResourceContext->queueReturnCommand(boost::bind(&RenderSystem::createRenderWindow_internal, this, name, width, height, fullScreen, *miscParams, _1));
			else
				op = mResourceContext->queueReturnCommand(boost::bind(&RenderSystem::createRenderWindow_internal, this, name, width, height, fullScreen, NameValuePairList(), _1));
		}

		submitToGpu(mResourceContext, true);
		return op.getReturnValue<RenderWindow*>();
	}
    //---------------------------------------------------------------------------------------------
    void RenderSystem::destroyRenderWindow_internal(const String& name)
    {
		THROW_IF_NOT_RENDER_THREAD;

        destroyRenderTarget_internal(name);
    }
    //---------------------------------------------------------------------------------------------
    void RenderSystem::destroyRenderTexture_internal(const String& name)
    {
		THROW_IF_NOT_RENDER_THREAD;

        destroyRenderTarget_internal(name);
    }
    //---------------------------------------------------------------------------------------------
    void RenderSystem::destroyRenderTarget_internal(const String& name)
    {
		THROW_IF_NOT_RENDER_THREAD;

        RenderTarget* rt = detachRenderTarget(name);
        delete rt;
    }
    //---------------------------------------------------------------------------------------------
    void RenderSystem::attachRenderTarget_internal( RenderTarget &target )
    {
		THROW_IF_NOT_RENDER_THREAD;

		assert( target.getPriority() < CM_NUM_RENDERTARGET_GROUPS );

        mRenderTargets.insert( RenderTargetMap::value_type( target.getName(), &target ) );
        mPrioritisedRenderTargets.insert(
            RenderTargetPriorityMap::value_type(target.getPriority(), &target ));
    }

    //---------------------------------------------------------------------------------------------
    RenderTarget * RenderSystem::getRenderTarget( const String &name )
    {
		THROW_IF_NOT_RENDER_THREAD;

        RenderTargetMap::iterator it = mRenderTargets.find( name );
        RenderTarget *ret = NULL;

        if( it != mRenderTargets.end() )
        {
            ret = it->second;
        }

        return ret;
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
    //---------------------------------------------------------------------------------------------
    RenderTarget * RenderSystem::detachRenderTarget( const String &name )
    {
		THROW_IF_NOT_RENDER_THREAD;

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
    void RenderSystem::setTextureUnitSettings(size_t texUnit, const TexturePtr& tex, const SamplerState& samplerState)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setTextureUnitSettings_internal, this, texUnit, tex, samplerState));
	}
	//-----------------------------------------------------------------------
    void RenderSystem::setTextureUnitSettings_internal(size_t texUnit, const TexturePtr& tex, const SamplerState& tl)
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
	void RenderSystem::setTexture(size_t unit, bool enabled, const TexturePtr &texPtr)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setTexture_internal, this, unit, enabled, texPtr));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setVertexTexture(size_t unit, const TexturePtr& tex)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setVertexTexture_internal, this, unit, tex));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setVertexTexture_internal(size_t unit, const TexturePtr& tex)
	{
		THROW_IF_NOT_RENDER_THREAD;

		CM_EXCEPT(NotImplementedException, 
			"This rendersystem does not support separate vertex texture samplers, "
			"you should use the regular texture samplers which are shared between "
			"the vertex and fragment units.");
	}
	//-----------------------------------------------------------------------
	void RenderSystem::disableTextureUnit(size_t texUnit)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::disableTextureUnit_internal, this, texUnit));
	}
    //-----------------------------------------------------------------------
    void RenderSystem::disableTextureUnit_internal(size_t texUnit)
    {
		THROW_IF_NOT_RENDER_THREAD;

        setTexture_internal(texUnit, false, sNullTexPtr);
    }
	//---------------------------------------------------------------------
	void RenderSystem::disableTextureUnitsFrom(size_t texUnit)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::disableTextureUnitsFrom_internal, this, texUnit));
	}
    //---------------------------------------------------------------------
    void RenderSystem::disableTextureUnitsFrom_internal(size_t texUnit)
    {
		THROW_IF_NOT_RENDER_THREAD;

        size_t disableTo = CM_MAX_TEXTURE_LAYERS;
        if (disableTo > mDisabledTexUnitsFrom)
            disableTo = mDisabledTexUnitsFrom;
        mDisabledTexUnitsFrom = texUnit;
        for (size_t i = texUnit; i < disableTo; ++i)
        {
            disableTextureUnit_internal(i);
        }
    }
	//-----------------------------------------------------------------------
	void RenderSystem::setTextureFiltering(size_t unit, FilterOptions minFilter,
		FilterOptions magFilter, FilterOptions mipFilter)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setTextureFiltering_internal, this, unit, minFilter, magFilter, mipFilter));
	}
    //-----------------------------------------------------------------------
    void RenderSystem::setTextureFiltering_internal(size_t unit, FilterOptions minFilter,
            FilterOptions magFilter, FilterOptions mipFilter)
    {
		THROW_IF_NOT_RENDER_THREAD;

        setTextureFiltering_internal(unit, FT_MIN, minFilter);
        setTextureFiltering_internal(unit, FT_MAG, magFilter);
        setTextureFiltering_internal(unit, FT_MIP, mipFilter);
    }
	//-----------------------------------------------------------------------
	void RenderSystem::setTextureAnisotropy(size_t unit, unsigned int maxAnisotropy)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setTextureAnisotropy_internal, this, unit, maxAnisotropy));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setTextureAddressingMode(size_t unit, const SamplerState::UVWAddressingMode& uvw)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setTextureAddressingMode_internal, this, unit, uvw));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setTextureBorderColor(size_t unit, const Color& color)
	{
		THROW_IF_INVALID_CONTEXT;
		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext->queueCommand(boost::bind(&RenderSystem::setTextureBorderColor_internal, this, unit, color));
	}
	//-----------------------------------------------------------------------
	void RenderSystem::setTextureMipmapBias(size_t unit, float bias)
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
	void RenderSystem::setScissorTest(bool enabled, size_t left, size_t top, size_t right, size_t bottom)
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

	void RenderSystem::setActiveContext(RenderSystemContextPtr context)
	{
		assert(context != nullptr);

		CM_LOCK_MUTEX(mActiveContextMutex);

		mActiveContext = context;
	}

	AsyncOp RenderSystem::queueResourceReturnCommand(boost::function<void(AsyncOp&)> commandCallback, bool blockUntilComplete, UINT32 _callbackId)
	{
		AsyncOp op;
		{
			CM_LOCK_MUTEX(mResourceContextMutex)

			op = mResourceContext->queueReturnCommand(commandCallback);
		}

		submitToGpu(mResourceContext, blockUntilComplete);
		return op;
	}

	void RenderSystem::queueResourceCommand(boost::function<void()> commandCallback, bool blockUntilComplete, UINT32 _callbackId)
	{
		{
			CM_LOCK_MUTEX(mResourceContextMutex)

			mResourceContext->queueCommand(commandCallback);
		}

		submitToGpu(mResourceContext, blockUntilComplete);
	}

	RenderSystemContextPtr RenderSystem::getActiveContext() const
	{
		CM_LOCK_MUTEX(mActiveContextMutex);

		return mActiveContext;
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