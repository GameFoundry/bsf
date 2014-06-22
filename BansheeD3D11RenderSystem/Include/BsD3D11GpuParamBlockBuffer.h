#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsGpuParamBlockBuffer.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 11 implementation of a parameter block buffer (constant buffer in DX11 lingo).
	 */
	class BS_D3D11_EXPORT D3D11GpuParamBlockBuffer : public GpuParamBlockBuffer
	{
	public:
		D3D11GpuParamBlockBuffer();

		/**
		 * @copydoc GpuParamBlockBuffer::writeData.
		 */
		void writeData(const UINT8* data);

		/**
		 * @copydoc GpuParamBlockBuffer::readData.
		 */
		void readData(UINT8* data) const;

		/**
		 * @brief	Returns internal DX11 buffer object.
		 */
		ID3D11Buffer* getD3D11Buffer() const;
	protected:
		/**
		 * @copydoc CoreGpuObject::initialize_internal.
		 */
		virtual void initialize_internal();

		/**
		 * @copydoc CoreGpuObject::destroy_internal.
		 */
		virtual void destroy_internal();

	private:
		D3D11HardwareBuffer* mBuffer;
	};
}