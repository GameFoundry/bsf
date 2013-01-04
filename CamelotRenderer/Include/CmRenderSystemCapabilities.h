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
#ifndef __RenderSystemCapabilities__
#define __RenderSystemCapabilities__ 1

// Precompiler options
#include "CmPrerequisites.h"
#include "CmString.h"
#include "CmGpuProgram.h"

// Because there are more than 32 possible Capabilities, more than 1 int is needed to store them all.
// In fact, an array of integers is used to store capabilities. However all the capabilities are defined in the single
// enum. The only way to know which capabilities should be stored where in the array is to use some of the 32 bits
// to record the category of the capability.  These top few bits are used as an index into mCapabilities array
// The lower bits are used to identify each capability individually by setting 1 bit for each

// Identifies how many bits are reserved for categories
// NOTE: Although 4 bits (currently) are enough
#define CAPS_CATEGORY_SIZE 4
#define CM_CAPS_BITSHIFT (32 - CAPS_CATEGORY_SIZE)
#define CAPS_CATEGORY_MASK (((1 << CAPS_CATEGORY_SIZE) - 1) << CM_CAPS_BITSHIFT)
#define CM_CAPS_VALUE(cat, val) ((cat << CM_CAPS_BITSHIFT) | (1 << val))

namespace CamelotEngine 
{
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/

	/// Enumerates the categories of capabilities
	enum CapabilitiesCategory
	{
		CAPS_CATEGORY_COMMON = 0,
		CAPS_CATEGORY_COMMON_2 = 1,
		CAPS_CATEGORY_D3D9 = 2,
		CAPS_CATEGORY_GL = 3,
		CAPS_CATEGORY_COMMON_3 = 4,
		/// Placeholder for max value
		CAPS_CATEGORY_COUNT = 5
	};

	/// Enum describing the different hardware capabilities we want to check for
	/// OGRE_CAPS_VALUE(a, b) defines each capability
	// a is the category (which can be from 0 to 15)
	// b is the value (from 0 to 27)
	enum Capabilities
	{
		/// Supports generating mipmaps in hardware
		RSC_AUTOMIPMAP              = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 0),
		RSC_BLENDING                = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 1),
		/// Supports anisotropic texture filtering
		RSC_ANISOTROPY              = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 2),
		/// Supports fixed-function DOT3 texture blend
		RSC_DOT3                    = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 3),
		/// Supports cube mapping
		RSC_CUBEMAPPING             = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 4),
		/// Supports hardware stencil buffer
		RSC_HWSTENCIL               = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 5),
		/// Supports hardware vertex and index buffers
		RSC_VBO                     = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 7),
		/// Supports vertex programs (vertex shaders)
		RSC_VERTEX_PROGRAM          = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 9),
		/// Supports fragment programs (pixel shaders)
		RSC_FRAGMENT_PROGRAM        = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 10),
		/// Supports performing a scissor test to exclude areas of the screen
		RSC_SCISSOR_TEST            = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 11),
		/// Supports separate stencil updates for both front and back faces
		RSC_TWO_SIDED_STENCIL       = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 12),
		/// Supports wrapping the stencil value at the range extremeties
		RSC_STENCIL_WRAP            = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 13),
		/// Supports hardware occlusion queries
		RSC_HWOCCLUSION             = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 14),
		/// Supports user clipping planes
		RSC_USER_CLIP_PLANES        = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 15),
		/// Supports the VET_UBYTE4 vertex element type
		RSC_VERTEX_FORMAT_UBYTE4    = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 16),
		/// Supports infinite far plane projection
		RSC_INFINITE_FAR_PLANE      = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 17),
		/// Supports hardware render-to-texture (bigger than framebuffer)
		RSC_HWRENDER_TO_TEXTURE     = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 18),
		/// Supports float textures and render targets
		RSC_TEXTURE_FLOAT           = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 19),
		/// Supports non-power of two textures
		RSC_NON_POWER_OF_2_TEXTURES = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 20),
		/// Supports 3d (volume) textures
		RSC_TEXTURE_3D              = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 21),
		/// Supports basic point sprite rendering
		RSC_POINT_SPRITES           = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 22),
		/// Supports extra point parameters (minsize, maxsize, attenuation)
		RSC_POINT_EXTENDED_PARAMETERS = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 23),
		/// Supports vertex texture fetch
		RSC_VERTEX_TEXTURE_FETCH = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 24),
		/// Supports mipmap LOD biasing
		RSC_MIPMAP_LOD_BIAS = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 25),
		/// Supports hardware geometry programs
		RSC_GEOMETRY_PROGRAM = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 26),
		/// Supports rendering to vertex buffers
		RSC_HWRENDER_TO_VERTEX_BUFFER = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON, 27),

		/// Supports compressed textures
		RSC_TEXTURE_COMPRESSION = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 0),
		/// Supports compressed textures in the DXT/ST3C formats
		RSC_TEXTURE_COMPRESSION_DXT = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 1),
		/// Supports compressed textures in the VTC format
		RSC_TEXTURE_COMPRESSION_VTC = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 2),
		/// Supports compressed textures in the PVRTC format
		RSC_TEXTURE_COMPRESSION_PVRTC = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 3),
		/// Supports fixed-function pipeline
		RSC_FIXED_FUNCTION = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 4),
		/// Supports MRTs with different bit depths
		RSC_MRT_DIFFERENT_BIT_DEPTHS = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 5),
		/// Supports Alpha to Coverage (A2C)
		RSC_ALPHA_TO_COVERAGE = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 6),
		/// Supports Blending operations other than +
		RSC_ADVANCED_BLEND_OPERATIONS = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON_2, 7),

		// ***** DirectX specific caps *****
		/// Is DirectX feature "per stage constants" supported
		RSC_PERSTAGECONSTANT = CM_CAPS_VALUE(CAPS_CATEGORY_D3D9, 0),

		// ***** GL Specific Caps *****
		/// Supports openGL GLEW version 1.5
		RSC_GL1_5_NOVBO    = CM_CAPS_VALUE(CAPS_CATEGORY_GL, 1),
		/// Support for Frame Buffer Objects (FBOs)
		RSC_FBO              = CM_CAPS_VALUE(CAPS_CATEGORY_GL, 2),
		/// Support for Frame Buffer Objects ARB implementation (regular FBO is higher precedence)
		RSC_FBO_ARB          = CM_CAPS_VALUE(CAPS_CATEGORY_GL, 3),
		/// Support for Frame Buffer Objects ATI implementation (ARB FBO is higher precedence)
		RSC_FBO_ATI          = CM_CAPS_VALUE(CAPS_CATEGORY_GL, 4),
		/// Support for PBuffer
		RSC_PBUFFER          = CM_CAPS_VALUE(CAPS_CATEGORY_GL, 5),
		/// Support for GL 1.5 but without HW occlusion workaround
		RSC_GL1_5_NOHWOCCLUSION = CM_CAPS_VALUE(CAPS_CATEGORY_GL, 6),
		/// Support for point parameters ARB implementation
		RSC_POINT_EXTENDED_PARAMETERS_ARB = CM_CAPS_VALUE(CAPS_CATEGORY_GL, 7),
		/// Support for point parameters EXT implementation
		RSC_POINT_EXTENDED_PARAMETERS_EXT = CM_CAPS_VALUE(CAPS_CATEGORY_GL, 8),

		/// Supports hardware tessellation programs
		RSC_TESSELLATION_PROGRAM = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON_3, 0),
		/// Supports hardware compute programs
		RSC_COMPUTE_PROGRAM = CM_CAPS_VALUE(CAPS_CATEGORY_COMMON_3, 1),

	};

	/// DriverVersion is used by RenderSystemCapabilities and both GL and D3D9
	/// to store the version of the current GPU driver
	struct CM_EXPORT DriverVersion 
	{
		int major;
		int minor;
		int release;
		int build;

		DriverVersion() 
		{
			major = minor = release = build = 0;
		}

		String toString() const 
		{
			StringUtil::StrStreamType str;
			str << major << "." << minor << "." << release << "." << build;
			return str.str();
		}

		void fromString(const String& versionString)
		{
			std::vector<CamelotEngine::String> tokens = StringUtil::split(versionString, ".");
			if(!tokens.empty())
			{
				major = parseInt(tokens[0]);
				if (tokens.size() > 1)
					minor = parseInt(tokens[1]);
				if (tokens.size() > 2)
					release = parseInt(tokens[2]);
				if (tokens.size() > 3)
					build = parseInt(tokens[3]);
			}

		}
	};

	/** Enumeration of GPU vendors. */
	enum GPUVendor
	{
		GPU_UNKNOWN = 0,
		GPU_NVIDIA = 1,
		GPU_ATI = 2, 
		GPU_INTEL = 3,
		GPU_S3 = 4,
		GPU_MATROX = 5,
		GPU_3DLABS = 6,
		GPU_SIS = 7,
        GPU_IMAGINATION_TECHNOLOGIES = 8,
        GPU_APPLE = 9,  // Apple Software Renderer
        GPU_NOKIA = 10,

		/// placeholder
		GPU_VENDOR_COUNT = 11
	};

	enum GpuProgramProfile
	{
		GPP_NONE,
		GPP_PS_1_1,
		GPP_PS_1_2,
		GPP_PS_1_3,
		GPP_PS_1_4,
		GPP_PS_2_0,
		GPP_PS_2_x,
		GPP_PS_2_a,
		GPP_PS_2_b,
		GPP_PS_3_0,
		GPP_PS_3_x,
		GPP_PS_4_0,
		GPP_PS_5_0,
		GPP_VS_1_1,
		GPP_VS_2_0,
		GPP_VS_2_x,
		GPP_VS_2_a,
		GPP_VS_3_0,
		GPP_VS_4_0,
		GPP_VS_5_0,
		GPP_GS_4_0,
		GPP_GS_5_0,
		GPP_HS_5_0,
		GPP_DS_5_0,
		GPP_CS_5_0
	};

	/** singleton class for storing the capabilities of the graphics card. 
	@remarks
	This class stores the capabilities of the graphics card.  This
	information is set by the individual render systems.
	*/
	class CM_EXPORT RenderSystemCapabilities
	{

	public:

		typedef set<String>::type ShaderProfiles;
	private:
		/// This is used to build a database of RSC's
		/// if a RSC with same name, but newer version is introduced, the older one 
		/// will be removed
		DriverVersion mDriverVersion;
		/// GPU Vendor
		GPUVendor mVendor;

		static std::vector<CamelotEngine::String> msGPUVendorStrings;
		static void initVendorStrings();

		/// The number of world matrices available
		UINT16 mNumWorldMatrices;
		/// The number of texture units available per stage
		map<GpuProgramType, UINT16>::type mNumTextureUnitsPerStage;
		/// Total number of texture units available
		UINT16 mNumCombinedTextureUnits;
		/// The stencil buffer bit depth
		UINT16 mStencilBufferBitDepth;
		/// The number of matrices available for hardware blending
		UINT16 mNumVertexBlendMatrices;
		/// Stores the capabilities flags.
		int mCapabilities[CAPS_CATEGORY_COUNT];
		/// Which categories are relevant
		bool mCategoryRelevant[CAPS_CATEGORY_COUNT];
		/// The name of the device as reported by the render system
		String mDeviceName;
		/// The identifier associated with the render system for which these capabilities are valid
		String mRenderSystemName;

		/// The number of floating-point constants vertex programs support
		UINT16 mVertexProgramConstantFloatCount;           
		/// The number of integer constants vertex programs support
		UINT16 mVertexProgramConstantIntCount;           
		/// The number of boolean constants vertex programs support
		UINT16 mVertexProgramConstantBoolCount;           
		/// The number of floating-point constants geometry programs support
		UINT16 mGeometryProgramConstantFloatCount;           
		/// The number of integer constants vertex geometry support
		UINT16 mGeometryProgramConstantIntCount;           
		/// The number of boolean constants vertex geometry support
		UINT16 mGeometryProgramConstantBoolCount;           
		/// The number of floating-point constants fragment programs support
		UINT16 mFragmentProgramConstantFloatCount;           
		/// The number of integer constants fragment programs support
		UINT16 mFragmentProgramConstantIntCount;           
		/// The number of boolean constants fragment programs support
		UINT16 mFragmentProgramConstantBoolCount;
		/// The number of simultaneous render targets supported
		UINT16 mNumMultiRenderTargets;
		/// The maximum point size
		float mMaxPointSize;
		/// Are non-POW2 textures feature-limited?
		bool mNonPOW2TexturesLimited;
		/// The number of vertices a geometry program can emit in a single run
		int mGeometryProgramNumOutputVertices;

		/// The list of supported shader profiles
		ShaderProfiles mSupportedShaderProfiles;

		// Allows us to convert a generic shader profile to a render-system specific one
		unordered_map<GpuProgramProfile, String>::type mGenericToSpecificShaderProfileMap;
	public:	
		RenderSystemCapabilities ();
		virtual ~RenderSystemCapabilities ();

		virtual size_t calculateSize() const {return 0;}

		/** Set the driver version. */
		void setDriverVersion(const DriverVersion& version)
		{
			mDriverVersion = version;
		}

		void parseDriverVersionFromString(const String& versionString)
		{
			DriverVersion version;
			version.fromString(versionString);
			setDriverVersion(version);
		}


		DriverVersion getDriverVersion() const
		{
			return mDriverVersion;
		}

		GPUVendor getVendor() const
		{
			return mVendor;
		}

		void setVendor(GPUVendor v)
		{
			mVendor = v;
		}

		/// Parse and set vendor
		void parseVendorFromString(const String& vendorString)
		{
			setVendor(vendorFromString(vendorString));
		}

		/// Convert a vendor string to an enum
		static GPUVendor vendorFromString(const String& vendorString);
		/// Convert a vendor enum to a string
		static String vendorToString(GPUVendor v);

		bool isDriverOlderThanVersion(DriverVersion v) const
		{
			if (mDriverVersion.major < v.major)
				return true;
			else if (mDriverVersion.major == v.major && 
				mDriverVersion.minor < v.minor)
				return true;
			else if (mDriverVersion.major == v.major && 
				mDriverVersion.minor == v.minor && 
				mDriverVersion.release < v.release)
				return true;
			else if (mDriverVersion.major == v.major && 
				mDriverVersion.minor == v.minor && 
				mDriverVersion.release == v.release &&
				mDriverVersion.build < v.build)
				return true;
			return false;
		}

		void setNumWorldMatrices(UINT16 num)
		{
			mNumWorldMatrices = num;
		}

		void setNumTextureUnits(GpuProgramType type, UINT16 num)
		{
			mNumTextureUnitsPerStage[type] = num;
		}

		void setNumCombinedTextureUnits(UINT16 num) const
		{
			mNumCombinedTextureUnits = num;
		}

		void setStencilBufferBitDepth(UINT16 num)
		{
			mStencilBufferBitDepth = num;
		}

		void setNumVertexBlendMatrices(UINT16 num)
		{
			mNumVertexBlendMatrices = num;
		}

		/// The number of simultaneous render targets supported
		void setNumMultiRenderTargets(UINT16 num)
		{
			mNumMultiRenderTargets = num;
		}

		UINT16 getNumWorldMatrices(void) const
		{ 
			return mNumWorldMatrices;
		}

		/** Returns the number of texture units the current output hardware
		supports, for the specified stage.
		*/
		UINT16 getNumTextureUnits(GpuProgramType stage) const
		{
			return mNumTextureUnitsPerStage[type];
		}

		/** Returns the number of texture units the current output hardware
		supports, total for all stages combined.
		*/
		UINT16 getNumCombinedTextureUnits() const
		{
			return mNumCombinedTextureUnits;
		}

		/** Determines the bit depth of the hardware accelerated stencil 
		buffer, if supported.
		@remarks
		If hardware stencilling is not supported, the software will
		provide an 8-bit software stencil.
		*/
		UINT16 getStencilBufferBitDepth(void) const
		{
			return mStencilBufferBitDepth;
		}

		/** Returns the number of matrices available to hardware vertex 
		blending for this rendering system. */
		UINT16 getNumVertexBlendMatrices(void) const
		{
			return mNumVertexBlendMatrices;
		}

		/// The number of simultaneous render targets supported
		UINT16 getNumMultiRenderTargets(void) const
		{
			return mNumMultiRenderTargets;
		}

		/** Returns true if capability is render system specific
		*/
		bool isCapabilityRenderSystemSpecific(const Capabilities c)
		{
			int cat = c >> CM_CAPS_BITSHIFT;
			if(cat == CAPS_CATEGORY_GL || cat == CAPS_CATEGORY_D3D9)
				return true;
			return false;
		}

		/** Adds a capability flag
		*/
		void setCapability(const Capabilities c) 
		{ 
			int index = (CAPS_CATEGORY_MASK & c) >> CM_CAPS_BITSHIFT;
			// zero out the index from the stored capability
			mCapabilities[index] |= (c & ~CAPS_CATEGORY_MASK);
		}

		/** Remove a capability flag
		*/
		void unsetCapability(const Capabilities c) 
		{ 
			int index = (CAPS_CATEGORY_MASK & c) >> CM_CAPS_BITSHIFT;
			// zero out the index from the stored capability
			mCapabilities[index] &= (~c | CAPS_CATEGORY_MASK);
		}

		/** Checks for a capability
		*/
		bool hasCapability(const Capabilities c) const
		{
			int index = (CAPS_CATEGORY_MASK & c) >> CM_CAPS_BITSHIFT;
			// test against
			if(mCapabilities[index] & (c & ~CAPS_CATEGORY_MASK))
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		/** Adds the profile to the list of supported profiles
		*/
		void addShaderProfile(const String& profile)
		{
			mSupportedShaderProfiles.insert(profile);

		}

		/** Adds the profile to the list of supported profiles
		*/
		void addGpuProgramProfile(GpuProgramProfile gpuProgProfile, const String& rsSpecificProfile)
		{
			mGenericToSpecificShaderProfileMap[gpuProgProfile] = rsSpecificProfile;
		}

		/** Remove a given shader profile, if present.
		*/
		void removeShaderProfile(const String& profile)
		{
			mSupportedShaderProfiles.erase(profile);
		}

		/** Returns true if profile is in the list of supported profiles
		*/
		bool isShaderProfileSupported(const String& profile) const
		{
			return (mSupportedShaderProfiles.end() != mSupportedShaderProfiles.find(profile));
		}


		/** Returns a set of all supported shader profiles
		* */
		const ShaderProfiles& getSupportedShaderProfiles() const
		{
			return mSupportedShaderProfiles;
		}

		/** Converts a generic GpuProgramProfile identifier into a render-system specific one.  
		* 
		*  Returns an empty string if it can't convert it.
		*/
		String gpuProgProfileToRSSpecificProfile(GpuProgramProfile gpuProgProfile) const
		{
			auto iterFind = mGenericToSpecificShaderProfileMap.find(gpuProgProfile);
			if(mGenericToSpecificShaderProfileMap.end() != iterFind)
			{
				return iterFind->second;
			}

			return "";
		}

		/// The number of floating-point constants vertex programs support
		UINT16 getVertexProgramConstantFloatCount(void) const
		{
			return mVertexProgramConstantFloatCount;           
		}
		/// The number of integer constants vertex programs support
		UINT16 getVertexProgramConstantIntCount(void) const
		{
			return mVertexProgramConstantIntCount;           
		}
		/// The number of boolean constants vertex programs support
		UINT16 getVertexProgramConstantBoolCount(void) const
		{
			return mVertexProgramConstantBoolCount;           
		}
		/// The number of floating-point constants geometry programs support
		UINT16 getGeometryProgramConstantFloatCount(void) const
		{
			return mGeometryProgramConstantFloatCount;           
		}
		/// The number of integer constants geometry programs support
		UINT16 getGeometryProgramConstantIntCount(void) const
		{
			return mGeometryProgramConstantIntCount;           
		}
		/// The number of boolean constants geometry programs support
		UINT16 getGeometryProgramConstantBoolCount(void) const
		{
			return mGeometryProgramConstantBoolCount;           
		}
		/// The number of floating-point constants fragment programs support
		UINT16 getFragmentProgramConstantFloatCount(void) const
		{
			return mFragmentProgramConstantFloatCount;           
		}
		/// The number of integer constants fragment programs support
		UINT16 getFragmentProgramConstantIntCount(void) const
		{
			return mFragmentProgramConstantIntCount;           
		}
		/// The number of boolean constants fragment programs support
		UINT16 getFragmentProgramConstantBoolCount(void) const
		{
			return mFragmentProgramConstantBoolCount;           
		}

		/// sets the device name for Render system
		void setDeviceName(const String& name)
		{
			mDeviceName = name;
		}

		/// gets the device name for render system
		String getDeviceName() const
		{
			return mDeviceName;
		}

		/// The number of floating-point constants vertex programs support
		void setVertexProgramConstantFloatCount(UINT16 c)
		{
			mVertexProgramConstantFloatCount = c;           
		}
		/// The number of integer constants vertex programs support
		void setVertexProgramConstantIntCount(UINT16 c)
		{
			mVertexProgramConstantIntCount = c;           
		}
		/// The number of boolean constants vertex programs support
		void setVertexProgramConstantBoolCount(UINT16 c)
		{
			mVertexProgramConstantBoolCount = c;           
		}
		/// The number of floating-point constants geometry programs support
		void setGeometryProgramConstantFloatCount(UINT16 c)
		{
			mGeometryProgramConstantFloatCount = c;           
		}
		/// The number of integer constants geometry programs support
		void setGeometryProgramConstantIntCount(UINT16 c)
		{
			mGeometryProgramConstantIntCount = c;           
		}
		/// The number of boolean constants geometry programs support
		void setGeometryProgramConstantBoolCount(UINT16 c)
		{
			mGeometryProgramConstantBoolCount = c;           
		}
		/// The number of floating-point constants fragment programs support
		void setFragmentProgramConstantFloatCount(UINT16 c)
		{
			mFragmentProgramConstantFloatCount = c;           
		}
		/// The number of integer constants fragment programs support
		void setFragmentProgramConstantIntCount(UINT16 c)
		{
			mFragmentProgramConstantIntCount = c;           
		}
		/// The number of boolean constants fragment programs support
		void setFragmentProgramConstantBoolCount(UINT16 c)
		{
			mFragmentProgramConstantBoolCount = c;           
		}
		/// Maximum point screen size in pixels
		void setMaxPointSize(float s)
		{
			mMaxPointSize = s;
		}
		/// Maximum point screen size in pixels
		float getMaxPointSize(void) const
		{
			return mMaxPointSize;
		}
		/// Non-POW2 textures limited
		void setNonPOW2TexturesLimited(bool l)
		{
			mNonPOW2TexturesLimited = l;
		}
		/** Are non-power of two textures limited in features?
		@remarks
		If the RSC_NON_POWER_OF_2_TEXTURES capability is set, but this
		method returns true, you can use non power of 2 textures only if:
		<ul><li>You load them explicitly with no mip maps</li>
		<li>You don't use DXT texture compression</li>
		<li>You use clamp texture addressing</li></ul>
		*/
		bool getNonPOW2TexturesLimited(void) const
		{
			return mNonPOW2TexturesLimited;
		}
		/// Set the number of vertices a single geometry program run can emit
		void setGeometryProgramNumOutputVertices(int numOutputVertices)
		{
			mGeometryProgramNumOutputVertices = numOutputVertices;
		}
		/// Get the number of vertices a single geometry program run can emit
		int getGeometryProgramNumOutputVertices(void) const
		{
			return mGeometryProgramNumOutputVertices;
		}

		/// Get the identifier of the rendersystem from which these capabilities were generated
		String getRenderSystemName(void) const
		{
			return mRenderSystemName;
		}
		///  Set the identifier of the rendersystem from which these capabilities were generated
		void setRenderSystemName(const String& rs)
		{
			mRenderSystemName = rs;
		}

		/// Mark a category as 'relevant' or not, ie will it be reported
		void setCategoryRelevant(CapabilitiesCategory cat, bool relevant)
		{
			mCategoryRelevant[cat] = relevant;
		}

		/// Return whether a category is 'relevant' or not, ie will it be reported
		bool isCategoryRelevant(CapabilitiesCategory cat)
		{
			return mCategoryRelevant[cat];
		}
	};

	/** @} */
	/** @} */
} // namespace

#endif // __RenderSystemCapabilities__

