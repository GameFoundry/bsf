#include "BsRenderTarget.h"
#include "BsViewport.h"
#include "BsException.h"
#include "BsRenderSystem.h"
#include "BsCoreThread.h"
#include "BsRenderTargetManager.h"

namespace BansheeEngine 
{
	void RenderTargetProperties::copyFrom(const RenderTargetProperties& other)
	{
		*this = other;
	}

	RenderTargetCore::RenderTargetCore(RenderTarget* parent, RenderTargetProperties* properties)
		:mProperties(properties), mParent(parent)
	{

	}

	RenderTargetCore::~RenderTargetCore()
	{
		bs_delete(mProperties);
	}

	void RenderTargetCore::getCustomAttribute(const String& name, void* pData) const
	{
		BS_EXCEPT(InvalidParametersException, "Attribute not found.");
	}

	const RenderTargetProperties& RenderTargetCore::getProperties() const 
	{ 
		THROW_IF_NOT_CORE_THREAD;

		return *mProperties; 
	}

    RenderTarget::RenderTarget()
		:mCore(nullptr), mProperties(nullptr)
    {
    }

    RenderTarget::~RenderTarget()
    {
		bs_delete(mProperties);
    }

	const RenderTargetProperties& RenderTarget::getProperties() const
	{
		THROW_IF_CORE_THREAD;

		return *mProperties;
	}

	RenderTargetCore* RenderTarget::getCore() const
	{
		return mCore;
	}

	void RenderTarget::initialize_internal()
	{
		CoreObject::initialize_internal();

		mCore = createCore();

		RenderTargetManager::instance().registerRenderTarget(this);
	}

	void RenderTarget::destroy_internal()
	{
		RenderTargetManager::instance().unregisterRenderTarget(this);

		bs_delete(mCore);
		mCore = nullptr;

		CoreObject::destroy_internal();
	}

	void RenderTarget::getCustomAttribute(const String& name, void* pData) const
	{
		BS_EXCEPT(InvalidParametersException, "Attribute not found.");
	}
}        
