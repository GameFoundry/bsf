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
        FO_NONE,
        /// Use the closest pixel
        FO_POINT,
        /// Average of a 2x2 pixel area, denotes bilinear for MIN and MAG, trilinear for MIP
        FO_LINEAR,
        /// Similar to FO_LINEAR, but compensates for the angle of the texture plane
        FO_ANISOTROPIC
    };

    /** Light shading modes. */
    enum ShadeOptions
    {
        SO_FLAT,
        SO_GOURAUD,
        SO_PHONG
    };

    /** Fog modes. */
    enum FogMode
    {
        /// No fog. Duh.
        FOG_NONE,
        /// Fog density increases  exponentially from the camera (fog = 1/e^(distance * density))
        FOG_EXP,
        /// Fog density increases at the square of FOG_EXP, i.e. even quicker (fog = 1/e^(distance * density)^2)
        FOG_EXP2,
        /// Fog density increases linearly between the start and end distances
        FOG_LINEAR
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
        CULL_ANTICLOCKWISE = 3
    };

    /** The polygon mode to use when rasterising. */
    enum PolygonMode
    {
		/// Only points are rendered.
        PM_POINTS = 1,
		/// Wireframe models are rendered.
        PM_WIREFRAME = 2,
		/// Solid polygons are rendered.
        PM_SOLID = 3
    };

    /** An enumeration describing which material properties should track the vertex colours */
    typedef int TrackVertexColourType;
    enum TrackVertexColourEnum {
        TVC_NONE        = 0x0,
        TVC_AMBIENT     = 0x1,        
        TVC_DIFFUSE     = 0x2,
        TVC_SPECULAR    = 0x4,
        TVC_EMISSIVE    = 0x8
    };

    /** Sort mode for billboard-set and particle-system */
    enum SortMode
    {
        /** Sort by direction of the camera */
        SM_DIRECTION,
        /** Sort by distance from the camera */
        SM_DISTANCE
    };

    /** Defines the frame buffer types. */
    enum FrameBufferType {
        FBT_COLOUR  = 0x1,
        FBT_DEPTH   = 0x2,
        FBT_STENCIL = 0x4
    };
    
	/// Name / value parameter pair (first = name, second = value)
	typedef map<String, String>::type NameValuePairList;

    /** Structure used to define a box in a 3-D integer space.
        Note that the left, top, and front edges are included but the right, 
        bottom and back ones are not.
        */
    struct Box
    {
        size_t left, top, right, bottom, front, back;
		/// Parameterless constructor for setting the members manually
        Box()
			: left(0), top(0), right(1), bottom(1), front(0), back(1)
        {
        }
        /** Define a box from left, top, right and bottom coordinates
            This box will have depth one (front=0 and back=1).
            @param	l	x value of left edge
            @param	t	y value of top edge
            @param	r	x value of right edge
            @param	b	y value of bottom edge
            @note Note that the left, top, and front edges are included 
 		        but the right, bottom and back ones are not.
        */
        Box( size_t l, size_t t, size_t r, size_t b ):
            left(l),
            top(t),   
            right(r),
            bottom(b),
            front(0),
            back(1)
        {
          	assert(right >= left && bottom >= top && back >= front);
        }
        /** Define a box from left, top, front, right, bottom and back
            coordinates.
            @param	l	x value of left edge
            @param	t	y value of top edge
            @param  ff  z value of front edge
            @param	r	x value of right edge
            @param	b	y value of bottom edge
            @param  bb  z value of back edge
            @note Note that the left, top, and front edges are included 
 		        but the right, bottom and back ones are not.
        */
        Box( size_t l, size_t t, size_t ff, size_t r, size_t b, size_t bb ):
            left(l),
            top(t),   
            right(r),
            bottom(b),
            front(ff),
            back(bb)
        {
          	assert(right >= left && bottom >= top && back >= front);
        }
            
        /// Return true if the other box is a part of this one
        bool contains(const Box &def) const
        {
            return (def.left >= left && def.top >= top && def.front >= front &&
				def.right <= right && def.bottom <= bottom && def.back <= back);
        }
            
        /// Get the width of this box
        size_t getWidth() const { return right-left; }
        /// Get the height of this box
        size_t getHeight() const { return bottom-top; }
        /// Get the depth of this box
        size_t getDepth() const { return back-front; }
    };


	/// Render window creation parameters.
	struct RenderWindowDescription
	{
		String				name;
		unsigned int		width;
		unsigned int		height;
		bool				useFullScreen;
		NameValuePairList	miscParams;
	};

	/// Render window creation parameters container.
	typedef vector<RenderWindowDescription>::type RenderWindowDescriptionList;

	/// Render window container.
	typedef vector<RenderWindow*>::type RenderWindowList;

	
	/** @} */
}

#endif
