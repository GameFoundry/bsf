#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsHardwareBuffer.h"

namespace BansheeEngine
{
	class BS_D3D11_EXPORT D3D11HardwareBuffer : public HardwareBuffer
	{
	public:
		enum BufferType
		{
			BT_VERTEX = 0x1,
			BT_INDEX = 0x2,
			BT_CONSTANT = 0x4,
			BT_GROUP_GENERIC = 0x8,
			BT_STRUCTURED = BT_GROUP_GENERIC | 0x10,
			BT_RAW = BT_GROUP_GENERIC | 0x20,
			BT_INDIRECTARGUMENT = BT_GROUP_GENERIC | 0x40,
			BT_APPENDCONSUME = BT_GROUP_GENERIC | 0x80
		};

		D3D11HardwareBuffer(BufferType btype, GpuBufferUsage usage, UINT32 elementCount, UINT32 elementSize, 
			D3D11Device& device, bool useSystemMem = false, bool streamOut = false, bool randomGpuWrite = false, bool useCounter = false);
		~D3D11HardwareBuffer();

		/** See HardwareBuffer. */
		void readData(UINT32 offset, UINT32 length, void* pDest);
		/** See HardwareBuffer. */
		void writeData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags = BufferWriteType::Normal);
		/** See HardwareBuffer. We perform a hardware copy here. */
		void copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, 
			UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false);

		/// Get the D3D-specific buffer
		ID3D11Buffer* getD3DBuffer(void) { return mD3DBuffer; }

	protected:
		BufferType mBufferType;
		bool mRandomGpuWrite;
		bool mUseCounter;
		UINT32 mElementCount;
		UINT32 mElementSize;

		ID3D11Buffer* mD3DBuffer;

		bool mUseTempStagingBuffer;
		D3D11HardwareBuffer* mpTempStagingBuffer;
		bool mStagingUploadNeeded;
		
		D3D11Device& mDevice;
		D3D11_BUFFER_DESC mDesc;

		/** See HardwareBuffer. */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);
		/** See HardwareBuffer. */
		void unlockImpl(void);
	};
}