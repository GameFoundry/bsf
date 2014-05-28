#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGpuParamBlockBuffer.h"

namespace BansheeEngine
{
	class BS_D3D11_EXPORT D3D11GpuParamBlockBuffer : public GpuParamBlockBuffer
	{
	public:
		D3D11GpuParamBlockBuffer();

		/**
		 * @brief	Writes all of the data to the buffer.
		 * 			Data size must be the same size as the buffer;
		 */
		void writeData(const UINT8* data);

		/**
		 * @copydoc GpuParamBlockBuffer::readAll.
		 */
		void readData(UINT8* data) const;

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