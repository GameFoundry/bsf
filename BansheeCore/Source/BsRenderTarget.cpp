#include "BsRenderTarget.h"
#include "BsViewport.h"
#include "BsException.h"
#include "BsRenderSystem.h"
#include "BsCoreThread.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	RenderTargetCore::RenderTargetCore()
	{

	}

	void RenderTargetCore::setActive(bool state)
	{ 
		RenderTargetProperties& props = const_cast<RenderTargetProperties&>(getProperties());

		props.mActive = state;
		markCoreDirty();
	}

	void RenderTargetCore::setPriority(INT32 priority) 
	{ 
		RenderTargetProperties& props = const_cast<RenderTargetProperties&>(getProperties());

		props.mPriority = priority;
		markCoreDirty();
	}

	const RenderTargetProperties& RenderTargetCore::getProperties() const
	{ 
		return getPropertiesInternal(); 
	}

	void RenderTargetCore::getCustomAttribute(const String& name, void* pData) const
	{
		BS_EXCEPT(InvalidParametersException, "Attribute not found.");
	}

	void RenderTarget::setPriority(CoreAccessor& accessor, UINT32 priority)
	{
		std::function<void(SPtr<RenderTargetCore>, UINT32)> windowedFunc =
			[](SPtr<RenderTargetCore> renderTarget, UINT32 priority)
		{
			renderTarget->setPriority(priority);
		};

		accessor.queueCommand(std::bind(windowedFunc, getCore(), priority));
	}

	SPtr<RenderTargetCore> RenderTarget::getCore() const
	{
		return std::static_pointer_cast<RenderTargetCore>(mCoreSpecific);
	}

	const RenderTargetProperties& RenderTarget::getProperties() const
	{
		THROW_IF_CORE_THREAD;

		return getPropertiesInternal();
	}

	void RenderTarget::getCustomAttribute(const String& name, void* pData) const
	{
		BS_EXCEPT(InvalidParametersException, "Attribute not found.");
	}

}        
