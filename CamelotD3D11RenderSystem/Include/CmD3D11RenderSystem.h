#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmRenderSystem.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11RenderSystem : public RenderSystem
	{
	public:
		
		static D3D11Device& getPrimaryDevice();

		void determineFSAASettings(UINT32 fsaa, const String& fsaaHint, DXGI_FORMAT format, DXGI_SAMPLE_DESC* outFSAASettings);
	protected:

	private:
	};
}