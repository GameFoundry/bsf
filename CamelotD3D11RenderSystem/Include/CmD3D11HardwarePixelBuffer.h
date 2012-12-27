#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmHardwarePixelBuffer.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11HardwarePixelBuffer : public HardwarePixelBuffer
	{
	public:
		D3D11HardwarePixelBuffer(D3D11Texture* parentTexture, D3D11Device& device, UINT32 subresourceIndex,
			UINT32 width, UINT32 height, UINT32 depth, UINT32 face, PixelFormat format, HardwareBuffer::Usage usage);

		/// @copydoc HardwarePixelBuffer::blit
		void blit(const HardwarePixelBufferPtr& src, const Box& srcBox, const Box& dstBox);

		/// @copydoc HardwarePixelBuffer::blitFromMemory
		void blitFromMemory(const PixelData& src, const Box& dstBox);

		/// @copydoc HardwarePixelBuffer::blitToMemory
		void blitToMemory(const Box& srcBox, const PixelData& dst);

		/// Internal function to update mipmaps on update of level 0
		void _genMipmaps();

		~D3D11HardwarePixelBuffer();

		/// Get rendertarget for z slice
		RenderTexture *getRenderTarget(UINT32 zoffset);

		/// Notify TextureBuffer of destruction of render target
		virtual void _clearSliceRTT(UINT32 zoffset)
		{
			if (mSliceTRT.size() > zoffset)
			{
				mSliceTRT[zoffset] = 0;
			}
		}

		D3D11Texture * getParentTexture() const;
		UINT32 getSubresourceIndex() const;
		UINT32 getFace() const;

	protected:
		/// Lock a box
		PixelData lockImpl(const Box lockBox, LockOptions options);

		/// Unlock a box
		void unlockImpl(void);

		/// D3DDevice pointer
		D3D11Device& mDevice;

		D3D11Texture* mParentTexture;
		UINT32 mSubresourceIndex;

		// if the usage is static - alloc at lock then use device UpdateSubresource when unlock and free memory
		UINT8* mDataForStaticUsageLock; 

		UINT32 mFace;

		Box mLockBox;
		PixelData mCurrentLock;
		LockOptions mCurrentLockOptions;

		D3D11_BOX convertBoxToDx11Box(const Box &inBox) const;

		/// Util functions to convert a D3D locked box to a pixel box
		void fromD3DLock(PixelData &rval, const DXGI_MAPPED_RECT &lrect);

		/// Render targets
		typedef vector<RenderTexture*>::type SliceTRT;
		SliceTRT mSliceTRT;

		void createStagingBuffer();
		bool mUsingStagingBuffer;
		ID3D11Resource *mStagingBuffer;

		void* _map(ID3D11Resource *res, D3D11_MAP flags);
		void* _mapstagingbuffer(D3D11_MAP flags);
		void* _mapstaticbuffer(PixelData lock);
		void _unmap(ID3D11Resource *res);
		void _unmapstagingbuffer(bool copyback = true);
		void _unmapstaticbuffer();
	};
}