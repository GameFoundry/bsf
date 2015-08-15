#pragma once

#include "BsD3D9Prerequisites.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Provides an interface for dealing with DX9 resources. Primarily
	 *			notifying the resources of any changed states.
	 */
	class BS_D3D9_EXPORT D3D9Resource
	{
	public:
		D3D9Resource();
		virtual ~D3D9Resource();

		/**
		 * @brief	Called immediately after the Direct3D device has been created.
		 */
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device) {}

		/**
		 * @brief	Called before the Direct3D device is going to be destroyed.
		 */
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device) {}

		/**
		 * @brief	Called immediately after the Direct3D device has entered a lost state.
		 */
		virtual void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device) {}

		/**
		 * @brief	Called immediately after the Direct3D device has been reset.
		 */
		virtual void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device) {}

		/**
		 * @brief	Called when device state is changing. Access to any device should be locked.
		 *			Relevant for multi thread application.
		 */
		static void lockDeviceAccess();

		/**
		 * @brief	Called when device state change completed. Access to any device is allowed.
		 *			Relevant for multi thread application.
		 */
		static void unlockDeviceAccess();

	protected:
		BS_STATIC_MUTEX(msDeviceAccessMutex)		
	};
}