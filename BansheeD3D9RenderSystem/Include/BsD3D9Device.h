#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsRenderTarget.h"

namespace BansheeEngine 
{
	/**
	 * @brief	High level interface for a DX9 device. Each device represents
	 *			a hardware adapter or a software emulation device.
	 */
	class BS_D3D9_EXPORT D3D9Device
	{
	protected:
		/**
		 * @brief	Holds device specific render window resources.
		 */
		struct RenderWindowResources
		{
			IDirect3DSwapChain9* swapChain;	/**< Swap chain interface. */
			UINT32 adapterOrdinalInGroupIndex; /**< Relative index of the render window in the group. */
			UINT32 presentParametersIndex; /**< Index of present parameter in the shared array of the device. */
			IDirect3DSurface9* backBuffer; /**< The back buffer of the render window. */
			IDirect3DSurface9* depthBuffer;	/**< The depth buffer of the render window. */
			D3DPRESENT_PARAMETERS presentParameters; /**< Present parameters of the render window. */
			bool acquired; /**< True if resources acquired. */	
		};

	public:
		D3D9Device(D3D9DeviceManager* deviceManager, UINT adapterNumber, HMONITOR hMonitor,
			D3DDEVTYPE devType, DWORD behaviorFlags);
		~D3D9Device();

		/**
		 * @brief	Attaches a new render window to this device. Caller must ensure
		 *			the window is not attached to multiple devices.
		 */
		void attachRenderWindow(const D3D9RenderWindow* renderWindow);

		/**
		 * @brief	Detaches the render window from this device.
		 */
		void detachRenderWindow(const D3D9RenderWindow* renderWindow);
	
		/**
		 * @brief	Acquires the device. This will cause a device reset in case present parameters changed.
		 */
		bool acquire();
		
		/**
		 * @brief	Release the device and all resources directly managed by it.
		 */
		void release();

		/**
		 * @brief	Destroys the device and all resources directly managed by it.
		 */
		void destroy();		
		
		/**
		 * @brief	Checks is the device lost. If lost you will need to "acquire" the device.
		 */
		bool isDeviceLost();				
		
		/**
		 * @brief	Return internal DX9 device object.
		 */
		IDirect3DDevice9* getD3D9Device() const;

		/**
		 * @brief	Returns adapter number this device is tied to. This number corresponds to the
		 *			adapter index returned by DX9 API.
		 */
		UINT getAdapterNumber() const;

		/**
		 * @brief	Returns type of the device.
		 */
		D3DDEVTYPE getDeviceType() const;

		/**
		 * @brief	Checks is the device multihead (manages multiple full-screen outputs).
		 */
		bool isMultihead() const;					

		/**
		 * @brief	Returns true if depth/stencil format can be automatically determined.
		 */
		bool isAutoDepthStencil() const;
		
		/**
		 * @brief	Returns DX9 device capabilities.
		 */
		const D3DCAPS9& getD3D9DeviceCaps() const;

		/**
		 * @brief	Returns DX9 format of the back buffer used by the primary window for this device.
		 */
		D3DFORMAT getBackBufferFormat() const;

		/**
		 * @brief	Returns DX9 format of the depth stencil buffer used by the primary window for this device.
		 */
		D3DFORMAT getDepthStencilFormat() const;

		/**
		 * @brief	Validates that the window is valid for this device. Will reset device if needed.
		 */
		bool validate(D3D9RenderWindow* renderWindow);

		/**
		 * @brief	Invalidates the window so on the next call to validate, the device will be re-acquired.
		 */
		void invalidate(const D3D9RenderWindow* renderWindow);

		/**
		 * @brief	Swap back and front buffers for the specified window.
		 */
		void present(const D3D9RenderWindow* renderWindow);
		
		/**
		 * @brief	Returns internal DX9 represention of the depth/stencil buffer.
		 */
		IDirect3DSurface9* getDepthBuffer(const D3D9RenderWindow* renderWindow);

		/**
		 * @brief	Returns internal DX9 represention of the backbuffer.
		 */
		IDirect3DSurface9* getBackBuffer(const D3D9RenderWindow* renderWindow);

		/**
		 * @brief	Sets adapter index for the specified window.
		 */
		void setAdapterOrdinalIndex(const D3D9RenderWindow* renderWindow, UINT32 adapterOrdinalInGroupIndex);

		/**
		 * @brief	Copies contents of the back or depth/stencil buffer in to the provided object.
		 */
		void copyContentsToMemory(const D3D9RenderWindow* window, PixelData &dst, RenderTarget::FrameBuffer buffer);

		/**
		 * @brief	Resets bound pipeline states/streams to null.
		 */
		void clearDeviceStreams();
	
	protected:
		friend class D3D9DeviceManager;
		friend class D3D9RenderSystem;

		typedef Map<const D3D9RenderWindow*, RenderWindowResources*> RenderWindowToResorucesMap;
		typedef RenderWindowToResorucesMap::iterator				 RenderWindowToResorucesIterator;

		/**
		 * @brief	Find iterator for the specified window in the render window resource list.
		 */
		RenderWindowToResorucesIterator getRenderWindowIterator(const D3D9RenderWindow* renderWindow);

		/**
		 * @brief	Acquires the device for the provided render window.
		 */
		bool acquire(const D3D9RenderWindow* renderWindow);

		/**
		 * @brief	Forcibly reset the device.
		 */
		bool reset();

		/**
		 * @brief	Update presentation parameters from the active render window.
		 */
		void updatePresentationParameters();

		/**
		 * @brief	Updates presentation parameter indices for all windows attached to this device.
		 */
		void updateRenderWindowsIndices();

		/**
		 * @brief	Creates a new DX9 device object.
		 */
		void createD3D9Device();

		/**
		 * @brief	Releases the DX9 device object.
		 */
		void releaseD3D9Device();

		/**
		 * @brief	Releases all render window resources in the provided object.
		 */
		void releaseRenderWindowResources(RenderWindowResources* renderWindowResources);

		/**
		 * @brief	Acquires all render window resources from the provided render window,
		 *			and stores them in the provided render window resources object.
		 */
		void acquireRenderWindowResources(RenderWindowToResorucesIterator it);

		/**
		 * @brief	Called when it has been detected that device has been lost.
		 */
		void notifyDeviceLost();

		/**
		 * @brief	Checks if focus window changed and should device be reacquired.
		 */
		void validateFocusWindow();

		/**
		 * @brief	Checks if back buffer size has changed and invalidates the window if it has.
		 */
		void validateBackBufferSize(const D3D9RenderWindow* renderWindow);

		/**
		 * @brief	Checks if window monitor changed and re-links the window if needed.
		 */
		bool validateDisplayMonitor(D3D9RenderWindow* renderWindow);

		/**
		 * @brief	Checks if device has been lost or active window invalidated and acquires the device if needed.
		 */
		bool validateDeviceState(const D3D9RenderWindow* renderWindow);

		/**
		 * @brief	Checks if the render window contains a custom swap chain.
		 */
		bool isSwapChainWindow(const D3D9RenderWindow* renderWindow);

		/**
		 * @brief	Returns primary window for this device.
		 */
		const D3D9RenderWindow*	getPrimaryWindow();

		/**
		 * @brief	Sets the shared window handle.
		 */
		void setSharedWindowHandle(HWND hSharedHWND);

	protected:			
		D3D9DeviceManager* mpDeviceManager;	/**< The manager of this device instance. */
		IDirect3DDevice9* mpDevice;	/**< Will hold the device interface. */
		UINT mAdapterNumber; /**< The adapter that this device belongs to. */
		HMONITOR mMonitor; /**< The monitor that this device belongs to. */
		D3DDEVTYPE mDeviceType; /**< Device type. */
		static HWND	msSharedFocusWindow; /**< The shared focus window in case of multiple full screen render windows. */
		HWND mFocusWindow; /**< The focus window this device attached to. */	
		DWORD mBehaviorFlags; /**< The behavior of this device.	*/
		
		/** Presentation parameters which the device was created with. May be
		 *  an array of presentation parameters in case of multi-head device.
		 */
		D3DPRESENT_PARAMETERS* mPresentationParams;
		UINT mPresentationParamsCount; /**< Number of presentation parameters elements.	 */
		D3DCAPS9 mD3D9DeviceCaps; /**< Device caps. */
		bool mD3D9DeviceCapsValid; /**< True if device caps initialized. */
		D3DDEVICE_CREATION_PARAMETERS mCreationParams; /**< Creation parameters. */
		bool mDeviceLost; /**< True if device entered lost state. */
	
		RenderWindowToResorucesMap mMapRenderWindowToResoruces;	/**< Map between render window to resources. */
	
	};
}