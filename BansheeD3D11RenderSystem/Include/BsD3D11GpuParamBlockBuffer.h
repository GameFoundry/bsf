#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsGpuParamBlockBuffer.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 11 implementation of a parameter block buffer (constant buffer in DX11 lingo).
	 */
	class BS_D3D11_EXPORT D3D11GpuParamBlockBufferCore : public GpuParamBlockBufferCore
	{
	public:
		D3D11GpuParamBlockBufferCore(UINT32 size, GpuParamBlockUsage usage);

		/**
		 * @copydoc GpuParamBlockBufferCore::writeData.
		 */
		void writeData(const UINT8* data);

		/**
		 * @copydoc GpuParamBlockBufferCore::readData.
		 */
		void readData(UINT8* data) const;

		/**
		 * @brief	Returns internal DX11 buffer object.
		 */
		ID3D11Buffer* getD3D11Buffer() const;
	protected:
		/**
		 * @copydoc GpuParamBlockBufferCore::initialize
		 */
		virtual void initialize();

		/**
		 * @copydoc GpuParamBlockBufferCore::destroy
		 */
		virtual void destroy();

	private:
		D3D11HardwareBuffer* mBuffer;
	};
}