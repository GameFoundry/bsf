#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmCommon.h"
#include "CmTexture.h"
#include "CmPixelData.h"
#include "CmHardwareIndexBuffer.h"
#include "CmVertexIndexData.h"
#include "CmSamplerState.h"

namespace CamelotEngine
{
	class D3D11Mappings
	{
	public:

		/// enum identifying D3D11 filter usage type
		enum eD3DFilterUsage
		{
			/// min filter
			D3D_FUSAGE_MIN,
			/// mag filter
			D3D_FUSAGE_MAG,
			/// mip filter
			D3D_FUSAGE_MIP
		};

		/// return a D3D11 equivalent for a Ogre TextureAddressingMode value
		static D3D11_TEXTURE_ADDRESS_MODE get(SamplerState::TextureAddressingMode tam);
		/// return a D3D11 equivalent for a Ogre SceneBlendFactor value
		static D3D11_BLEND get(SceneBlendFactor sbf);
		/// return a D3D11 equivalent for a Ogre SceneBlendOperation value
		static D3D11_BLEND_OP get(SceneBlendOperation sbo);
		/// return a D3D11 equivalent for a Ogre CompareFunction value
		static D3D11_COMPARISON_FUNC get(CompareFunction cf);
		/// return a D3D11 equivalent for a Ogre CillingMode value
		static D3D11_CULL_MODE get(CullingMode cm, bool flip = false);
		/// return a D3D11 equivalent for a Ogre PolygonMode value
		static D3D11_FILL_MODE get(PolygonMode level);
		/// return a D3D11 equivalent for a Ogre StencilOperation value
		static D3D11_STENCIL_OP get(StencilOperation op, bool invert = false);
		/// return a D3D11 state type for Ogre FilterType value
		static DWORD get(FilterType ft);
		static D3D11_FILTER get(const FilterOptions minification, const FilterOptions magnification, const FilterOptions mips, const bool comparison = false);
		/// return the combination of D3DUSAGE values for Ogre buffer usage
		static DWORD get(HardwareBuffer::Usage usage);
		/// Get lock options
		static D3D11_MAP get(HardwareBuffer::LockOptions options, HardwareBuffer::Usage usage);
		static UINT getByteWidth(HardwareIndexBuffer::IndexType itype);
		/// Get vertex data type
		static DXGI_FORMAT get(VertexElementType vType);
		/// Get vertex semantic
		static LPCSTR get(VertexElementSemantic sem);
		static VertexElementSemantic get(LPCSTR sem);
		/// Get dx11 color
		static void get(const Color& inColour, float * outColour );


		/// utility method, convert D3D11 pixel format to Ogre pixel format
		static PixelFormat _getPF(DXGI_FORMAT d3dPF);
		/// utility method, convert Ogre pixel format to D3D11 pixel format
		static DXGI_FORMAT _getPF(PixelFormat ogrePF);
		//
		static D3D11_USAGE _getUsage(HardwareBuffer::Usage mUsage);
		static D3D11_USAGE _getUsage(int mUsage);
		static UINT _getAccessFlags(HardwareBuffer::Usage mUsage);
		static UINT _getAccessFlags(int mUsage);
		static bool _isDynamic(HardwareBuffer::Usage mUsage);

		static bool _isDynamic(int mUsage);

		/// utility method, find closest Ogre pixel format that D3D11 can support
		static PixelFormat _getClosestSupportedPF(PixelFormat ogrePF);

		static TextureType _getTexType(D3D11_SRV_DIMENSION type);

		static size_t _getSizeInBytes(PixelFormat pf, size_t xcount = 1, size_t ycount = 1);

		static UINT _getTextureBindFlags(DXGI_FORMAT format, bool isdynamic);
		static UINT _getTextureMiscFlags(UINT bindflags, TextureType textype, bool isdynamic);
	};
}