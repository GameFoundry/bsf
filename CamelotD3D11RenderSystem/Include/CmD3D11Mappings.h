#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmCommonEnums.h"
#include "CmTexture.h"
#include "CmPixelData.h"
#include "CmIndexBuffer.h"
#include "CmVertexData.h"
#include "CmIndexData.h"
#include "CmSamplerState.h"
#include "CmDrawOps.h"

namespace CamelotFramework
{
	class CM_D3D11_EXPORT D3D11Mappings
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

		/// return a D3D11 equivalent for an engine TextureAddressingMode value
		static D3D11_TEXTURE_ADDRESS_MODE get(TextureAddressingMode tam);
		/// return a D3D11 equivalent for an engine SceneBlendFactor value
		static D3D11_BLEND get(BlendFactor sbf);
		/// return a D3D11 equivalent for an engine SceneBlendOperation value
		static D3D11_BLEND_OP get(BlendOperation sbo);
		/// return a D3D11 equivalent for an engine CompareFunction value
		static D3D11_COMPARISON_FUNC get(CompareFunction cf);
		/// return a D3D11 equivalent for an engine CillingMode value
		static D3D11_CULL_MODE get(CullingMode cm);
		/// return a D3D11 equivalent for an engine PolygonMode value
		static D3D11_FILL_MODE get(PolygonMode level);
		/// return a D3D11 equivalent for an engine StencilOperation value
		static D3D11_STENCIL_OP get(StencilOperation op, bool invert = false);
		/// return a D3D11 state type for engine FilterType value
		static DWORD get(FilterType ft);
		static D3D11_FILTER get(const FilterOptions minification, const FilterOptions magnification, const FilterOptions mips, const bool comparison = false);
		/// return the combination of D3DUSAGE values for engine buffer usage
		static DWORD get(GpuBufferUsage usage);
		/// Get lock options
		static D3D11_MAP get(GpuLockOptions options, GpuBufferUsage usage);
		static UINT getByteWidth(IndexBuffer::IndexType itype);
		/// Get vertex data type
		static DXGI_FORMAT get(VertexElementType vType);
		/// Get vertex semantic
		static LPCSTR get(VertexElementSemantic sem);
		static VertexElementSemantic get(LPCSTR sem);
		static VertexElementType getInputType(D3D_REGISTER_COMPONENT_TYPE type);
		static D3D11_PRIMITIVE_TOPOLOGY getPrimitiveType(DrawOperationType type);
		/// Get dx11 color
		static void get(const Color& inColour, float * outColour );
		static bool isMappingWrite(D3D11_MAP map);
		static bool isMappingRead(D3D11_MAP map);
		static D3D11_BOX toDx11Box(const PixelVolume &inBox);

		/// utility method, convert D3D11 pixel format to engine pixel format
		static PixelFormat _getPF(DXGI_FORMAT d3dPF);
		/// utility method, convert engine pixel format to D3D11 pixel format
		static DXGI_FORMAT _getPF(PixelFormat ogrePF);
		//
		static D3D11_USAGE _getUsage(GpuBufferUsage mUsage);
		static D3D11_USAGE _getUsage(int mUsage);
		static UINT _getAccessFlags(GpuBufferUsage mUsage);
		static UINT _getAccessFlags(int mUsage);
		static bool _isDynamic(GpuBufferUsage mUsage);

		static bool _isDynamic(int mUsage);

		/// utility method, find closest engine pixel format that D3D11 can support
		static PixelFormat _getClosestSupportedPF(PixelFormat ogrePF);

		static TextureType _getTexType(D3D11_SRV_DIMENSION type);

		static UINT32 _getSizeInBytes(PixelFormat pf, UINT32 xcount = 1, UINT32 ycount = 1);

		static D3D11_MAP getLockOptions(GpuLockOptions lockOptions);
	};
}