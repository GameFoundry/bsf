/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

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
#include "CmGpuProgramParams.h"
#include "CmMatrix4.h"
#include "CmVector3.h"
#include "CmVector4.h"
#include "CmRenderSystemCapabilities.h"
#include "CmException.h"


namespace CamelotEngine
{

	//---------------------------------------------------------------------
	GpuProgramParameters::AutoConstantDefinition GpuProgramParameters::AutoConstantDictionary[] = {
		AutoConstantDefinition(ACT_WORLD_MATRIX,                  "world_matrix",                16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_WORLD_MATRIX,          "inverse_world_matrix",        16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_TRANSPOSE_WORLD_MATRIX,             "transpose_world_matrix",            16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_TRANSPOSE_WORLD_MATRIX, "inverse_transpose_world_matrix", 16, ET_REAL, ACDT_NONE),

		AutoConstantDefinition(ACT_WORLD_MATRIX_ARRAY_3x4,        "world_matrix_array_3x4",      12, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_WORLD_MATRIX_ARRAY,            "world_matrix_array",          16, ET_REAL, ACDT_NONE),

		AutoConstantDefinition(ACT_VIEW_MATRIX,                   "view_matrix",                 16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_VIEW_MATRIX,           "inverse_view_matrix",         16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_TRANSPOSE_VIEW_MATRIX,              "transpose_view_matrix",             16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_TRANSPOSE_VIEW_MATRIX,       "inverse_transpose_view_matrix",     16, ET_REAL, ACDT_NONE),

		AutoConstantDefinition(ACT_PROJECTION_MATRIX,             "projection_matrix",           16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_PROJECTION_MATRIX,          "inverse_projection_matrix",         16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_TRANSPOSE_PROJECTION_MATRIX,        "transpose_projection_matrix",       16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_TRANSPOSE_PROJECTION_MATRIX, "inverse_transpose_projection_matrix", 16, ET_REAL, ACDT_NONE),

		AutoConstantDefinition(ACT_VIEWPROJ_MATRIX,               "viewproj_matrix",             16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_VIEWPROJ_MATRIX,       "inverse_viewproj_matrix",     16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_TRANSPOSE_VIEWPROJ_MATRIX,          "transpose_viewproj_matrix",         16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_TRANSPOSE_VIEWPROJ_MATRIX,   "inverse_transpose_viewproj_matrix", 16, ET_REAL, ACDT_NONE),

		AutoConstantDefinition(ACT_WORLDVIEW_MATRIX,              "worldview_matrix",            16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_WORLDVIEW_MATRIX,      "inverse_worldview_matrix",    16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_TRANSPOSE_WORLDVIEW_MATRIX,         "transpose_worldview_matrix",        16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_TRANSPOSE_WORLDVIEW_MATRIX, "inverse_transpose_worldview_matrix", 16, ET_REAL, ACDT_NONE),

		AutoConstantDefinition(ACT_WORLDVIEWPROJ_MATRIX,          "worldviewproj_matrix",        16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_WORLDVIEWPROJ_MATRIX,       "inverse_worldviewproj_matrix",      16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_TRANSPOSE_WORLDVIEWPROJ_MATRIX,     "transpose_worldviewproj_matrix",    16, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_TRANSPOSE_WORLDVIEWPROJ_MATRIX, "inverse_transpose_worldviewproj_matrix", 16, ET_REAL, ACDT_NONE),

		AutoConstantDefinition(ACT_RENDER_TARGET_FLIPPING,          "render_target_flipping",         1, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_VERTEX_WINDING,          "vertex_winding",         1, ET_REAL, ACDT_NONE),

		AutoConstantDefinition(ACT_FOG_COLOUR,                    "fog_colour",                   4, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_FOG_PARAMS,                    "fog_params",                   4, ET_REAL, ACDT_NONE),

		AutoConstantDefinition(ACT_SURFACE_AMBIENT_COLOUR,          "surface_ambient_colour",           4, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_SURFACE_DIFFUSE_COLOUR,          "surface_diffuse_colour",           4, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_SURFACE_SPECULAR_COLOUR,         "surface_specular_colour",          4, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_SURFACE_EMISSIVE_COLOUR,         "surface_emissive_colour",          4, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_SURFACE_SHININESS,               "surface_shininess",                1, ET_REAL, ACDT_NONE),

		AutoConstantDefinition(ACT_LIGHT_COUNT,                   "light_count",                  1, ET_REAL, ACDT_NONE),

		AutoConstantDefinition(ACT_AMBIENT_LIGHT_COLOUR,          "ambient_light_colour",         4, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_LIGHT_DIFFUSE_COLOUR,          "light_diffuse_colour",         4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_SPECULAR_COLOUR,         "light_specular_colour",        4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_ATTENUATION,             "light_attenuation",            4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_SPOTLIGHT_PARAMS,              "spotlight_params",             4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_POSITION,                "light_position",               4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_POSITION_OBJECT_SPACE,   "light_position_object_space",  4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_POSITION_VIEW_SPACE,          "light_position_view_space",    4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_DIRECTION,               "light_direction",              4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_DIRECTION_OBJECT_SPACE,  "light_direction_object_space", 4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_DIRECTION_VIEW_SPACE,         "light_direction_view_space",   4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_DISTANCE_OBJECT_SPACE,   "light_distance_object_space",  1, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_POWER_SCALE,   		  "light_power",  1, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED, "light_diffuse_colour_power_scaled",         4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED, "light_specular_colour_power_scaled",        4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_DIFFUSE_COLOUR_ARRAY,          "light_diffuse_colour_array",         4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_SPECULAR_COLOUR_ARRAY,         "light_specular_colour_array",        4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY, "light_diffuse_colour_power_scaled_array",         4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED_ARRAY, "light_specular_colour_power_scaled_array",        4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_ATTENUATION_ARRAY,             "light_attenuation_array",            4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_POSITION_ARRAY,                "light_position_array",               4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_POSITION_OBJECT_SPACE_ARRAY,   "light_position_object_space_array",  4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_POSITION_VIEW_SPACE_ARRAY,          "light_position_view_space_array",    4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_DIRECTION_ARRAY,               "light_direction_array",              4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_DIRECTION_OBJECT_SPACE_ARRAY,  "light_direction_object_space_array", 4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_DIRECTION_VIEW_SPACE_ARRAY,         "light_direction_view_space_array",   4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_DISTANCE_OBJECT_SPACE_ARRAY,   "light_distance_object_space_array",  1, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_POWER_SCALE_ARRAY,   		  "light_power_array",  1, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_SPOTLIGHT_PARAMS_ARRAY,              "spotlight_params_array",             4, ET_REAL, ACDT_INT),

		AutoConstantDefinition(ACT_DERIVED_AMBIENT_LIGHT_COLOUR,    "derived_ambient_light_colour",     4, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_DERIVED_SCENE_COLOUR,            "derived_scene_colour",             4, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_DERIVED_LIGHT_DIFFUSE_COLOUR,    "derived_light_diffuse_colour",     4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_DERIVED_LIGHT_SPECULAR_COLOUR,   "derived_light_specular_colour",    4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_DERIVED_LIGHT_DIFFUSE_COLOUR_ARRAY,  "derived_light_diffuse_colour_array",   4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_DERIVED_LIGHT_SPECULAR_COLOUR_ARRAY, "derived_light_specular_colour_array",  4, ET_REAL, ACDT_INT),

		AutoConstantDefinition(ACT_LIGHT_NUMBER,   					  "light_number",  1, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LIGHT_CASTS_SHADOWS, 			  "light_casts_shadows",  1, ET_REAL, ACDT_INT),

		AutoConstantDefinition(ACT_SHADOW_EXTRUSION_DISTANCE,     "shadow_extrusion_distance",    1, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_CAMERA_POSITION,               "camera_position",              3, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_CAMERA_POSITION_OBJECT_SPACE,  "camera_position_object_space", 3, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_TEXTURE_VIEWPROJ_MATRIX,       "texture_viewproj_matrix",     16, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_TEXTURE_VIEWPROJ_MATRIX_ARRAY, "texture_viewproj_matrix_array", 16, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_TEXTURE_WORLDVIEWPROJ_MATRIX,  "texture_worldviewproj_matrix",16, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY, "texture_worldviewproj_matrix_array",16, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_SPOTLIGHT_VIEWPROJ_MATRIX,       "spotlight_viewproj_matrix",     16, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_SPOTLIGHT_VIEWPROJ_MATRIX_ARRAY, "spotlight_viewproj_matrix_array", 16, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_SPOTLIGHT_WORLDVIEWPROJ_MATRIX,  "spotlight_worldviewproj_matrix",16, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_CUSTOM,                        "custom",                       4, ET_REAL, ACDT_INT),  // *** needs to be tested
		AutoConstantDefinition(ACT_TIME,                               "time",                               1, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_TIME_0_X,                      "time_0_x",                     4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_COSTIME_0_X,                   "costime_0_x",                  4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_SINTIME_0_X,                   "sintime_0_x",                  4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_TANTIME_0_X,                   "tantime_0_x",                  4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_TIME_0_X_PACKED,               "time_0_x_packed",              4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_TIME_0_1,                      "time_0_1",                     4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_COSTIME_0_1,                   "costime_0_1",                  4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_SINTIME_0_1,                   "sintime_0_1",                  4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_TANTIME_0_1,                   "tantime_0_1",                  4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_TIME_0_1_PACKED,               "time_0_1_packed",              4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_TIME_0_2PI,                    "time_0_2pi",                   4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_COSTIME_0_2PI,                 "costime_0_2pi",                4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_SINTIME_0_2PI,                 "sintime_0_2pi",                4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_TANTIME_0_2PI,                 "tantime_0_2pi",                4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_TIME_0_2PI_PACKED,             "time_0_2pi_packed",            4, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_FRAME_TIME,                    "frame_time",                   1, ET_REAL, ACDT_REAL),
		AutoConstantDefinition(ACT_FPS,                           "fps",                          1, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_VIEWPORT_WIDTH,                "viewport_width",               1, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_VIEWPORT_HEIGHT,               "viewport_height",              1, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_VIEWPORT_WIDTH,        "inverse_viewport_width",       1, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_INVERSE_VIEWPORT_HEIGHT,       "inverse_viewport_height",      1, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_VIEWPORT_SIZE,                 "viewport_size",                4, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_VIEW_DIRECTION,                "view_direction",               3, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_VIEW_SIDE_VECTOR,              "view_side_vector",             3, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_VIEW_UP_VECTOR,                "view_up_vector",               3, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_FOV,                           "fov",                          1, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_NEAR_CLIP_DISTANCE,            "near_clip_distance",           1, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_FAR_CLIP_DISTANCE,             "far_clip_distance",            1, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_PASS_NUMBER,                        "pass_number",                        1, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_PASS_ITERATION_NUMBER,              "pass_iteration_number",              1, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_ANIMATION_PARAMETRIC,               "animation_parametric",               4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_TEXEL_OFFSETS,               "texel_offsets",				  4, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_SCENE_DEPTH_RANGE,           "scene_depth_range",			  4, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_SHADOW_SCENE_DEPTH_RANGE,    "shadow_scene_depth_range",		  4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_SHADOW_COLOUR,				"shadow_colour",				  4, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_TEXTURE_SIZE,                "texture_size",                   4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_INVERSE_TEXTURE_SIZE,        "inverse_texture_size",           4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_PACKED_TEXTURE_SIZE,         "packed_texture_size",            4, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_TEXTURE_MATRIX,  "texture_matrix", 16, ET_REAL, ACDT_INT),
		AutoConstantDefinition(ACT_LOD_CAMERA_POSITION,               "lod_camera_position",              3, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_LOD_CAMERA_POSITION_OBJECT_SPACE,  "lod_camera_position_object_space", 3, ET_REAL, ACDT_NONE),
		AutoConstantDefinition(ACT_LIGHT_CUSTOM,	"light_custom", 4, ET_REAL, ACDT_INT)
	};

	bool GpuNamedConstants::msGenerateAllConstantDefinitionArrayEntries = false;

	//---------------------------------------------------------------------
	void GpuNamedConstants::generateConstantDefinitionArrayEntries(
		const String& paramName, const GpuConstantDefinition& baseDef)
	{
		// Copy definition for use with arrays
		GpuConstantDefinition arrayDef = baseDef;
		arrayDef.arraySize = 1;
		String arrayName;

		// Add parameters for array accessors
		// [0] will refer to the same location, [1+] will increment
		// only populate others individually up to 16 array slots so as not to get out of hand,
		// unless the system has been explicitly configured to allow all the parameters to be added

		// paramName[0] version will always exist 
		size_t maxArrayIndex = 1;
		if (baseDef.arraySize <= 16 || msGenerateAllConstantDefinitionArrayEntries)
			maxArrayIndex = baseDef.arraySize;

		for (size_t i = 0; i < maxArrayIndex; i++)
		{
			arrayName = paramName + "[" + toString(i) + "]";
			map.insert(GpuConstantDefinitionMap::value_type(arrayName, arrayDef));
			// increment location
			arrayDef.physicalIndex += arrayDef.elementSize;
		}
		// note no increment of buffer sizes since this is shared with main array def

	}

	//---------------------------------------------------------------------
	bool GpuNamedConstants::getGenerateAllConstantDefinitionArrayEntries()
	{
		return msGenerateAllConstantDefinitionArrayEntries;
	}

	//---------------------------------------------------------------------
	void GpuNamedConstants::setGenerateAllConstantDefinitionArrayEntries(bool generateAll)
	{
		msGenerateAllConstantDefinitionArrayEntries = generateAll;
	}    
	//-----------------------------------------------------------------------------
	//      GpuSharedParameters Methods
	//-----------------------------------------------------------------------------
	GpuSharedParameters::GpuSharedParameters(const String& name)
		:mName(name)
		// TODO PORT - Commented out because I don't have root in port. But I dont think this will be needed in final version
		/*, mFrameLastUpdated(Root::getSingleton().getNextFrameNumber())*/
		, mFrameLastUpdated(0)
		, mVersion(0)
	{

	}
	//---------------------------------------------------------------------
	GpuSharedParameters::~GpuSharedParameters()
	{

	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::addConstantDefinition(const String& name, GpuConstantType constType, size_t arraySize)
	{
		if (mNamedConstants.map.find(name) != mNamedConstants.map.end())
		{
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
				"Constant entry with name '" + name + "' already exists. ", 
				"GpuSharedParameters::addConstantDefinition");
		}
		GpuConstantDefinition def;
		def.arraySize = arraySize;
		def.constType = constType;
		// for compatibility we do not pad values to multiples of 4
		// when it comes to arrays, user is responsible for creating matching defs
		def.elementSize = GpuConstantDefinition::getElementSize(constType, false);

		// not used
		def.logicalIndex = 0;
		def.variability = (UINT16)GPV_GLOBAL;

		if (def.isFloat())
		{
			def.physicalIndex = mFloatConstants.size();
			mFloatConstants.resize(mFloatConstants.size() + def.arraySize * def.elementSize);
		}
		else
		{
			def.physicalIndex = mIntConstants.size();
			mIntConstants.resize(mIntConstants.size() + def.arraySize * def.elementSize);
		}

		mNamedConstants.map[name] = def;

		++mVersion;
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::removeConstantDefinition(const String& name)
	{
		GpuConstantDefinitionMap::iterator i = mNamedConstants.map.find(name);
		if (i != mNamedConstants.map.end())
		{
			GpuConstantDefinition& def = i->second;
			bool isFloat = def.isFloat();
			size_t numElems = def.elementSize * def.arraySize;

			for (GpuConstantDefinitionMap::iterator j = mNamedConstants.map.begin();
				j != mNamedConstants.map.end(); ++j)
			{
				GpuConstantDefinition& otherDef = j->second;
				bool otherIsFloat = otherDef.isFloat();

				// same type, and comes after in the buffer
				if ( ((isFloat && otherIsFloat) || (!isFloat && !otherIsFloat)) && 
					otherDef.physicalIndex > def.physicalIndex)
				{
					// adjust index
					otherDef.physicalIndex -= numElems;
				}
			}

			// remove floats and reduce buffer
			if (isFloat)
			{
				mNamedConstants.floatBufferSize -= numElems;

				FloatConstantList::iterator beg = mFloatConstants.begin();
				std::advance(beg, def.physicalIndex);
				FloatConstantList::iterator en = beg;
				std::advance(en, numElems);
				mFloatConstants.erase(beg, en);
			}
			else
			{
				mNamedConstants.intBufferSize -= numElems;

				IntConstantList::iterator beg = mIntConstants.begin();
				std::advance(beg, def.physicalIndex);
				IntConstantList::iterator en = beg;
				std::advance(en, numElems);
				mIntConstants.erase(beg, en);

			}

			++mVersion;
			
		}

	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::removeAllConstantDefinitions()
	{
		mNamedConstants.map.clear();
		mNamedConstants.floatBufferSize = 0;
		mNamedConstants.intBufferSize = 0;
		mFloatConstants.clear();
		mIntConstants.clear();
	}
	//---------------------------------------------------------------------
	GpuConstantDefinitionIterator GpuSharedParameters::getConstantDefinitionIterator(void) const
	{
		return mNamedConstants.map.begin();
	}
	//---------------------------------------------------------------------
	const GpuConstantDefinition& GpuSharedParameters::getConstantDefinition(const String& name) const
	{
		GpuConstantDefinitionMap::const_iterator i = mNamedConstants.map.find(name);
		if (i == mNamedConstants.map.end())
		{
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
				"Constant entry with name '" + name + "' does not exist. ", 
				"GpuSharedParameters::getConstantDefinition");
		}
		return i->second;
	}
	//---------------------------------------------------------------------
	const GpuNamedConstants& GpuSharedParameters::getConstantDefinitions() const
	{
		return mNamedConstants;
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, float val)
	{
		setNamedConstant(name, &val, 1);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, int val)
	{
		setNamedConstant(name, &val, 1);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const Vector4& vec)
	{
		setNamedConstant(name, vec.ptr(), 4);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const Vector3& vec)
	{
		setNamedConstant(name, vec.ptr(), 3);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const Matrix4& m)
	{
		setNamedConstant(name, m[0], 16);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const Matrix4* m, size_t numEntries)
	{
		setNamedConstant(name, m[0][0], 16 * numEntries);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const float *val, size_t count)
	{
		GpuConstantDefinitionMap::const_iterator i = mNamedConstants.map.find(name);
		if (i != mNamedConstants.map.end())
		{
			const GpuConstantDefinition& def = i->second;
			memcpy(&mFloatConstants[def.physicalIndex], val, 
				sizeof(float) * std::min(count, def.elementSize * def.arraySize));
		}

		_markDirty();

	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const double *val, size_t count)
	{
		GpuConstantDefinitionMap::const_iterator i = mNamedConstants.map.find(name);
		if (i != mNamedConstants.map.end())
		{
			const GpuConstantDefinition& def = i->second;

			count = std::min(count, def.elementSize * def.arraySize);
			const double* src = val;
			float* dst = &mFloatConstants[def.physicalIndex];
			for (size_t v = 0; v < count; ++v)
			{
				*dst++ = static_cast<float>(*src++);
			}
		}

		_markDirty();

	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const ColourValue& colour)
	{
		setNamedConstant(name, colour.ptr(), 4);
	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::setNamedConstant(const String& name, const int *val, size_t count)
	{
		GpuConstantDefinitionMap::const_iterator i = mNamedConstants.map.find(name);
		if (i != mNamedConstants.map.end())
		{
			const GpuConstantDefinition& def = i->second;
			memcpy(&mIntConstants[def.physicalIndex], val, 
				sizeof(int) * std::min(count, def.elementSize * def.arraySize));
		}

		_markDirty();

	}
	//---------------------------------------------------------------------
	void GpuSharedParameters::_markDirty()
	{
		// TODO PORT - Don't have access to Root in port. Set it to 0 for now. Will probably be able to remove this completely
		mFrameLastUpdated = 0;
		/*mFrameLastUpdated = Root::getSingleton().getNextFrameNumber();*/
	}

	//-----------------------------------------------------------------------------
	//      GpuSharedParametersUsage Methods
	//-----------------------------------------------------------------------------
	GpuSharedParametersUsage::GpuSharedParametersUsage(GpuSharedParametersPtr sharedParams, 
		GpuProgramParameters* params)
		: mSharedParams(sharedParams)
		, mParams(params)
	{
		initCopyData();
	}
	//---------------------------------------------------------------------
	void GpuSharedParametersUsage::initCopyData()
	{

		mCopyDataList.clear();

		const GpuConstantDefinitionMap& sharedmap = mSharedParams->getConstantDefinitions().map;
		for (GpuConstantDefinitionMap::const_iterator i = sharedmap.begin(); i != sharedmap.end(); ++i)
		{
			const String& pName = i->first;
			const GpuConstantDefinition& shareddef = i->second;

			const GpuConstantDefinition* instdef = mParams->_findNamedConstantDefinition(pName, false);
			if (instdef)
			{
				// Check that the definitions are the same 
				if (instdef->constType == shareddef.constType && 
					instdef->arraySize == shareddef.arraySize)
				{
					CopyDataEntry e;
					e.srcDefinition = &shareddef;
					e.dstDefinition = instdef;
					mCopyDataList.push_back(e);
				}
			}

		}

		mCopyDataVersion = mSharedParams->getVersion();

	}
	//---------------------------------------------------------------------
	void GpuSharedParametersUsage::_copySharedParamsToTargetParams()
	{
		// check copy data version
		if (mCopyDataVersion != mSharedParams->getVersion())
			initCopyData();

		for (CopyDataList::iterator i = mCopyDataList.begin(); i != mCopyDataList.end(); ++i)
		{
			CopyDataEntry& e = *i;

			if (e.dstDefinition->isFloat())
			{	
				const float* pSrc = mSharedParams->getFloatPointer(e.srcDefinition->physicalIndex);
				float* pDst = mParams->getFloatPointer(e.dstDefinition->physicalIndex);

				// Deal with matrix transposition here!!!
				// transposition is specific to the dest param set, shared params don't do it
				if (mParams->getTransposeMatrices() && e.dstDefinition->constType == GCT_MATRIX_4X4)
				{
					for (int row = 0; row < 4; ++row)
						for (int col = 0; col < 4; ++col)
							pDst[row * 4 + col] = pSrc[col * 4 + row];
				}
				else
				{
					if (e.dstDefinition->elementSize == e.srcDefinition->elementSize)
					{
						// simple copy
						memcpy(pDst, pSrc, sizeof(float) * e.dstDefinition->elementSize * e.dstDefinition->arraySize);
					}
					else
					{
						// target params may be padded to 4 elements, shared params are packed
						assert(e.dstDefinition->elementSize % 4 == 0);
						size_t iterations = e.dstDefinition->elementSize / 4 
							* e.dstDefinition->arraySize;
						size_t valsPerIteration = e.srcDefinition->elementSize / iterations;
						for (size_t l = 0; l < iterations; ++l)
						{
							memcpy(pDst, pSrc, sizeof(float) * valsPerIteration);
							pSrc += valsPerIteration;
							pDst += 4;
						}
					}
				}
			}
			else
			{
				const int* pSrc = mSharedParams->getIntPointer(e.srcDefinition->physicalIndex);
				int* pDst = mParams->getIntPointer(e.dstDefinition->physicalIndex);

				if (e.dstDefinition->elementSize == e.srcDefinition->elementSize)
				{
					// simple copy
					memcpy(pDst, pSrc, sizeof(int) * e.dstDefinition->elementSize * e.dstDefinition->arraySize);
				}
				else
				{
					// target params may be padded to 4 elements, shared params are packed
					assert(e.dstDefinition->elementSize % 4 == 0);
					size_t iterations = (e.dstDefinition->elementSize / 4)
						* e.dstDefinition->arraySize;
					size_t valsPerIteration = e.srcDefinition->elementSize / iterations;
					for (size_t l = 0; l < iterations; ++l)
					{
						memcpy(pDst, pSrc, sizeof(int) * valsPerIteration);
						pSrc += valsPerIteration;
						pDst += 4;
					}
				}
			}
		}
	}



	//-----------------------------------------------------------------------------
	//      GpuProgramParameters Methods
	//-----------------------------------------------------------------------------
	GpuProgramParameters::GpuProgramParameters() :
		mCombinedVariability(GPV_GLOBAL)
		, mTransposeMatrices(false)
		, mIgnoreMissingParams(false)
		, mActivePassIterationIndex(std::numeric_limits<size_t>::max())	
	{
	}
	//-----------------------------------------------------------------------------

	GpuProgramParameters::GpuProgramParameters(const GpuProgramParameters& oth)
	{
		*this = oth;
	}

	//-----------------------------------------------------------------------------
	GpuProgramParameters& GpuProgramParameters::operator=(const GpuProgramParameters& oth)
	{
		// let compiler perform shallow copies of structures 
		// AutoConstantEntry, RealConstantEntry, IntConstantEntry
		mFloatConstants = oth.mFloatConstants;
		mIntConstants  = oth.mIntConstants;
		mAutoConstants = oth.mAutoConstants;
		mFloatLogicalToPhysical = oth.mFloatLogicalToPhysical;
		mIntLogicalToPhysical = oth.mIntLogicalToPhysical;
		mNamedConstants = oth.mNamedConstants;
		copySharedParamSetUsage(oth.mSharedParamSets);

		mCombinedVariability = oth.mCombinedVariability;
		mTransposeMatrices = oth.mTransposeMatrices;
		mIgnoreMissingParams  = oth.mIgnoreMissingParams;
		mActivePassIterationIndex = oth.mActivePassIterationIndex;

		return *this;
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::copySharedParamSetUsage(const GpuSharedParamUsageList& srcList)
	{
		mSharedParamSets.clear();
		for (GpuSharedParamUsageList::const_iterator i = srcList.begin(); i != srcList.end(); ++i)
		{
			mSharedParamSets.push_back(GpuSharedParametersUsage(i->getSharedParams(), this));
		}

	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::_setNamedConstants(
		const GpuNamedConstantsPtr& namedConstants)
	{
		mNamedConstants = namedConstants;

		// Determine any extension to local buffers

		// Size and reset buffer (fill with zero to make comparison later ok)
		if (namedConstants->floatBufferSize > mFloatConstants.size())
		{
			mFloatConstants.insert(mFloatConstants.end(), 
				namedConstants->floatBufferSize - mFloatConstants.size(), 0.0f);
		}
		if (namedConstants->intBufferSize > mIntConstants.size())
		{
			mIntConstants.insert(mIntConstants.end(), 
				namedConstants->intBufferSize - mIntConstants.size(), 0);
		}
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::_setLogicalIndexes(
		const GpuLogicalBufferStructPtr& floatIndexMap, 
		const GpuLogicalBufferStructPtr& intIndexMap)
	{
		mFloatLogicalToPhysical = floatIndexMap;
		mIntLogicalToPhysical = intIndexMap;

		// resize the internal buffers
		// Note that these will only contain something after the first parameter
		// set has set some parameters

		// Size and reset buffer (fill with zero to make comparison later ok)
		if ((floatIndexMap != nullptr) && floatIndexMap->bufferSize > mFloatConstants.size())
		{
			mFloatConstants.insert(mFloatConstants.end(), 
				floatIndexMap->bufferSize - mFloatConstants.size(), 0.0f);
		}
		if ((intIndexMap != nullptr) &&  intIndexMap->bufferSize > mIntConstants.size())
		{
			mIntConstants.insert(mIntConstants.end(), 
				intIndexMap->bufferSize - mIntConstants.size(), 0);
		}

	}
	//---------------------------------------------------------------------()
	void GpuProgramParameters::setConstant(size_t index, const Vector4& vec)
	{
		setConstant(index, vec.ptr(), 1);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, float val)
	{
		setConstant(index, Vector4(val, 0.0f, 0.0f, 0.0f));
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, const Vector3& vec)
	{
		setConstant(index, Vector4(vec.x, vec.y, vec.z, 1.0f));
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, const Matrix4& m)
	{
		// set as 4x 4-element floats
		if (mTransposeMatrices)
		{
			Matrix4 t = m.transpose();
			GpuProgramParameters::setConstant(index, t[0], 4);
		}
		else
		{
			GpuProgramParameters::setConstant(index, m[0], 4);
		}

	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, const Matrix4* pMatrix, 
		size_t numEntries)
	{
		if (mTransposeMatrices)
		{
			for (size_t i = 0; i < numEntries; ++i)
			{
				Matrix4 t = pMatrix[i].transpose();
				GpuProgramParameters::setConstant(index, t[0], 4);
				index += 4;
			}
		}
		else
		{
			GpuProgramParameters::setConstant(index, pMatrix[0][0], 4 * numEntries);
		}

	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, const ColourValue& colour)
	{
		setConstant(index, colour.ptr(), 1);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, const float *val, size_t count)
	{
		// Raw buffer size is 4x count
		size_t rawCount = count * 4;
		// get physical index
		assert((mFloatLogicalToPhysical != nullptr) && "GpuProgram hasn't set up the logical -> physical map!");

		size_t physicalIndex = _getFloatConstantPhysicalIndex(index, rawCount, GPV_GLOBAL);

		// Copy 
		_writeRawConstants(physicalIndex, val, rawCount);

	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, const double *val, size_t count)
	{
		// Raw buffer size is 4x count
		size_t rawCount = count * 4;
		// get physical index
		assert((mFloatLogicalToPhysical != nullptr) && "GpuProgram hasn't set up the logical -> physical map!");

		size_t physicalIndex = _getFloatConstantPhysicalIndex(index, rawCount, GPV_GLOBAL);
		assert(physicalIndex + rawCount <= mFloatConstants.size());
		// Copy manually since cast required
		for (size_t i = 0; i < rawCount; ++i)
		{
			mFloatConstants[physicalIndex + i] = 
				static_cast<float>(val[i]);
		}

	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setConstant(size_t index, const int *val, size_t count)
	{
		// Raw buffer size is 4x count
		size_t rawCount = count * 4;
		// get physical index
		assert((mIntLogicalToPhysical != nullptr) && "GpuProgram hasn't set up the logical -> physical map!");

		size_t physicalIndex = _getIntConstantPhysicalIndex(index, rawCount, GPV_GLOBAL);
		// Copy 
		_writeRawConstants(physicalIndex, val, rawCount);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, const Vector4& vec,
		size_t count)
	{
		// remember, raw content access uses raw float count rather than float4
		// write either the number requested (for packed types) or up to 4
		_writeRawConstants(physicalIndex, vec.ptr(), std::min(count, (size_t)4));
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, float val)
	{
		_writeRawConstants(physicalIndex, &val, 1);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, int val)
	{
		_writeRawConstants(physicalIndex, &val, 1);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, const Vector3& vec)
	{
		_writeRawConstants(physicalIndex, vec.ptr(), 3);		
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, const Matrix4& m,size_t elementCount)
	{

		// remember, raw content access uses raw float count rather than float4
		if (mTransposeMatrices)
		{
			Matrix4 t = m.transpose();
			_writeRawConstants(physicalIndex, t[0], elementCount>16?16:elementCount);
		}
		else
		{
			_writeRawConstants(physicalIndex, m[0], elementCount>16?16:elementCount);
		}

	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, const Matrix4* pMatrix, size_t numEntries)
	{
		// remember, raw content access uses raw float count rather than float4
		if (mTransposeMatrices)
		{
			for (size_t i = 0; i < numEntries; ++i)
			{
				Matrix4 t = pMatrix[i].transpose();
				_writeRawConstants(physicalIndex, t[0], 16);
				physicalIndex += 16;
			}
		}
		else
		{
			_writeRawConstants(physicalIndex, pMatrix[0][0], 16 * numEntries);
		}


	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstant(size_t physicalIndex, 
		const ColourValue& colour, size_t count)
	{
		// write either the number requested (for packed types) or up to 4
		_writeRawConstants(physicalIndex, colour.ptr(), std::min(count, (size_t)4));
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstants(size_t physicalIndex, const double* val, size_t count)
	{
		assert(physicalIndex + count <= mFloatConstants.size());
		for (size_t i = 0; i < count; ++i)
		{
			mFloatConstants[physicalIndex+i] = static_cast<float>(val[i]);
		}
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstants(size_t physicalIndex, const float* val, size_t count)
	{
		assert(physicalIndex + count <= mFloatConstants.size());
		memcpy(&mFloatConstants[physicalIndex], val, sizeof(float) * count);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_writeRawConstants(size_t physicalIndex, const int* val, size_t count)
	{
		assert(physicalIndex + count <= mIntConstants.size());
		memcpy(&mIntConstants[physicalIndex], val, sizeof(int) * count);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_readRawConstants(size_t physicalIndex, size_t count, float* dest)
	{
		assert(physicalIndex + count <= mFloatConstants.size());
		memcpy(dest, &mFloatConstants[physicalIndex], sizeof(float) * count);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_readRawConstants(size_t physicalIndex, size_t count, int* dest)
	{
		assert(physicalIndex + count <= mIntConstants.size());
		memcpy(dest, &mIntConstants[physicalIndex], sizeof(int) * count);
	}
	//---------------------------------------------------------------------
	UINT16 GpuProgramParameters::deriveVariability(GpuProgramParameters::AutoConstantType act)
	{
		switch(act)
		{
		case ACT_VIEW_MATRIX:
		case ACT_INVERSE_VIEW_MATRIX:
		case ACT_TRANSPOSE_VIEW_MATRIX:
		case ACT_INVERSE_TRANSPOSE_VIEW_MATRIX:
		case ACT_PROJECTION_MATRIX:
		case ACT_INVERSE_PROJECTION_MATRIX:
		case ACT_TRANSPOSE_PROJECTION_MATRIX:
		case ACT_INVERSE_TRANSPOSE_PROJECTION_MATRIX:
		case ACT_VIEWPROJ_MATRIX:
		case ACT_INVERSE_VIEWPROJ_MATRIX:
		case ACT_TRANSPOSE_VIEWPROJ_MATRIX:
		case ACT_INVERSE_TRANSPOSE_VIEWPROJ_MATRIX:
		case ACT_RENDER_TARGET_FLIPPING:
		case ACT_VERTEX_WINDING:
		case ACT_AMBIENT_LIGHT_COLOUR: 
		case ACT_DERIVED_AMBIENT_LIGHT_COLOUR:
		case ACT_DERIVED_SCENE_COLOUR:
		case ACT_FOG_COLOUR:
		case ACT_FOG_PARAMS:
		case ACT_SURFACE_AMBIENT_COLOUR:
		case ACT_SURFACE_DIFFUSE_COLOUR:
		case ACT_SURFACE_SPECULAR_COLOUR:
		case ACT_SURFACE_EMISSIVE_COLOUR:
		case ACT_SURFACE_SHININESS:
		case ACT_CAMERA_POSITION:
		case ACT_TIME:
		case ACT_TIME_0_X:
		case ACT_COSTIME_0_X:
		case ACT_SINTIME_0_X:
		case ACT_TANTIME_0_X:
		case ACT_TIME_0_X_PACKED:
		case ACT_TIME_0_1:
		case ACT_COSTIME_0_1:
		case ACT_SINTIME_0_1:
		case ACT_TANTIME_0_1:
		case ACT_TIME_0_1_PACKED:
		case ACT_TIME_0_2PI:
		case ACT_COSTIME_0_2PI:
		case ACT_SINTIME_0_2PI:
		case ACT_TANTIME_0_2PI:
		case ACT_TIME_0_2PI_PACKED:
		case ACT_FRAME_TIME:
		case ACT_FPS:
		case ACT_VIEWPORT_WIDTH:
		case ACT_VIEWPORT_HEIGHT:
		case ACT_INVERSE_VIEWPORT_WIDTH:
		case ACT_INVERSE_VIEWPORT_HEIGHT:
		case ACT_VIEWPORT_SIZE:
		case ACT_TEXEL_OFFSETS:
		case ACT_TEXTURE_SIZE:
		case ACT_INVERSE_TEXTURE_SIZE:
		case ACT_PACKED_TEXTURE_SIZE:
		case ACT_SCENE_DEPTH_RANGE:
		case ACT_VIEW_DIRECTION:
		case ACT_VIEW_SIDE_VECTOR:
		case ACT_VIEW_UP_VECTOR:
		case ACT_FOV:
		case ACT_NEAR_CLIP_DISTANCE:
		case ACT_FAR_CLIP_DISTANCE:
		case ACT_PASS_NUMBER:
		case ACT_TEXTURE_MATRIX:
		case ACT_LOD_CAMERA_POSITION:

			return (UINT16)GPV_GLOBAL;

		case ACT_WORLD_MATRIX:
		case ACT_INVERSE_WORLD_MATRIX:
		case ACT_TRANSPOSE_WORLD_MATRIX:
		case ACT_INVERSE_TRANSPOSE_WORLD_MATRIX:
		case ACT_WORLD_MATRIX_ARRAY_3x4:
		case ACT_WORLD_MATRIX_ARRAY:
		case ACT_WORLDVIEW_MATRIX:
		case ACT_INVERSE_WORLDVIEW_MATRIX:
		case ACT_TRANSPOSE_WORLDVIEW_MATRIX:
		case ACT_INVERSE_TRANSPOSE_WORLDVIEW_MATRIX:
		case ACT_WORLDVIEWPROJ_MATRIX:
		case ACT_INVERSE_WORLDVIEWPROJ_MATRIX:
		case ACT_TRANSPOSE_WORLDVIEWPROJ_MATRIX:
		case ACT_INVERSE_TRANSPOSE_WORLDVIEWPROJ_MATRIX:
		case ACT_CAMERA_POSITION_OBJECT_SPACE:
		case ACT_LOD_CAMERA_POSITION_OBJECT_SPACE:
		case ACT_CUSTOM:
		case ACT_ANIMATION_PARAMETRIC:

			return (UINT16)GPV_PER_OBJECT;

		case ACT_LIGHT_POSITION_OBJECT_SPACE:
		case ACT_LIGHT_DIRECTION_OBJECT_SPACE:
		case ACT_LIGHT_DISTANCE_OBJECT_SPACE:
		case ACT_LIGHT_POSITION_OBJECT_SPACE_ARRAY:
		case ACT_LIGHT_DIRECTION_OBJECT_SPACE_ARRAY:
		case ACT_LIGHT_DISTANCE_OBJECT_SPACE_ARRAY:
		case ACT_TEXTURE_WORLDVIEWPROJ_MATRIX:
		case ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY:
		case ACT_SPOTLIGHT_WORLDVIEWPROJ_MATRIX:

			// These depend on BOTH lights and objects
			return ((UINT16)GPV_PER_OBJECT) | ((UINT16)GPV_LIGHTS);

		case ACT_LIGHT_COUNT:
		case ACT_LIGHT_DIFFUSE_COLOUR:
		case ACT_LIGHT_SPECULAR_COLOUR:
		case ACT_LIGHT_POSITION:
		case ACT_LIGHT_DIRECTION:
		case ACT_LIGHT_POSITION_VIEW_SPACE:
		case ACT_LIGHT_DIRECTION_VIEW_SPACE:
		case ACT_SHADOW_EXTRUSION_DISTANCE:
		case ACT_SHADOW_SCENE_DEPTH_RANGE:
		case ACT_SHADOW_COLOUR:
		case ACT_LIGHT_POWER_SCALE:
		case ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED:
		case ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED:
		case ACT_LIGHT_NUMBER:
		case ACT_LIGHT_CASTS_SHADOWS:
		case ACT_LIGHT_ATTENUATION:
		case ACT_SPOTLIGHT_PARAMS:
		case ACT_LIGHT_DIFFUSE_COLOUR_ARRAY:
		case ACT_LIGHT_SPECULAR_COLOUR_ARRAY:
		case ACT_LIGHT_DIFFUSE_COLOUR_POWER_SCALED_ARRAY:
		case ACT_LIGHT_SPECULAR_COLOUR_POWER_SCALED_ARRAY:
		case ACT_LIGHT_POSITION_ARRAY:
		case ACT_LIGHT_DIRECTION_ARRAY:
		case ACT_LIGHT_POSITION_VIEW_SPACE_ARRAY:
		case ACT_LIGHT_DIRECTION_VIEW_SPACE_ARRAY:
		case ACT_LIGHT_POWER_SCALE_ARRAY:
		case ACT_LIGHT_ATTENUATION_ARRAY:
		case ACT_SPOTLIGHT_PARAMS_ARRAY:
		case ACT_TEXTURE_VIEWPROJ_MATRIX:
		case ACT_TEXTURE_VIEWPROJ_MATRIX_ARRAY:
		case ACT_SPOTLIGHT_VIEWPROJ_MATRIX:
		case ACT_SPOTLIGHT_VIEWPROJ_MATRIX_ARRAY:
		case ACT_LIGHT_CUSTOM:

			return (UINT16)GPV_LIGHTS;

		case ACT_DERIVED_LIGHT_DIFFUSE_COLOUR:
		case ACT_DERIVED_LIGHT_SPECULAR_COLOUR:
		case ACT_DERIVED_LIGHT_DIFFUSE_COLOUR_ARRAY:
		case ACT_DERIVED_LIGHT_SPECULAR_COLOUR_ARRAY:

			return ((UINT16)GPV_GLOBAL | (UINT16)GPV_LIGHTS);

		case ACT_PASS_ITERATION_NUMBER:

			return (UINT16)GPV_PASS_ITERATION_NUMBER;

		default:
			return (UINT16)GPV_GLOBAL;
		};

	}
	//---------------------------------------------------------------------
	GpuLogicalIndexUse* GpuProgramParameters::_getFloatConstantLogicalIndexUse(
		size_t logicalIndex, size_t requestedSize, UINT16 variability)
	{
		if (mFloatLogicalToPhysical == nullptr)
			return 0;

		GpuLogicalIndexUse* indexUse = 0;
		CM_LOCK_MUTEX(mFloatLogicalToPhysical->mutex)

			GpuLogicalIndexUseMap::iterator logi = mFloatLogicalToPhysical->map.find(logicalIndex);
		if (logi == mFloatLogicalToPhysical->map.end())
		{
			if (requestedSize)
			{
				size_t physicalIndex = mFloatConstants.size();

				// Expand at buffer end
				mFloatConstants.insert(mFloatConstants.end(), requestedSize, 0.0f);

				// Record extended size for future GPU params re-using this information
				mFloatLogicalToPhysical->bufferSize = mFloatConstants.size();

				// low-level programs will not know about mapping ahead of time, so 
				// populate it. Other params objects will be able to just use this
				// accepted mapping since the constant structure will be the same

				// Set up a mapping for all items in the count
				size_t currPhys = physicalIndex;
				size_t count = requestedSize / 4;
				GpuLogicalIndexUseMap::iterator insertedIterator;

				for (size_t logicalNum = 0; logicalNum < count; ++logicalNum)
				{
					GpuLogicalIndexUseMap::iterator it = 
						mFloatLogicalToPhysical->map.insert(
						GpuLogicalIndexUseMap::value_type(
						logicalIndex + logicalNum, 
						GpuLogicalIndexUse(currPhys, requestedSize, variability))).first;
					currPhys += 4;

					if (logicalNum == 0)
						insertedIterator = it;
				}

				indexUse = &(insertedIterator->second);
			}
			else
			{
				// no match & ignore
				return 0;
			}

		}
		else
		{
			size_t physicalIndex = logi->second.physicalIndex;
			indexUse = &(logi->second);
			// check size
			if (logi->second.currentSize < requestedSize)
			{
				// init buffer entry wasn't big enough; could be a mistake on the part
				// of the original use, or perhaps a variable length we can't predict
				// until first actual runtime use e.g. world matrix array
				size_t insertCount = requestedSize - logi->second.currentSize;
				FloatConstantList::iterator insertPos = mFloatConstants.begin();
				std::advance(insertPos, physicalIndex);
				mFloatConstants.insert(insertPos, insertCount, 0.0f);
				// shift all physical positions after this one
				for (GpuLogicalIndexUseMap::iterator i = mFloatLogicalToPhysical->map.begin();
					i != mFloatLogicalToPhysical->map.end(); ++i)
				{
					if (i->second.physicalIndex > physicalIndex)
						i->second.physicalIndex += insertCount;
				}
				mFloatLogicalToPhysical->bufferSize += insertCount;
				for (AutoConstantList::iterator i = mAutoConstants.begin();
					i != mAutoConstants.end(); ++i)
				{
					if (i->physicalIndex > physicalIndex &&
						getAutoConstantDefinition(i->paramType)->elementType == ET_REAL)
					{
						i->physicalIndex += insertCount;
					}
				}
				if (mNamedConstants != nullptr)
				{
					for (GpuConstantDefinitionMap::iterator i = mNamedConstants->map.begin();
						i != mNamedConstants->map.end(); ++i)
					{
						if (i->second.isFloat() && i->second.physicalIndex > physicalIndex)
							i->second.physicalIndex += insertCount;
					}
					mNamedConstants->floatBufferSize += insertCount;
				}

				logi->second.currentSize += insertCount;
			}
		}

		if (indexUse)
			indexUse->variability = variability;

		return indexUse;

	}
	//---------------------------------------------------------------------()
	GpuLogicalIndexUse* GpuProgramParameters::_getIntConstantLogicalIndexUse(size_t logicalIndex, size_t requestedSize, UINT16 variability)
	{
		if (mIntLogicalToPhysical == nullptr)
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
			"This is not a low-level parameter parameter object",
			"GpuProgramParameters::_getIntConstantPhysicalIndex");

		GpuLogicalIndexUse* indexUse = 0;
		CM_LOCK_MUTEX(mIntLogicalToPhysical->mutex)

			GpuLogicalIndexUseMap::iterator logi = mIntLogicalToPhysical->map.find(logicalIndex);
		if (logi == mIntLogicalToPhysical->map.end())
		{
			if (requestedSize)
			{
				size_t physicalIndex = mIntConstants.size();

				// Expand at buffer end
				mIntConstants.insert(mIntConstants.end(), requestedSize, 0);

				// Record extended size for future GPU params re-using this information
				mIntLogicalToPhysical->bufferSize = mIntConstants.size();

				// low-level programs will not know about mapping ahead of time, so 
				// populate it. Other params objects will be able to just use this
				// accepted mapping since the constant structure will be the same

				// Set up a mapping for all items in the count
				size_t currPhys = physicalIndex;
				size_t count = requestedSize / 4;
				GpuLogicalIndexUseMap::iterator insertedIterator;
				for (size_t logicalNum = 0; logicalNum < count; ++logicalNum)
				{
					GpuLogicalIndexUseMap::iterator it = 
						mIntLogicalToPhysical->map.insert(
						GpuLogicalIndexUseMap::value_type(
						logicalIndex + logicalNum, 
						GpuLogicalIndexUse(currPhys, requestedSize, variability))).first;
					if (logicalNum == 0)
						insertedIterator = it;
					currPhys += 4;
				}
				indexUse = &(insertedIterator->second);

			}
			else
			{
				// no match
				return 0;
			}

		}
		else
		{
			size_t physicalIndex = logi->second.physicalIndex;
			indexUse = &(logi->second);

			// check size
			if (logi->second.currentSize < requestedSize)
			{
				// init buffer entry wasn't big enough; could be a mistake on the part
				// of the original use, or perhaps a variable length we can't predict
				// until first actual runtime use e.g. world matrix array
				size_t insertCount = requestedSize - logi->second.currentSize;
				IntConstantList::iterator insertPos = mIntConstants.begin();
				std::advance(insertPos, physicalIndex);
				mIntConstants.insert(insertPos, insertCount, 0);
				// shift all physical positions after this one
				for (GpuLogicalIndexUseMap::iterator i = mIntLogicalToPhysical->map.begin();
					i != mIntLogicalToPhysical->map.end(); ++i)
				{
					if (i->second.physicalIndex > physicalIndex)
						i->second.physicalIndex += insertCount;
				}
				mIntLogicalToPhysical->bufferSize += insertCount;
				for (AutoConstantList::iterator i = mAutoConstants.begin();
					i != mAutoConstants.end(); ++i)
				{
					if (i->physicalIndex > physicalIndex &&
						getAutoConstantDefinition(i->paramType)->elementType == ET_INT)
					{
						i->physicalIndex += insertCount;
					}
				}
				if (mNamedConstants != nullptr)
				{
					for (GpuConstantDefinitionMap::iterator i = mNamedConstants->map.begin();
						i != mNamedConstants->map.end(); ++i)
					{
						if (!i->second.isFloat() && i->second.physicalIndex > physicalIndex)
							i->second.physicalIndex += insertCount;
					}
					mNamedConstants->intBufferSize += insertCount;
				}

				logi->second.currentSize += insertCount;
			}
		}

		if (indexUse)
			indexUse->variability = variability;

		return indexUse;

	}
	//-----------------------------------------------------------------------------
	size_t GpuProgramParameters::_getFloatConstantPhysicalIndex(
		size_t logicalIndex, size_t requestedSize, UINT16 variability) 
	{
		GpuLogicalIndexUse* indexUse = _getFloatConstantLogicalIndexUse(logicalIndex, requestedSize, variability);
		return indexUse ? indexUse->physicalIndex : 0;
	}
	//-----------------------------------------------------------------------------
	size_t GpuProgramParameters::_getIntConstantPhysicalIndex(
		size_t logicalIndex, size_t requestedSize, UINT16 variability)
	{
		GpuLogicalIndexUse* indexUse = _getIntConstantLogicalIndexUse(logicalIndex, requestedSize, variability);
		return indexUse ? indexUse->physicalIndex : 0;
	}
	//-----------------------------------------------------------------------------
	size_t GpuProgramParameters::getFloatLogicalIndexForPhysicalIndex(size_t physicalIndex)
	{
		// perhaps build a reverse map of this sometime (shared in GpuProgram)
		for (GpuLogicalIndexUseMap::iterator i = mFloatLogicalToPhysical->map.begin();
			i != mFloatLogicalToPhysical->map.end(); ++i)
		{
			if (i->second.physicalIndex == physicalIndex)
				return i->first;
		}
		return std::numeric_limits<size_t>::max();

	}
	//-----------------------------------------------------------------------------
	size_t GpuProgramParameters::getIntLogicalIndexForPhysicalIndex(size_t physicalIndex)
	{
		// perhaps build a reverse map of this sometime (shared in GpuProgram)
		for (GpuLogicalIndexUseMap::iterator i = mIntLogicalToPhysical->map.begin();
			i != mIntLogicalToPhysical->map.end(); ++i)
		{
			if (i->second.physicalIndex == physicalIndex)
				return i->first;
		}
		return std::numeric_limits<size_t>::max();

	}
	//-----------------------------------------------------------------------------
	GpuConstantDefinitionIterator GpuProgramParameters::getConstantDefinitionIterator(void) const
	{
		if (mNamedConstants == nullptr)
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
			"This params object is not based on a program with named parameters.",
			"GpuProgramParameters::getConstantDefinitionIterator");

		return mNamedConstants->map.begin();

	}
	//-----------------------------------------------------------------------------
	const GpuNamedConstants& GpuProgramParameters::getConstantDefinitions() const
	{
		if (mNamedConstants == nullptr)
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
			"This params object is not based on a program with named parameters.",
			"GpuProgramParameters::getConstantDefinitionIterator");

		return *mNamedConstants;
	}
	//-----------------------------------------------------------------------------
	const GpuConstantDefinition& GpuProgramParameters::getConstantDefinition(const String& name) const
	{
		if (mNamedConstants == nullptr)
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
			"This params object is not based on a program with named parameters.",
			"GpuProgramParameters::getConstantDefinitionIterator");


		// locate, and throw exception if not found
		const GpuConstantDefinition* def = _findNamedConstantDefinition(name, true);

		return *def;

	}
	//-----------------------------------------------------------------------------
	const GpuConstantDefinition* 
		GpuProgramParameters::_findNamedConstantDefinition(const String& name, 
		bool throwExceptionIfNotFound) const
	{
		if (mNamedConstants == nullptr)
		{
			if (throwExceptionIfNotFound)
				OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
				"Named constants have not been initialised, perhaps a compile error.",
				"GpuProgramParameters::_findNamedConstantDefinition");
			return 0;
		}

		GpuConstantDefinitionMap::const_iterator i = mNamedConstants->map.find(name);
		if (i == mNamedConstants->map.end())
		{
			if (throwExceptionIfNotFound)
				OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
				"Parameter called " + name + " does not exist. ",
				"GpuProgramParameters::_findNamedConstantDefinition");
			return 0;
		}
		else
		{
			return &(i->second);
		}
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setAutoConstant(size_t index, AutoConstantType acType, size_t extraInfo)
	{
		// Get auto constant definition for sizing
		const AutoConstantDefinition* autoDef = getAutoConstantDefinition(acType);
		// round up to nearest multiple of 4
		size_t sz = autoDef->elementCount;
		if (sz % 4 > 0)
		{
			sz += 4 - (sz % 4);
		}

		GpuLogicalIndexUse* indexUse = _getFloatConstantLogicalIndexUse(index, sz, deriveVariability(acType));

		_setRawAutoConstant(indexUse->physicalIndex, acType, extraInfo, indexUse->variability, sz);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_setRawAutoConstant(size_t physicalIndex, 
		AutoConstantType acType, size_t extraInfo, UINT16 variability, size_t elementSize)
	{
		// update existing index if it exists
		bool found = false;
		for (AutoConstantList::iterator i = mAutoConstants.begin(); 
			i != mAutoConstants.end(); ++i)
		{
			if (i->physicalIndex == physicalIndex)
			{
				i->paramType = acType;
				i->data = extraInfo;
				i->elementCount = elementSize;
				i->variability = variability;
				found = true;
				break;
			}
		}
		if (!found)
			mAutoConstants.push_back(AutoConstantEntry(acType, physicalIndex, extraInfo, variability, elementSize));

		mCombinedVariability |= variability;


	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setAutoConstant(size_t index, AutoConstantType acType, UINT16 extraInfo1, UINT16 extraInfo2)
	{
		size_t extraInfo = (size_t)extraInfo1 | ((size_t)extraInfo2) << 16;

		// Get auto constant definition for sizing
		const AutoConstantDefinition* autoDef = getAutoConstantDefinition(acType);
		// round up to nearest multiple of 4
		size_t sz = autoDef->elementCount;
		if (sz % 4 > 0)
		{
			sz += 4 - (sz % 4);
		}

		GpuLogicalIndexUse* indexUse = _getFloatConstantLogicalIndexUse(index, sz, deriveVariability(acType));

		_setRawAutoConstant(indexUse->physicalIndex, acType, extraInfo, indexUse->variability, sz);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::_setRawAutoConstantReal(size_t physicalIndex, 
		AutoConstantType acType, float rData, UINT16 variability, size_t elementSize)
	{
		// update existing index if it exists
		bool found = false;
		for (AutoConstantList::iterator i = mAutoConstants.begin(); 
			i != mAutoConstants.end(); ++i)
		{
			if (i->physicalIndex == physicalIndex)
			{
				i->paramType = acType;
				i->fData = rData;
				i->elementCount = elementSize;
				i->variability = variability;
				found = true;
				break;
			}
		}
		if (!found)
			mAutoConstants.push_back(AutoConstantEntry(acType, physicalIndex, rData, variability, elementSize));

		mCombinedVariability |= variability;
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::clearAutoConstant(size_t index)
	{
		GpuLogicalIndexUse* indexUse = _getFloatConstantLogicalIndexUse(index, 0, GPV_GLOBAL);

		if (indexUse)
		{
			indexUse->variability = GPV_GLOBAL;
			size_t physicalIndex = indexUse->physicalIndex;
			// update existing index if it exists
			for (AutoConstantList::iterator i = mAutoConstants.begin(); 
				i != mAutoConstants.end(); ++i)
			{
				if (i->physicalIndex == physicalIndex)
				{
					mAutoConstants.erase(i);
					break;
				}
			}
		}
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::clearNamedAutoConstant(const String& name)
	{
		const GpuConstantDefinition* def = _findNamedConstantDefinition(name);
		if (def)
		{
			def->variability = GPV_GLOBAL;

			// Autos are always floating point
			if (def->isFloat())
			{
				for (AutoConstantList::iterator i = mAutoConstants.begin(); 
					i != mAutoConstants.end(); ++i)
				{
					if (i->physicalIndex == def->physicalIndex)
					{
						mAutoConstants.erase(i);
						break;
					}
				}
			}

		}
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::clearAutoConstants(void)
	{
		mAutoConstants.clear();
		mCombinedVariability = GPV_GLOBAL;
	}
	//-----------------------------------------------------------------------------
	GpuProgramParameters::AutoConstantIterator GpuProgramParameters::getAutoConstantIterator(void) const
	{
		return mAutoConstants.begin();
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setAutoConstantReal(size_t index, AutoConstantType acType, float rData)
	{
		// Get auto constant definition for sizing
		const AutoConstantDefinition* autoDef = getAutoConstantDefinition(acType);
		// round up to nearest multiple of 4
		size_t sz = autoDef->elementCount;
		if (sz % 4 > 0)
		{
			sz += 4 - (sz % 4);
		}

		GpuLogicalIndexUse* indexUse = _getFloatConstantLogicalIndexUse(index, sz, deriveVariability(acType));

		_setRawAutoConstantReal(indexUse->physicalIndex, acType, rData, indexUse->variability, sz);
	}
	//-----------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, float val)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, val);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, int val)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, val);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, const Vector4& vec)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, vec, def->elementSize);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, const Vector3& vec)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, vec);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, const Matrix4& m)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, m, def->elementSize);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, const Matrix4* m, 
		size_t numEntries)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, m, numEntries);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, 
		const float *val, size_t count, size_t multiple)
	{
		size_t rawCount = count * multiple;
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstants(def->physicalIndex, val, rawCount);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, 
		const double *val, size_t count, size_t multiple)
	{
		size_t rawCount = count * multiple;
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstants(def->physicalIndex, val, rawCount);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, const ColourValue& colour)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstant(def->physicalIndex, colour, def->elementSize);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstant(const String& name, 
		const int *val, size_t count, size_t multiple)
	{
		size_t rawCount = count * multiple;
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
			_writeRawConstants(def->physicalIndex, val, rawCount);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedAutoConstant(const String& name, 
		AutoConstantType acType, size_t extraInfo)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
		{
			def->variability = deriveVariability(acType);
			// make sure we also set variability on the logical index map
			GpuLogicalIndexUse* indexUse = _getFloatConstantLogicalIndexUse(def->logicalIndex, def->elementSize * def->arraySize, def->variability);
			if (indexUse)
				indexUse->variability = def->variability;

			_setRawAutoConstant(def->physicalIndex, acType, extraInfo, def->variability, def->elementSize);
		}

	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedAutoConstantReal(const String& name, 
		AutoConstantType acType, float rData)
	{
		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
		{
			def->variability = deriveVariability(acType);
			// make sure we also set variability on the logical index map
			GpuLogicalIndexUse* indexUse = _getFloatConstantLogicalIndexUse(def->logicalIndex, def->elementSize * def->arraySize, def->variability);
			if (indexUse)
				indexUse->variability = def->variability;
			_setRawAutoConstantReal(def->physicalIndex, acType, rData, def->variability, def->elementSize);
		}
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedAutoConstant(const String& name, 
		AutoConstantType acType, UINT16 extraInfo1, UINT16 extraInfo2)
	{
		size_t extraInfo = (size_t)extraInfo1 | ((size_t)extraInfo2) << 16;

		// look up, and throw an exception if we're not ignoring missing
		const GpuConstantDefinition* def = 
			_findNamedConstantDefinition(name, !mIgnoreMissingParams);
		if (def)
		{
			def->variability = deriveVariability(acType);
			// make sure we also set variability on the logical index map
			GpuLogicalIndexUse* indexUse = _getFloatConstantLogicalIndexUse(def->logicalIndex, def->elementSize * def->arraySize, def->variability);
			if (indexUse)
				indexUse->variability = def->variability;

			_setRawAutoConstant(def->physicalIndex, acType, extraInfo, def->variability, def->elementSize);
		}

	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setConstantFromTime(size_t index, float factor)
	{
		setAutoConstantReal(index, ACT_TIME, factor);
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::setNamedConstantFromTime(const String& name, float factor)
	{
		setNamedAutoConstantReal(name, ACT_TIME, factor);
	}
	//---------------------------------------------------------------------------
	GpuProgramParameters::AutoConstantEntry* GpuProgramParameters::getAutoConstantEntry(const size_t index)
	{
		if (index < mAutoConstants.size())
		{
			return &(mAutoConstants[index]);
		}
		else
		{
			return NULL;
		}
	}
	//---------------------------------------------------------------------------
	const GpuProgramParameters::AutoConstantEntry* 
		GpuProgramParameters::findFloatAutoConstantEntry(size_t logicalIndex)
	{
		if (mFloatLogicalToPhysical == nullptr)
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
			"This is not a low-level parameter parameter object",
			"GpuProgramParameters::findFloatAutoConstantEntry");

		return _findRawAutoConstantEntryFloat(
			_getFloatConstantPhysicalIndex(logicalIndex, 0, GPV_GLOBAL));

	}
	//---------------------------------------------------------------------------
	const GpuProgramParameters::AutoConstantEntry* 
		GpuProgramParameters::findIntAutoConstantEntry(size_t logicalIndex)
	{
		if (mIntLogicalToPhysical == nullptr)
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
			"This is not a low-level parameter parameter object",
			"GpuProgramParameters::findIntAutoConstantEntry");

		return _findRawAutoConstantEntryInt(
			_getIntConstantPhysicalIndex(logicalIndex, 0, GPV_GLOBAL));


	}
	//---------------------------------------------------------------------------
	const GpuProgramParameters::AutoConstantEntry* 
		GpuProgramParameters::findAutoConstantEntry(const String& paramName)
	{
		if (mNamedConstants == nullptr)
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
			"This params object is not based on a program with named parameters.",
			"GpuProgramParameters::findAutoConstantEntry");

		const GpuConstantDefinition& def = getConstantDefinition(paramName);
		if (def.isFloat())
		{
			return _findRawAutoConstantEntryFloat(def.physicalIndex);
		}
		else
		{
			return _findRawAutoConstantEntryInt(def.physicalIndex);
		}
	}
	//---------------------------------------------------------------------------
	const GpuProgramParameters::AutoConstantEntry* 
		GpuProgramParameters::_findRawAutoConstantEntryFloat(size_t physicalIndex)
	{
		for(AutoConstantList::iterator i = mAutoConstants.begin();
			i != mAutoConstants.end(); ++i)
		{
			AutoConstantEntry& ac = *i;
			// should check that auto is float and not int so that physicalIndex
			// doesn't have any ambiguity
			// However, all autos are float I think so no need
			if (ac.physicalIndex == physicalIndex)
				return &ac;
		}

		return 0;

	}
	//---------------------------------------------------------------------------
	const GpuProgramParameters::AutoConstantEntry* 
		GpuProgramParameters::_findRawAutoConstantEntryInt(size_t physicalIndex)
	{
		// No autos are float?
		return 0;
	}
	//---------------------------------------------------------------------------
	void GpuProgramParameters::copyConstantsFrom(const GpuProgramParameters& source)
	{
		// Pull buffers & auto constant list over directly
		mFloatConstants = source.getFloatConstantList();
		mIntConstants = source.getIntConstantList();
		mAutoConstants = source.getAutoConstantList();
		mCombinedVariability = source.mCombinedVariability;
		copySharedParamSetUsage(source.mSharedParamSets);
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::copyMatchingNamedConstantsFrom(const GpuProgramParameters& source)
	{
		if ((mNamedConstants != nullptr) && (source.mNamedConstants != nullptr))
		{
			std::map<size_t, String> srcToDestNamedMap;
			for (GpuConstantDefinitionMap::const_iterator i = source.mNamedConstants->map.begin(); 
				i != source.mNamedConstants->map.end(); ++i)
			{
				const String& paramName = i->first;
				const GpuConstantDefinition& olddef = i->second;
				const GpuConstantDefinition* newdef = _findNamedConstantDefinition(paramName, false);
				if (newdef)
				{
					// Copy data across, based on smallest common definition size
					size_t srcsz = olddef.elementSize * olddef.arraySize;
					size_t destsz = newdef->elementSize * newdef->arraySize;
					size_t sz = std::min(srcsz, destsz);
					if (newdef->isFloat())
					{

						memcpy(getFloatPointer(newdef->physicalIndex), 
							source.getFloatPointer(olddef.physicalIndex),
							sz * sizeof(float));
					}
					else
					{
						memcpy(getIntPointer(newdef->physicalIndex), 
							source.getIntPointer(olddef.physicalIndex),
							sz * sizeof(int));
					}
					// we'll use this map to resolve autos later
					// ignore the [0] aliases
					if (!StringUtil::endsWith(paramName, "[0]"))
						srcToDestNamedMap[olddef.physicalIndex] = paramName;
				}
			}

			for (AutoConstantList::const_iterator i = source.mAutoConstants.begin(); 
				i != source.mAutoConstants.end(); ++i)
			{
				const GpuProgramParameters::AutoConstantEntry& autoEntry = *i;
				// find dest physical index
				std::map<size_t, String>::iterator mi = srcToDestNamedMap.find(autoEntry.physicalIndex);
				if (mi != srcToDestNamedMap.end())
				{
					if (autoEntry.fData)
					{
						setNamedAutoConstantReal(mi->second, autoEntry.paramType, autoEntry.fData);
					}
					else
					{
						setNamedAutoConstant(mi->second, autoEntry.paramType, autoEntry.data);
					}
				}

			}

			// Copy shared param sets
			for (GpuSharedParamUsageList::const_iterator i = source.mSharedParamSets.begin();
				i != source.mSharedParamSets.end(); ++i)
			{
				const GpuSharedParametersUsage& usage = *i;
				if (!isUsingSharedParameters(usage.getName()))
				{
					addSharedParameters(usage.getSharedParams());
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	const GpuProgramParameters::AutoConstantDefinition* 
		GpuProgramParameters::getAutoConstantDefinition(const String& name)
	{
		// find a constant definition that matches name by iterating through the 
		// constant definition array
		bool nameFound = false;
		size_t i = 0;
		const size_t numDefs = getNumAutoConstantDefinitions();
		while (!nameFound && (i < numDefs))
		{
			if (name == AutoConstantDictionary[i].name) 
				nameFound = true;
			else
				++i;
		}

		if (nameFound)
			return &AutoConstantDictionary[i];
		else
			return 0;
	}

	//-----------------------------------------------------------------------
	const GpuProgramParameters::AutoConstantDefinition* 
		GpuProgramParameters::getAutoConstantDefinition(const size_t idx) 
	{

		if (idx < getNumAutoConstantDefinitions())
		{
			// verify index is equal to acType
			// if they are not equal then the dictionary was not setup properly
			assert(idx == static_cast<size_t>(AutoConstantDictionary[idx].acType));
			return &AutoConstantDictionary[idx];
		}
		else
			return 0;
	}
	//-----------------------------------------------------------------------
	size_t GpuProgramParameters::getNumAutoConstantDefinitions(void)
	{
		return sizeof(AutoConstantDictionary)/sizeof(AutoConstantDefinition);
	}

	//-----------------------------------------------------------------------
	void GpuProgramParameters::incPassIterationNumber(void)
	{
		if (mActivePassIterationIndex != std::numeric_limits<size_t>::max())
		{
			// This is a physical index
			++mFloatConstants[mActivePassIterationIndex];
		}
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::addSharedParameters(GpuSharedParametersPtr sharedParams)
	{
		if (!isUsingSharedParameters(sharedParams->getName()))
		{
			mSharedParamSets.push_back(GpuSharedParametersUsage(sharedParams, this));
		}
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::addSharedParameters(const String& sharedParamsName)
	{
		// TODO PORT - I don't think I'll be needing this. I dont plan on including GpuProgramManager in the port
		//addSharedParameters(GpuProgramManager::getSingleton().getSharedParameters(sharedParamsName));
	}
	//---------------------------------------------------------------------
	bool GpuProgramParameters::isUsingSharedParameters(const String& sharedParamsName) const
	{
		for (GpuSharedParamUsageList::const_iterator i = mSharedParamSets.begin();
			i != mSharedParamSets.end(); ++i)
		{
			if (i->getName() == sharedParamsName)
				return true;
		}
		return false;
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::removeSharedParameters(const String& sharedParamsName)
	{
		for (GpuSharedParamUsageList::iterator i = mSharedParamSets.begin();
			i != mSharedParamSets.end(); ++i)
		{
			if (i->getName() == sharedParamsName)
			{
				mSharedParamSets.erase(i);
				break;
			}
		}
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::removeAllSharedParameters()
	{
		mSharedParamSets.clear();
	}
	//---------------------------------------------------------------------
	const GpuProgramParameters::GpuSharedParamUsageList& 
	GpuProgramParameters::getSharedParameters() const
	{
		return mSharedParamSets;
	}
	//---------------------------------------------------------------------
	void GpuProgramParameters::_copySharedParams()
	{
		for (GpuSharedParamUsageList::iterator i = mSharedParamSets.begin(); 
			i != mSharedParamSets.end(); ++i )
		{
			i->_copySharedParamsToTargetParams();
		}

	}





}

