#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGpuParamBlock.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11GpuParamBlockBuffer : public GpuParamBlockBuffer
	{
	public:
		D3D11GpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage);
		~D3D11GpuParamBlockBuffer();

		/**
		 * @copydoc CpuParamBlockBuffer::writeAll.
		 */
		void writeAll(const void* data);

		ID3D11Buffer* getD3D11Buffer() const;

	private:
		D3D11HardwareBuffer* mBuffer;
	};

	class CM_D3D11_EXPORT D3D11GpuParamBlock : public GpuParamBlock
	{
	protected:
		/**
		 * @copydoc GpuParamBlock::createBuffer.
		 */
		GpuParamBlockBuffer* createBuffer() const;
	};
}