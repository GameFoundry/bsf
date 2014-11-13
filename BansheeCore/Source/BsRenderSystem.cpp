#include "BsRenderSystem.h"

#include "BsCoreThread.h"
#include "BsViewport.h"
#include "BsException.h"
#include "BsRenderTarget.h"
#include "BsRenderWindow.h"
#include "BsPixelBuffer.h"
#include "BsOcclusionQuery.h"
#include "BsResource.h"
#include "BsCoreThread.h"
#include "BsMesh.h"
#include "BsProfilerCPU.h"
#include "BsRenderStats.h"

using namespace std::placeholders;

namespace BansheeEngine 
{
    static const SPtr<TextureCore> sNullTexPtr;

    RenderSystem::RenderSystem()
        : mCullingMode(CULL_COUNTERCLOCKWISE)
        , mDisabledTexUnitsFrom(0)
        , mVertexProgramBound(false)
		, mGeometryProgramBound(false)
        , mFragmentProgramBound(false)
		, mDomainProgramBound(false)
		, mHullProgramBound(false)
		, mComputeProgramBound(false)
		, mClipPlanesDirty(true)
		, mCurrentCapabilities(nullptr)
    {
    }

    RenderSystem::~RenderSystem()
    {
		// Base classes need to call virtual destroy_internal method instead of a destructor

		bs_delete(mCurrentCapabilities);
		mCurrentCapabilities = nullptr;
    }

	RenderWindowPtr RenderSystem::initialize(const RENDER_WINDOW_DESC& primaryWindowDesc)
	{
		gCoreThread().queueCommand(std::bind(&RenderSystem::initializePrepare, this), true);

		RENDER_WINDOW_DESC windowDesc = primaryWindowDesc;
		RenderWindowPtr renderWindow = RenderWindow::create(windowDesc, nullptr);

		gCoreThread().queueCommand(std::bind(&RenderSystem::initializeFinalize, this, renderWindow->getCore()), true);

		return renderWindow;
	}

	void RenderSystem::initializePrepare()
	{
		// Do nothing
	}

	void RenderSystem::initializeFinalize(const SPtr<RenderWindowCore>& primaryWindow)
	{
		THROW_IF_NOT_CORE_THREAD;

		mVertexProgramBound = false;
		mGeometryProgramBound = false;
		mFragmentProgramBound = false;
		mDomainProgramBound = false;
		mHullProgramBound = false;
		mComputeProgramBound = false;
	}

	void RenderSystem::destroy()
	{
		gCoreAccessor().queueCommand(std::bind(&RenderSystem::destroy_internal, this));
		gCoreThread().submitAccessors(true);
	}

	void RenderSystem::destroy_internal()
	{
		mActiveRenderTarget = nullptr;
	}

	const RenderSystemCapabilities* RenderSystem::getCapabilities(void) const 
	{ 
		return mCurrentCapabilities; 
	}

	const DriverVersion& RenderSystem::getDriverVersion(void) const 
	{ 
		THROW_IF_NOT_CORE_THREAD;

		return mDriverVersion; 
	}

    void RenderSystem::disableTextureUnit(GpuProgramType gptype, UINT16 texUnit)
    {
		THROW_IF_NOT_CORE_THREAD;

        setTexture(gptype, texUnit, false, sNullTexPtr);
    }

	void RenderSystem::addClipPlane (const Plane &p)
	{
		THROW_IF_NOT_CORE_THREAD;

		mClipPlanes.push_back(p);
		mClipPlanesDirty = true;
	}

	void RenderSystem::setClipPlanes(const PlaneList& clipPlanes)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (clipPlanes != mClipPlanes)
		{
			mClipPlanes = clipPlanes;
			mClipPlanesDirty = true;
		}
	}

	void RenderSystem::resetClipPlanes()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (!mClipPlanes.empty())
		{
			mClipPlanes.clear();
			mClipPlanesDirty = true;
		}
	}

	void RenderSystem::bindGpuProgram(const SPtr<GpuProgramCore>& prg)
	{
		THROW_IF_NOT_CORE_THREAD;

		switch(prg->getProperties().getType())
		{
		case GPT_VERTEX_PROGRAM:
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
		case GPT_DOMAIN_PROGRAM:
			mDomainProgramBound = true;
			break;
		case GPT_HULL_PROGRAM:
			mHullProgramBound = true;
			break;
		case GPT_COMPUTE_PROGRAM:
			mComputeProgramBound = true;
			break;
		}
	}

	void RenderSystem::unbindGpuProgram(GpuProgramType gptype)
	{
		THROW_IF_NOT_CORE_THREAD;

		switch(gptype)
		{
		case GPT_VERTEX_PROGRAM:
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
		case GPT_DOMAIN_PROGRAM:
			mDomainProgramBound = false;
			break;
		case GPT_HULL_PROGRAM:
			mHullProgramBound = false;
			break;
		case GPT_COMPUTE_PROGRAM:
			mComputeProgramBound = false;
			break;
		}
	}

	bool RenderSystem::isGpuProgramBound(GpuProgramType gptype)
	{
		THROW_IF_NOT_CORE_THREAD;

	    switch(gptype)
	    {
        case GPT_VERTEX_PROGRAM:
            return mVertexProgramBound;
        case GPT_GEOMETRY_PROGRAM:
            return mGeometryProgramBound;
        case GPT_FRAGMENT_PROGRAM:
            return mFragmentProgramBound;
		case GPT_DOMAIN_PROGRAM:
			return mDomainProgramBound;
		case GPT_HULL_PROGRAM:
			return mHullProgramBound;
		case GPT_COMPUTE_PROGRAM:
			return mComputeProgramBound;
	    }

        return false;
	}

	void RenderSystem::swapBuffers(const SPtr<RenderTargetCore>& target)
	{
		THROW_IF_NOT_CORE_THREAD;

		target->swapBuffers();

		BS_INC_RENDER_STAT(NumPresents);
	}
}