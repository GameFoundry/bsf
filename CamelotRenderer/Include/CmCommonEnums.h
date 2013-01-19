/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __Common_H__
#define __Common_H__
// Common stuff

#include "CmString.h"

#if defined ( CM_GCC_VISIBILITY )
#   pragma GCC visibility push(default)
#endif

#include <utility>
#include <sstream>

#if defined ( CM_GCC_VISIBILITY )
#   pragma GCC visibility pop
#endif

namespace CamelotEngine {
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup General
	*  @{
	*/

	/** Blending factors for manually blending objects with the scene. If there isn't a predefined
        SceneBlendType that you like, then you can specify the blending factors directly to affect the
        combination of object and the existing scene. See Material::setSceneBlending for more details.
    */
    enum SceneBlendFactor
    {
        SBF_ONE,
        SBF_ZERO,
        SBF_DEST_COLOUR,
        SBF_SOURCE_COLOUR,
        SBF_ONE_MINUS_DEST_COLOUR,
        SBF_ONE_MINUS_SOURCE_COLOUR,
        SBF_DEST_ALPHA,
        SBF_SOURCE_ALPHA,
        SBF_ONE_MINUS_DEST_ALPHA,
        SBF_ONE_MINUS_SOURCE_ALPHA

    };

	/** Blending operations controls how objects are blended into the scene. The default operation
		is add (+) but by changing this you can change how drawn objects are blended into the
		existing scene.
	*/
	enum SceneBlendOperation
	{
		SBO_ADD,
		SBO_SUBTRACT,
		SBO_REVERSE_SUBTRACT,
		SBO_MIN,
		SBO_MAX
	};

    /** Comparison functions used for the depth/stencil buffer operations and 
		others. */
    enum CompareFunction
    {
        CMPF_ALWAYS_FAIL,
        CMPF_ALWAYS_PASS,
        CMPF_LESS,
        CMPF_LESS_EQUAL,
        CMPF_EQUAL,
        CMPF_NOT_EQUAL,
        CMPF_GREATER_EQUAL,
        CMPF_GREATER
    };

    /** Texture addressing modes - default is TAM_WRAP.
    @note
        These settings are relevant in both the fixed-function and the
        programmable pipeline.
    */
    enum TextureAddressingMode
    {
        /// Texture wraps at values over 1.0
        TAM_WRAP,
        /// Texture mirrors (flips) at joins over 1.0
        TAM_MIRROR,
        /// Texture clamps at 1.0
        TAM_CLAMP,
        /// Texture coordinates outside the range [0.0, 1.0] are set to the border colour
        TAM_BORDER
    };

    /** High-level filtering options providing shortcuts to settings the
        minification, magnification and mip filters. */
    enum TextureFilterOptions
    {
        /// Equal to: min=FO_POINT, mag=FO_POINT, mip=FO_NONE
        TFO_NONE,
        /// Equal to: min=FO_LINEAR, mag=FO_LINEAR, mip=FO_POINT
        TFO_BILINEAR,
        /// Equal to: min=FO_LINEAR, mag=FO_LINEAR, mip=FO_LINEAR
        TFO_TRILINEAR,
        /// Equal to: min=FO_ANISOTROPIC, max=FO_ANISOTROPIC, mip=FO_LINEAR
		TFO_ANISOTROPIC
    };

    enum FilterType
    {
        /// The filter used when shrinking a texture
        FT_MIN,
        /// The filter used when magnifying a texture
        FT_MAG,
        /// The filter used when determining the mipmap
        FT_MIP
    };
    /** Filtering options for textures / mipmaps. */
    enum FilterOptions
    {
        /// No filtering, used for FILT_MIP to turn off mipmapping
        FO_NONE = 0,
        /// Use the closest pixel
        FO_POINT = 1,
        /// Average of a 2x2 pixel area, denotes bilinear for MIN and MAG, trilinear for MIP
        FO_LINEAR = 2,
        /// Similar to FO_LINEAR, but compensates for the angle of the texture plane
        FO_ANISOTROPIC = 3,
		/// Specifies that the sampled values will be compared against existing sampled data. 
		/// Should be OR-ed with other filtering options.
		FO_USE_COMPARISON = 4
    };

    /** Hardware culling modes based on vertex winding.
        This setting applies to how the hardware API culls triangles it is sent. */
    enum CullingMode
    {
        /// Hardware never culls triangles and renders everything it receives.
        CULL_NONE = 1,
        /// Hardware culls triangles whose vertices are listed clockwise in the view (default).
        CULL_CLOCKWISE = 2,
        /// Hardware culls triangles whose vertices are listed anticlockwise in the view.
        CULL_COUNTERCLOCKWISE = 3
    };

    /** The polygon mode to use when rasterising. */
    enum PolygonMode
    {
		/// Wireframe models are rendered.
        PM_WIREFRAME = 1,
		/// Solid polygons are rendered.
        PM_SOLID = 2
    };

    /** Defines the frame buffer types. */
    enum FrameBufferType {
        FBT_COLOUR  = 0x1,
        FBT_DEPTH   = 0x2,
        FBT_STENCIL = 0x4
    };

	/// Enum describing the various actions which can be taken onthe stencil buffer
	enum StencilOperation
	{
		/// Leave the stencil buffer unchanged
		SOP_KEEP,
		/// Set the stencil value to zero
		SOP_ZERO,
		/// Set the stencil value to the reference value
		SOP_REPLACE,
		/// Increase the stencil value by 1, clamping at the maximum value
		SOP_INCREMENT,
		/// Decrease the stencil value by 1, clamping at 0
		SOP_DECREMENT,
		/// Increase the stencil value by 1, wrapping back to 0 when incrementing the maximum value
		SOP_INCREMENT_WRAP,
		/// Decrease the stencil value by 1, wrapping when decrementing 0
		SOP_DECREMENT_WRAP,
		/// Invert the bits of the stencil buffer
		SOP_INVERT
	};

	/// Locking options
	enum GpuLockOptions
	{
        /** Normal mode, ie allows read/write and contents are preserved. */
        GBL_READ_WRITE,
		/** Discards the <em>entire</em> buffer while locking; this allows optimisation to be 
		performed because synchronisation issues are relaxed. 
		*/
		GBL_WRITE_ONLY_DISCARD,
		/** Lock the buffer for reading only. Not allowed in buffers which are created with HBU_WRITE_ONLY. 
		*/ 
		GBL_READ_ONLY,
        /** As HBL_NORMAL, except the application guarantees not to overwrite any 
        region of the buffer which has already been used in this frame, can allow
        some optimisation on some APIs. */
        GBL_WRITE_ONLY_NO_OVERWRITE,
		/** Lock for writing only */
		GBL_WRITE_ONLY	
	};

	/// Enums describing buffer usage; not mutually exclusive
	enum GpuBufferUsage 
	{
        /** Static buffer which the application rarely modifies once created. Modifying 
        the contents of this buffer will involve a performance hit.
        */
        GBU_STATIC = 1,
		/** Indicates the application would like to modify this buffer with the CPU
		fairly often. 
		*/
		GBU_DYNAMIC = 2
	};

	/**
	 * @brief	Types of generic GPU buffers that may be attached to GPU programs.
	 */
	enum GpuBufferType
	{
		GBT_STRUCTURED,
		GBT_RAW,
		GBT_INDIRECTARGUMENT,
		GBT_APPENDCONSUME
	};

	/**
	 * @brief	Different types of gpu views
	 */
	enum GpuViewUsage
	{
		GVU_DEFAULT = 0x01,
		GVU_RENDERTARGET = 0x02,
		GVU_DEPTHSTENCIL = 0x04,
		GVU_RANDOMWRITE = 0x08
	};

	/** Texture addressing mode for each texture coordinate. */
	struct UVWAddressingMode
	{
		UVWAddressingMode()
			:u(TAM_WRAP), v(TAM_WRAP), w(TAM_WRAP)
		{ }

		TextureAddressingMode u, v, w;
	};
    
	/// Name / value parameter pair (first = name, second = value)
	typedef map<String, String>::type NameValuePairList;

	/** @} */
}

#endif
