#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmHardwareBuffer.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11HardwareBuffer : public HardwareBuffer
	{
	public:
		enum BufferType
		{
			VERTEX_BUFFER,
			INDEX_BUFFER,
			CONSTANT_BUFFER
		};

	protected:
		ID3D11Buffer* mD3DBuffer;
		bool mUseTempStagingBuffer;
		D3D11HardwareBuffer* mpTempStagingBuffer;
		bool mStagingUploadNeeded;
		BufferType mBufferType;
		D3D11Device& mDevice;
		D3D11_BUFFER_DESC mDesc;

		/** See HardwareBuffer. */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);
		/** See HardwareBuffer. */
		void unlockImpl(void);

	public:
		D3D11HardwareBuffer(BufferType btype, UINT32 sizeBytes, GpuBufferUsage usage, 
			D3D11Device& device, bool useSystemMem, bool streamOut);
		~D3D11HardwareBuffer();

		/** See HardwareBuffer. */
		void readData(UINT32 offset, UINT32 length, void* pDest);
		/** See HardwareBuffer. */
		void writeData(UINT32 offset, UINT32 length, const void* pSource,
			bool discardWholeBuffer = false);
		/** See HardwareBuffer. We perform a hardware copy here. */
		void copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, 
			UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false);

		/// Get the D3D-specific buffer
		ID3D11Buffer* getD3DBuffer(void) { return mD3DBuffer; }
	};
}