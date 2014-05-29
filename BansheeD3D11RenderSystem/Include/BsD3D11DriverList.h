#pragma once

#include "BsD3D11Prerequisites.h"

namespace BansheeEngine
{
	class BS_D3D11_EXPORT D3D11DriverList
	{
	public:
		D3D11DriverList(IDXGIFactory* dxgiFactory);
		~D3D11DriverList();

		UINT32 count() const;

		D3D11Driver* item(UINT32 idx) const;
		D3D11Driver* item(const String &name) const;

	private:
		Vector<D3D11Driver*> mDriverList;

		void enumerate(IDXGIFactory* dxgiFactory);
	};
}