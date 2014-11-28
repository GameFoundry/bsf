#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsRenderAPI.h"
#include "BsHardwareBuffer.h"
#include "BsIndexBuffer.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Provides helper methods for mapping between engine and DirectX 9 types.
	 */
	class BS_D3D9_EXPORT D3D9Mappings
	{
	public:
		/**
		 * @brief	DirectX 9 texture types.
		 */
		enum D3DTexType
		{
			D3D_TEX_TYPE_NORMAL,
			D3D_TEX_TYPE_CUBE,
			D3D_TEX_TYPE_VOLUME,
			D3D_TEX_TYPE_NONE
		};

		/**
		 * @brief	Returns DirectX 9 texture addressing mode. Returns exact mode if supported, or
		 *			nearest available if not.
		 */
		static D3DTEXTUREADDRESS get(TextureAddressingMode tam, const D3DCAPS9& devCaps);

		/**
		 * @brief	Returns DirectX 9 blend factor.
		 */
		static D3DBLEND get(BlendFactor sbf);

		/**
		 * @brief	Returns DirectX 9 blend operation
		 */
		static D3DBLENDOP get(BlendOperation sbo);

		/**
		 * @brief	Return DirectX 9 compare function.
		 */
		static DWORD get(CompareFunction cf);

		/**
		 * @brief	Returns DirectX 9 culling mode. Optionally flip the mode so that
		 *			engine CCW is DX9 CW and reverse.
		 */
		static DWORD get(CullingMode cm, bool flip);

		/**
		 * @brief	Return DirectX 9 fill mode depending on provided polygon mode.
		 */
		static D3DFILLMODE get(PolygonMode level);

		/**
		 * @brief	Return DirectX 9 stencil operation and optionally 
		 *			invert it (greater than becomes less than, etc.)
		 */
		static DWORD get(StencilOperation op, bool invert = false);

		/**
		 * @brief	Returns DirectX 9 sampler state based on provided filter type.
		 */
		static D3DSAMPLERSTATETYPE get(FilterType ft);

		/**
		 * @brief	Returns a DirectX 9 texture filter type based on provided filter type, options and texture type.
		 *			If wanted filter type is not available closest type will be returned.
		 */
		static DWORD get(FilterType ft, FilterOptions fo, const D3DCAPS9& devCaps, D3DTexType texType);
		
		/**
		 * @brief	Returns DirectX 9 texture type.
		 */
		static D3DTexType get(TextureType textype);
        
		/**
		 * @brief	Return DirectX 9 buffer usage.
		 */
        static DWORD get(GpuBufferUsage usage);
        
		/**
		 * @brief	Returns DirectX 9 lock options, constrained by the provided usage.
		 */
        static DWORD get(GpuLockOptions options, GpuBufferUsage usage);
        
		/**
		 * @brief	Returns DirectX 9 index buffer type.
		 */
        static D3DFORMAT get(IndexType itype);
		
		/**
		 * @brief	Returns DirectX 9 vertex element type.
		 */
		static D3DDECLTYPE get(VertexElementType vType);

		/**
		 * @brief	Returns DirectX9 vertex element semantic.
		 */
		static D3DDECLUSAGE get(VertexElementSemantic sem);
        
		/**
		 * @brief	Converts a matrix to one usable by DirectX 9 API.
		 */
        static 	D3DXMATRIX makeD3DXMatrix(const Matrix4& mat);
        
		/**
		 * @brief	Converts matrix returned by DirectX 9 API to engine matrix.
		 */
        static Matrix4 convertD3DXMatrix(const D3DXMATRIX& mat);

		/**
		 * @brief	Converts DirectX 9 pixel format to engine pixel format.
		 */
		static PixelFormat _getPF(D3DFORMAT d3dPF);
		
		/**
		 * @brief	Converts engine pixel format to DirectX 9 pixel format.
		 */
		static D3DFORMAT _getPF(PixelFormat pf);

		/**
		 * @brief	Returns closest pixel format supported by DirectX 9.
		 */
		static PixelFormat _getClosestSupportedPF(PixelFormat pf);

		/**
		 * @brief	Returns closest color render target pixel format supported by DirectX 9.
		 */
		static PixelFormat _getClosestSupportedRenderTargetPF(PixelFormat pf);

		/**
		 * @brief	Returns closest depth/stencil format supported by DirectX 9.
		 */
		static PixelFormat _getClosestSupportedDepthStencilPF(PixelFormat pf);
	};
}