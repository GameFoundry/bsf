//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsD3D11Prerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains a list of all available drivers.
	 */
	class BS_D3D11_EXPORT D3D11DriverList
	{
	public:
		/**
		 * @brief	Constructs a new driver list from an existing DXGI factory object.
		 */
		D3D11DriverList(IDXGIFactory* dxgiFactory);
		~D3D11DriverList();

		/**
		 * @brief	Returns the number of available drivers.
		 */
		UINT32 count() const;

		/**
		 * @brief	Returns a driver at the specified index.
		 */
		D3D11Driver* item(UINT32 idx) const;

		/**
		 * @brief	Returns a driver with the specified name, or null if it cannot be found.
		 */
		D3D11Driver* item(const String &name) const;

	private:
		/**
		 * @brief	Enumerates the DXGI factory object and constructs a list of available drivers.
		 */
		void enumerate(IDXGIFactory* dxgiFactory);

		Vector<D3D11Driver*> mDriverList;
	};
}