#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class D3D11RenderUtility : public Module<D3D11RenderUtility>
	{
	public:
		D3D11RenderUtility(D3D11Device* device);
		~D3D11RenderUtility();

		void drawClearQuad(UINT32 clearBuffers, const Color& color, float depth, UINT16 stencil);

	protected:
		D3D11Device* mDevice;

		void initClearQuadResources();

		ID3D11Buffer* mClearQuadIB;
		ID3D11Buffer* mClearQuadVB;
		ID3D11InputLayout* mClearQuadIL;
		ID3D11VertexShader* mClearQuadVS;
		ID3D11PixelShader* mClearQuadPS;

		HBlendState mClearQuadBlendStateYesC;
		HBlendState mClearQuadBlendStateNoC;
		HRasterizerState mClearQuadRasterizerState;
		HDepthStencilState mClearQuadDSStateNoD_NoS;
		HDepthStencilState mClearQuadDSStateYesD_NoS;
		HDepthStencilState mClearQuadDSStateYesD_YesS;
		HDepthStencilState mClearQuadDSStateNoD_YesS;
	};
}