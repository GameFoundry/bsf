#include "CmD3D11GpuParamBlock.h"
#include "CmD3D11HardwareBuffer.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"

namespace CamelotEngine
{
	D3D11GpuParamBlock::D3D11GpuParamBlock(const GpuParamBlockDesc& desc, GpuParamBlockUsage usage)
		:GpuParamBlock(desc, usage), mD3D11SharedData(nullptr)
	{
		mD3D11SharedData = new D3D11GpuParamBlockSharedData();
	}

	D3D11GpuParamBlock::~D3D11GpuParamBlock()
	{
		if(mOwnsSharedData)
		{
			delete mD3D11SharedData->mBuffer;
			delete mD3D11SharedData;
		}
	}

	void D3D11GpuParamBlock::updateIfDirty()
	{
		if(!sharedData->mInitialized)
		{
			D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
			D3D11Device& device = d3d11rs->getPrimaryDevice();

			mD3D11SharedData->mBuffer = new D3D11HardwareBuffer(D3D11HardwareBuffer::BT_CONSTANT, GBU_STATIC, 1, mSize, device);

			sharedData->mInitialized = true;
		}

		if(sharedData->mDirty)
		{
			mD3D11SharedData->mBuffer->writeData(0, mSize, mData, true);
		}

		GpuParamBlock::updateIfDirty();
	}

	GpuParamBlockPtr D3D11GpuParamBlock::clone() const
	{
		std::shared_ptr<D3D11GpuParamBlock> clonedParamBlock(new D3D11GpuParamBlock(*this));
		clonedParamBlock->mData = new UINT8[mSize];
		clonedParamBlock->mOwnsSharedData = false;
		clonedParamBlock->mD3D11SharedData = mD3D11SharedData;
		memcpy(clonedParamBlock->mData, mData, mSize);

		return clonedParamBlock;
	}

	ID3D11Buffer* D3D11GpuParamBlock::getD3D11Buffer() const 
	{ 
		return mD3D11SharedData->mBuffer->getD3DBuffer(); 
	}
}