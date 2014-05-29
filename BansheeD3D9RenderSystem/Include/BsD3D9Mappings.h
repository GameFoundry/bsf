
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsRenderSystem.h"
#include "BsHardwareBuffer.h"
#include "BsIndexBuffer.h"

namespace BansheeEngine 
{
	class BS_D3D9_EXPORT D3D9Mappings
	{
	public:
		/// enum identifying D3D9 tex. types
		enum eD3DTexType
		{
			/// standard texture
			D3D_TEX_TYPE_NORMAL,
			/// cube texture
			D3D_TEX_TYPE_CUBE,
			/// volume texture
			D3D_TEX_TYPE_VOLUME,
			/// just to have it...
			D3D_TEX_TYPE_NONE
		};

		/// enum identifying D3D9 filter usage type
		enum eD3DFilterUsage
		{
			/// min filter
			D3D_FUSAGE_MIN,
			/// mag filter
			D3D_FUSAGE_MAG,
			/// mip filter
			D3D_FUSAGE_MIP
		};

		/// return a D3D9 equivalent for a Ogre TextureAddressingMode value
		static D3DTEXTUREADDRESS get(TextureAddressingMode tam, const D3DCAPS9& devCaps);
		/// return a D3D9 equivalent for a Ogre SceneBlendFactor value
		static D3DBLEND get(BlendFactor sbf);
		/// return a D3D9 equivlalent for a Ogre SceneBlendOperation value
		static D3DBLENDOP get(BlendOperation sbo);
		/// return a D3D9 equivalent for a Ogre CompareFunction value
		static DWORD get(CompareFunction cf);
		/// return a D3D9 equivalent for a Ogre CillingMode value
		static DWORD get(CullingMode cm, bool flip);
		/// return a D3D9 equivalent for a Ogre PolygonMode value
		static D3DFILLMODE get(PolygonMode level);
		/// return a D3D9 equivalent for a Ogre StencilOperation value
		static DWORD get(StencilOperation op, bool invert = false);
		/// return a D3D9 state type for Ogre FilterType value
		static D3DSAMPLERSTATETYPE get(FilterType ft);
		/// return a D3D9 filter option for Ogre FilterType & FilterOption value
		static DWORD get(FilterType ft, FilterOptions fo, const D3DCAPS9& devCaps, eD3DTexType texType);
		/// return the D3DtexType equivalent of a Ogre tex. type
		static eD3DTexType get(TextureType ogreTexType);
        /// return the combination of D3DUSAGE values for Ogre buffer usage
        static DWORD get(GpuBufferUsage usage);
        /// Get lock options
        static DWORD get(GpuLockOptions options, GpuBufferUsage usage);
        /// Get index type
        static D3DFORMAT get(IndexBuffer::IndexType itype);
		/// Get vertex data type
		static D3DDECLTYPE get(VertexElementType vType);
		/// Get vertex semantic
		static D3DDECLUSAGE get(VertexElementSemantic sem);
        // Convert matrix to D3D style
        static 	D3DXMATRIX makeD3DXMatrix( const Matrix4& mat );
        // Convert matrix from D3D style
        static Matrix4 convertD3DXMatrix( const D3DXMATRIX& mat );

		/// utility method, convert D3D9 pixel format to engine pixel format
		static PixelFormat _getPF(D3DFORMAT d3dPF);
		/// utility method, convert engine pixel format to D3D9 pixel format
		static D3DFORMAT _getPF(PixelFormat ogrePF);

		static PixelFormat _getClosestSupportedPF(PixelFormat ogrePF);
		static PixelFormat _getClosestSupportedRenderTargetPF(PixelFormat enginePF);
		static PixelFormat _getClosestSupportedDepthStencilPF(PixelFormat enginePF);
	};
}