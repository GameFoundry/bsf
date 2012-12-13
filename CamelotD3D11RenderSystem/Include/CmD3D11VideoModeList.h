#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmD3D11VideoMode.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11VideoModeList
	{
	public:
		D3D11VideoModeList(D3D11Driver* driver, UINT32 adapterOutput = 0);
		~D3D11VideoModeList();

		UINT32 count() const;

		const D3D11VideoMode& item(UINT32 idx) const;
		const D3D11VideoMode& item(const String &name) const;

	private:
		D3D11Driver* mDriver;
		vector<D3D11VideoMode>::type mModeList;

		void enumerate(UINT32 adapterOutput);
	};
}