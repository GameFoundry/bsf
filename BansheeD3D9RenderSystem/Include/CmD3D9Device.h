#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmRenderTarget.h"

namespace BansheeEngine 
{
	/** High level interface of Direct3D9 Device.
	Provide useful methods for device handling.
	*/
	class BS_D3D9_EXPORT D3D9Device
	{
	protected:
		struct RenderWindowResources
		{
			IDirect3DSwapChain9* swapChain;	// Swap chain interface.
			UINT32 adapterOrdinalInGroupIndex; // Relative index of the render window in the group.
			UINT32 presentParametersIndex; // Index of present parameter in the shared array of the device.
			IDirect3DSurface9* backBuffer; // The back buffer of the render window.
			IDirect3DSurface9* depthBuffer;	// The depth buffer of the render window.
			D3DPRESENT_PARAMETERS presentParameters; // Present parameters of the render window.
			bool acquired; // True if resources acquired.			
		};

	public:
		D3D9Device(D3D9DeviceManager* deviceManager, UINT adapterNumber, HMONITOR hMonitor,
			D3DDEVTYPE devType, DWORD behaviorFlags);
		~D3D9Device();

		void attachRenderWindow(const D3D9RenderWindow* renderWindow);
		void detachRenderWindow(const D3D9RenderWindow* renderWindow);
	
		bool acquire();
		
		void release();		
		void destroy();		
		
		bool isDeviceLost();				
		IDirect3DDevice9* getD3D9Device() const;
					
		UINT getAdapterNumber() const;
		D3DDEVTYPE getDeviceType() const;
		bool isMultihead() const;					
		bool isAutoDepthStencil() const;
		
		const D3DCAPS9& getD3D9DeviceCaps() const;
		D3DFORMAT getBackBufferFormat() const;
		D3DFORMAT getDepthStencilFormat() const;

		bool validate(D3D9RenderWindow* renderWindow);
		void invalidate(const D3D9RenderWindow* renderWindow);

		void present(const D3D9RenderWindow* renderWindow);
		
		IDirect3DSurface9* getDepthBuffer(const D3D9RenderWindow* renderWindow);
		IDirect3DSurface9* getBackBuffer(const D3D9RenderWindow* renderWindow);

		void setAdapterOrdinalIndex(const D3D9RenderWindow* renderWindow, UINT32 adapterOrdinalInGroupIndex);
		void copyContentsToMemory(const D3D9RenderWindow* window, const PixelData &dst, RenderTarget::FrameBuffer buffer);
		void clearDeviceStreams();
	
	protected:
		friend class D3D9DeviceManager;
		friend class D3D9RenderSystem;

		typedef Map<const D3D9RenderWindow*, RenderWindowResources*> RenderWindowToResorucesMap;
		typedef RenderWindowToResorucesMap::iterator				 RenderWindowToResorucesIterator;

		RenderWindowToResorucesIterator getRenderWindowIterator(const D3D9RenderWindow* renderWindow);

		bool acquire(const D3D9RenderWindow* renderWindow);
		bool reset();
		void updatePresentationParameters();
		void updateRenderWindowsIndices();

		void createD3D9Device();
		void releaseD3D9Device();
		void releaseRenderWindowResources(RenderWindowResources* renderWindowResources);
		void acquireRenderWindowResources(RenderWindowToResorucesIterator it);
		void notifyDeviceLost();

		void validateFocusWindow();
		void validateBackBufferSize(const D3D9RenderWindow* renderWindow);
		bool validateDisplayMonitor(D3D9RenderWindow* renderWindow);
		bool validateDeviceState(const D3D9RenderWindow* renderWindow);
		bool isSwapChainWindow(const D3D9RenderWindow* renderWindow);
		const D3D9RenderWindow*	getPrimaryWindow();
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
	
		RenderWindowToResorucesMap mMapRenderWindowToResoruces;		// Map between render window to resources.
	
	};
}