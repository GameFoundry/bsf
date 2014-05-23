#pragma once

#include "CmD3D9Prerequisites.h"

namespace BansheeEngine {

	/** Represents a Direct3D rendering resource.
	Provide unified interface to
	handle various device states.
	*/
	class CM_D3D9_EXPORT D3D9Resource
	{
	// Interface.
	public:
		D3D9Resource();
		virtual ~D3D9Resource();

		// Called immediately after the Direct3D device has been created.
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device) {}

		// Called before the Direct3D device is going to be destroyed.
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device) {}

		// Called immediately after the Direct3D device has entered a lost state.
		// This is the place to release non-managed resources.
		virtual void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device) {}

		// Called immediately after the Direct3D device has been reset.
		// This is the place to create non-managed resources.
		virtual void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device) {}

		// Called when device state is changing. Access to any device should be locked.
		// Relevant for multi thread application.
		static void lockDeviceAccess();

		// Called when device state change completed. Access to any device is allowed.
		// Relevant for multi thread application.
		static void unlockDeviceAccess();

	protected:
		CM_STATIC_MUTEX(msDeviceAccessMutex)		
	};
}