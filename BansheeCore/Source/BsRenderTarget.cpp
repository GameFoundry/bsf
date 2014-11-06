#include "BsRenderTarget.h"
#include "BsViewport.h"
#include "BsException.h"
#include "BsRenderSystem.h"
#include "BsCoreThread.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	void RenderTargetProperties::copyToBuffer(UINT8* buffer) const
	{
		*(RenderTargetProperties*)buffer = *this;
	}

	void RenderTargetProperties::copyFromBuffer(UINT8* buffer)
	{
		*this = *(RenderTargetProperties*)buffer;
	}

	UINT32 RenderTargetProperties::getSize() const
	{
		return sizeof(RenderTargetProperties);
	}

	RenderTargetCore::RenderTargetCore(RenderTarget* parent, RenderTargetProperties* properties)
		:mProperties(properties), mParent(parent)
	{

	}

	RenderTargetCore::~RenderTargetCore()
	{
		bs_delete(mProperties);
	}

	CoreSyncData RenderTargetCore::syncFromCore(FrameAlloc* allocator)
	{
		UINT8* buffer = allocator->alloc(mProperties->getSize());
		mProperties->copyToBuffer(buffer);

		return CoreSyncData(buffer, mProperties->getSize());
	}

	void RenderTargetCore::syncToCore(const CoreSyncData& data)
	{
		assert(data.getBufferSize() == mProperties->getSize());

		mProperties->copyFromBuffer(data.getBuffer());
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

	RenderTargetCore* RenderTarget::getCore() const
	{
		return static_cast<RenderTargetCore*>(mCoreSpecific);
	}

	RenderTarget::RenderTarget()
		:mProperties(nullptr)
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

	CoreSyncData RenderTarget::syncToCore(FrameAlloc* allocator)
	{
		UINT8* buffer = allocator->alloc(mProperties->getSize());
		mProperties->copyToBuffer(buffer);

		return CoreSyncData(buffer, mProperties->getSize());
	}

	void RenderTarget::syncFromCore(const CoreSyncData& data)
	{
		assert(data.getBufferSize() == mProperties->getSize());

		mProperties->copyFromBuffer(data.getBuffer());
	}

	void RenderTarget::getCustomAttribute(const String& name, void* pData) const
	{
		BS_EXCEPT(InvalidParametersException, "Attribute not found.");
	}
}        
