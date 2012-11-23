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

The above copyright notice and this permission notice shall be included in*
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.s
-----------------------------------------------------------------------------
*/


#include "CmGLRenderSystem.h"
#include "CmRenderSystem.h"
#include "CmCamera.h"
#include "CmGLTextureManager.h"
#include "CmGLHardwareVertexBuffer.h"
#include "CmGLHardwareIndexBuffer.h"
#include "CmGLDefaultHardwareBufferManager.h"
#include "CmGLUtil.h"
#include "CmGLGpuProgram.h"
#include "ATI_FS_GLGpuProgram.h"
#include "CmGLGpuProgramManager.h"
#include "CmException.h"
#include "CmGLSLExtSupport.h"
#include "CmGLHardwareOcclusionQuery.h"
#include "CmGLContext.h"

#include "CmGLFBORenderTexture.h"
#include "CmGLPBRenderTexture.h"

// Convenience macro from ARB_vertex_buffer_object spec
#define VBO_BUFFER_OFFSET(i) ((char *)NULL + (i))

#if CM_THREAD_SUPPORT != 1
GLenum glewContextInit (CamelotEngine::GLSupport *glSupport);
#endif

namespace CamelotEngine {

	// Callback function used when registering GLGpuPrograms
	GpuProgram* createGLArbGpuProgram(GpuProgramType gptype, const String& syntaxCode)
	{
		GLArbGpuProgram* ret = new GLArbGpuProgram();
		ret->setType(gptype);
		ret->setSyntaxCode(syntaxCode);
		return ret;
	}

	GpuProgram* createGL_ATI_FS_GpuProgram(GpuProgramType gptype, const String& syntaxCode)
	{
		ATI_FS_GLGpuProgram* ret = new ATI_FS_GLGpuProgram();
		ret->setType(gptype);
		ret->setSyntaxCode(syntaxCode);
		return ret;
	}

	GLRenderSystem::GLRenderSystem()
		: mDepthWrite(true), mStencilMask(0xFFFFFFFF),
		mGLSLProgramFactory(0),
		mCgProgramFactory(0),
		mActiveTextureUnit(0)
	{
		size_t i;

		// Get our GLSupport
		mGLSupport = getGLSupport();

		mWorldMatrix = Matrix4::IDENTITY;
		mViewMatrix = Matrix4::IDENTITY;

		initConfigOptions();

		mColourWrite[0] = mColourWrite[1] = mColourWrite[2] = mColourWrite[3] = true;

		for (i = 0; i < CM_MAX_TEXTURE_LAYERS; i++)
		{
			// Dummy value
			mTextureCoordIndex[i] = 99;
			mTextureTypes[i] = 0;
		}

		mActiveRenderTarget = 0;
		mCurrentContext = 0;
		mMainContext = 0;

		mGLInitialised = false;

		mCurrentLights = 0;
		mMinFilter = FO_LINEAR;
		mMipFilter = FO_POINT;
		mCurrentVertexProgram = 0;
		mCurrentGeometryProgram = 0;
		mCurrentFragmentProgram = 0;

	}

	GLRenderSystem::~GLRenderSystem()
	{
		shutdown();

		// Destroy render windows
		RenderTargetMap::iterator i;
		for (i = mRenderTargets.begin(); i != mRenderTargets.end(); ++i)
		{
			delete i->second;
		}
		mRenderTargets.clear();

        if(mGLSupport)
            delete mGLSupport;
	}

	const String& GLRenderSystem::getName(void) const
	{
		static String strName("GLRenderSystem");
		return strName;
	}

	void GLRenderSystem::initConfigOptions(void)
	{
		mGLSupport->addConfig();
	}

	ConfigOptionMap& GLRenderSystem::getConfigOptions(void)
	{
		return mGLSupport->getConfigOptions();
	}

	void GLRenderSystem::setConfigOption(const String &name, const String &value)
	{
		mGLSupport->setConfigOption(name, value);
	}

	String GLRenderSystem::validateConfigOptions(void)
	{
		// XXX Return an error string if something is invalid
		return mGLSupport->validateConfig();
	}

	RenderWindow* GLRenderSystem::_initialise(bool autoCreateWindow, const String& windowTitle)
	{
		mGLSupport->start();

		RenderWindow* autoWindow = mGLSupport->createWindow(autoCreateWindow, this, windowTitle);

		RenderSystem::_initialise(autoCreateWindow, windowTitle);

		return autoWindow;
	}

	RenderSystemCapabilities* GLRenderSystem::createRenderSystemCapabilities() const
	{
		RenderSystemCapabilities* rsc = new RenderSystemCapabilities();

		rsc->setCategoryRelevant(CAPS_CATEGORY_GL, true);
		rsc->setDriverVersion(mDriverVersion);
		const char* deviceName = (const char*)glGetString(GL_RENDERER);
		const char* vendorName = (const char*)glGetString(GL_VENDOR);
		rsc->setDeviceName(deviceName);
		rsc->setRenderSystemName(getName());

		// determine vendor
		if (strstr(vendorName, "NVIDIA"))
			rsc->setVendor(GPU_NVIDIA);
		else if (strstr(vendorName, "ATI"))
			rsc->setVendor(GPU_ATI);
		else if (strstr(vendorName, "Intel"))
			rsc->setVendor(GPU_INTEL);
		else if (strstr(vendorName, "S3"))
			rsc->setVendor(GPU_S3);
		else if (strstr(vendorName, "Matrox"))
			rsc->setVendor(GPU_MATROX);
		else if (strstr(vendorName, "3DLabs"))
			rsc->setVendor(GPU_3DLABS);
		else if (strstr(vendorName, "SiS"))
			rsc->setVendor(GPU_SIS);
		else
			rsc->setVendor(GPU_UNKNOWN);

		// Supports fixed-function
		rsc->setCapability(RSC_FIXED_FUNCTION);

        // Check for hardware mipmapping support.
        if(GLEW_VERSION_1_4 || GLEW_SGIS_generate_mipmap)
        {
			bool disableAutoMip = false;
#if CM_PLATFORM == CM_PLATFORM_APPLE || CM_PLATFORM == CM_PLATFORM_LINUX
			// Apple & Linux ATI drivers have faults in hardware mipmap generation
			if (rsc->getVendor() == GPU_ATI)
				disableAutoMip = true;
#endif
			// The Intel 915G frequently corrupts textures when using hardware mip generation
			// I'm not currently sure how many generations of hardware this affects, 
			// so for now, be safe.
			if (rsc->getVendor() == GPU_INTEL)
				disableAutoMip = true;

			// SiS chipsets also seem to have problems with this
			if (rsc->getVendor() == GPU_SIS)
				disableAutoMip = true;

			if (!disableAutoMip)
				rsc->setCapability(RSC_AUTOMIPMAP);
        }

		// Check for blending support
		if(GLEW_VERSION_1_3 || 
			GLEW_ARB_texture_env_combine || 
			GLEW_EXT_texture_env_combine)
		{
			rsc->setCapability(RSC_BLENDING);
		}

		// Check for Multitexturing support and set number of texture units
		if(GLEW_VERSION_1_3 || 
			GLEW_ARB_multitexture)
		{
			GLint units;
			glGetIntegerv( GL_MAX_TEXTURE_UNITS, &units );

			if (GLEW_ARB_fragment_program)
			{
				// Also check GL_MAX_TEXTURE_IMAGE_UNITS_ARB since NV at least
				// only increased this on the FX/6x00 series
				GLint arbUnits;
				glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &arbUnits );
				if (arbUnits > units)
					units = arbUnits;
			}
			rsc->setNumTextureUnits(units);
		}
		else
		{
			// If no multitexture support then set one texture unit
			rsc->setNumTextureUnits(1);
		}

		// Check for Anisotropy support
		if(GLEW_EXT_texture_filter_anisotropic)
		{
			rsc->setCapability(RSC_ANISOTROPY);
		}

		// Check for DOT3 support
		if(GLEW_VERSION_1_3 ||
			GLEW_ARB_texture_env_dot3 ||
			GLEW_EXT_texture_env_dot3)
		{
			rsc->setCapability(RSC_DOT3);
		}

		// Check for cube mapping
		if(GLEW_VERSION_1_3 || 
			GLEW_ARB_texture_cube_map ||
			GLEW_EXT_texture_cube_map)
		{
			rsc->setCapability(RSC_CUBEMAPPING);
		}


		// Point sprites
		if (GLEW_VERSION_2_0 ||	GLEW_ARB_point_sprite)
		{
			rsc->setCapability(RSC_POINT_SPRITES);
		}
		// Check for point parameters
		if (GLEW_VERSION_1_4)
		{
			rsc->setCapability(RSC_POINT_EXTENDED_PARAMETERS);
		}
		if (GLEW_ARB_point_parameters)
		{
			rsc->setCapability(RSC_POINT_EXTENDED_PARAMETERS_ARB);
		}
		if (GLEW_EXT_point_parameters)
		{
			rsc->setCapability(RSC_POINT_EXTENDED_PARAMETERS_EXT);
		}

		// Check for hardware stencil support and set bit depth
		GLint stencil;
		glGetIntegerv(GL_STENCIL_BITS,&stencil);

		if(stencil)
		{
			rsc->setCapability(RSC_HWSTENCIL);
			rsc->setStencilBufferBitDepth(stencil);
		}


		if(GLEW_VERSION_1_5 || GLEW_ARB_vertex_buffer_object)
		{
			if (!GLEW_ARB_vertex_buffer_object)
			{
				rsc->setCapability(RSC_GL1_5_NOVBO);
			}
			rsc->setCapability(RSC_VBO);
		}

		if(GLEW_ARB_vertex_program)
		{
			rsc->setCapability(RSC_VERTEX_PROGRAM);

			// Vertex Program Properties
			rsc->setVertexProgramConstantBoolCount(0);
			rsc->setVertexProgramConstantIntCount(0);

			GLint floatConstantCount;
			glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, &floatConstantCount);
			rsc->setVertexProgramConstantFloatCount(floatConstantCount);

			rsc->addShaderProfile("arbvp1");
			rsc->addGpuProgramProfile(GPP_VS_1_1, "arbvp1"); // TODO - I don't know if any of these GpuProgramProfile mappings are correct!
			rsc->addGpuProgramProfile(GPP_VS_2_0, "arbvp1");
			rsc->addGpuProgramProfile(GPP_VS_2_a, "arbvp1");
			rsc->addGpuProgramProfile(GPP_VS_2_x, "arbvp1");

			if (GLEW_NV_vertex_program2_option)
			{
				rsc->addShaderProfile("vp30");
				rsc->addGpuProgramProfile(GPP_VS_3_0, "vp30"); // TODO - I don't know if any of these GpuProgramProfile mappings are correct!
				rsc->addGpuProgramProfile(GPP_VS_4_0, "vp30");
			}

			if (GLEW_NV_vertex_program3)
			{
				rsc->addShaderProfile("vp40");
				rsc->addGpuProgramProfile(GPP_VS_3_0, "vp40"); // TODO - I don't know if any of these GpuProgramProfile mappings are correct!
				rsc->addGpuProgramProfile(GPP_VS_4_0, "vp40");
			}

			if (GLEW_NV_vertex_program4)
			{
				rsc->addShaderProfile("gp4vp");
				rsc->addShaderProfile("gpu_vp");
			}
		}

		if (GLEW_NV_register_combiners2 &&
			GLEW_NV_texture_shader)
		{
			rsc->setCapability(RSC_FRAGMENT_PROGRAM);
		}

		// NFZ - check for ATI fragment shader support
		if (GLEW_ATI_fragment_shader)
		{
			rsc->setCapability(RSC_FRAGMENT_PROGRAM);
			// no boolean params allowed
			rsc->setFragmentProgramConstantBoolCount(0);
			// no integer params allowed
			rsc->setFragmentProgramConstantIntCount(0);

			// only 8 Vector4 constant floats supported
			rsc->setFragmentProgramConstantFloatCount(8);

			rsc->addShaderProfile("ps_1_4");
			rsc->addShaderProfile("ps_1_3");
			rsc->addShaderProfile("ps_1_2");
			rsc->addShaderProfile("ps_1_1");

			rsc->addGpuProgramProfile(GPP_PS_1_1, "ps_1_1"); // TODO - I don't know if any of these GpuProgramProfile mappings are correct!
			rsc->addGpuProgramProfile(GPP_PS_1_2, "ps_1_2");
			rsc->addGpuProgramProfile(GPP_PS_1_3, "ps_1_3");
			rsc->addGpuProgramProfile(GPP_PS_1_4, "ps_1_4");
		}

		if (GLEW_ARB_fragment_program)
		{
			rsc->setCapability(RSC_FRAGMENT_PROGRAM);

			// Fragment Program Properties
			rsc->setFragmentProgramConstantBoolCount(0);
			rsc->setFragmentProgramConstantIntCount(0);

			GLint floatConstantCount;
			glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, &floatConstantCount);
			rsc->setFragmentProgramConstantFloatCount(floatConstantCount);

			rsc->addShaderProfile("arbfp1");
			rsc->addGpuProgramProfile(GPP_PS_1_1, "arbfp1"); // TODO - I don't know if any of these GpuProgramProfile mappings are correct!
			rsc->addGpuProgramProfile(GPP_PS_1_2, "arbfp1");
			rsc->addGpuProgramProfile(GPP_PS_1_3, "arbfp1");
			rsc->addGpuProgramProfile(GPP_PS_1_4, "arbfp1");
			rsc->addGpuProgramProfile(GPP_PS_2_0, "arbfp1");
			rsc->addGpuProgramProfile(GPP_PS_2_a, "arbfp1");
			rsc->addGpuProgramProfile(GPP_PS_2_b, "arbfp1");
			rsc->addGpuProgramProfile(GPP_PS_2_x, "arbfp1");

			if (GLEW_NV_fragment_program_option)
			{
				rsc->addShaderProfile("fp30");
				rsc->addGpuProgramProfile(GPP_PS_3_0, "fp30"); // TODO - I don't know if any of these GpuProgramProfile mappings are correct!
				rsc->addGpuProgramProfile(GPP_PS_3_x, "fp30");
				rsc->addGpuProgramProfile(GPP_PS_4_0, "fp30");
			}

			if (GLEW_NV_fragment_program2)
			{
				rsc->addShaderProfile("fp40");
				rsc->addGpuProgramProfile(GPP_PS_3_0, "fp40"); // TODO - I don't know if any of these GpuProgramProfile mappings are correct!
				rsc->addGpuProgramProfile(GPP_PS_3_x, "fp40");
				rsc->addGpuProgramProfile(GPP_PS_4_0, "fp40");
			}        
		}

		rsc->addShaderProfile("cg");

		// NFZ - Check if GLSL is supported
		if ( GLEW_VERSION_2_0 || 
			(GLEW_ARB_shading_language_100 &&
			GLEW_ARB_shader_objects &&
			GLEW_ARB_fragment_shader &&
			GLEW_ARB_vertex_shader) )
		{
			rsc->addShaderProfile("glsl");
		}

		// Check if geometry shaders are supported
		if (GLEW_VERSION_2_0 &&
			GLEW_EXT_geometry_shader4)
		{
			rsc->setCapability(RSC_GEOMETRY_PROGRAM);
			rsc->addShaderProfile("nvgp4");

			//Also add the CG profiles
			rsc->addShaderProfile("gpu_gp");
			rsc->addShaderProfile("gp4gp");

			rsc->setGeometryProgramConstantBoolCount(0);
			rsc->setGeometryProgramConstantIntCount(0);

			GLint floatConstantCount = 0;
            glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_EXT, &floatConstantCount);
			rsc->setGeometryProgramConstantFloatCount(floatConstantCount);

			GLint maxOutputVertices;
			glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&maxOutputVertices);
			rsc->setGeometryProgramNumOutputVertices(maxOutputVertices);
		}
		
		//Check if render to vertex buffer (transform feedback in OpenGL)
		if (GLEW_VERSION_2_0 && 
			GLEW_NV_transform_feedback)
		{
			rsc->setCapability(RSC_HWRENDER_TO_VERTEX_BUFFER);
		}

		// Check for texture compression
        if(GLEW_VERSION_1_3 || GLEW_ARB_texture_compression)
        {   
			rsc->setCapability(RSC_TEXTURE_COMPRESSION);
         
            // Check for dxt compression
            if(GLEW_EXT_texture_compression_s3tc)
            {
#if defined(__APPLE__) && defined(__PPC__)
			// Apple on ATI & PPC has errors in DXT
			if (mGLSupport->getGLVendor().find("ATI") == std::string::npos)
#endif
					rsc->setCapability(RSC_TEXTURE_COMPRESSION_DXT);
            }
            // Check for vtc compression
            if(GLEW_NV_texture_compression_vtc)
            {
                rsc->setCapability(RSC_TEXTURE_COMPRESSION_VTC);
            }
        }

		// Scissor test is standard in GL 1.2 (is it emulated on some cards though?)
		rsc->setCapability(RSC_SCISSOR_TEST);
		// As are user clipping planes
		rsc->setCapability(RSC_USER_CLIP_PLANES);

		// 2-sided stencil?
		if (GLEW_VERSION_2_0 || GLEW_EXT_stencil_two_side)
		{
			rsc->setCapability(RSC_TWO_SIDED_STENCIL);
		}
		// stencil wrapping?
		if (GLEW_VERSION_1_4 || GLEW_EXT_stencil_wrap)
		{
			rsc->setCapability(RSC_STENCIL_WRAP);
		}

		// Check for hardware occlusion support
		if(GLEW_VERSION_1_5 || GLEW_ARB_occlusion_query)
		{
			// Some buggy driver claim that it is GL 1.5 compliant and
			// not support ARB_occlusion_query
			if (!GLEW_ARB_occlusion_query)
			{
				rsc->setCapability(RSC_GL1_5_NOHWOCCLUSION);
			}

			rsc->setCapability(RSC_HWOCCLUSION);
		}
		else if (GLEW_NV_occlusion_query)
		{
			// Support NV extension too for old hardware
			rsc->setCapability(RSC_HWOCCLUSION);
		}

		// UBYTE4 always supported
		rsc->setCapability(RSC_VERTEX_FORMAT_UBYTE4);

		// Infinite far plane always supported
		rsc->setCapability(RSC_INFINITE_FAR_PLANE);

		// Check for non-power-of-2 texture support
		if(GLEW_ARB_texture_non_power_of_two)
		{
			rsc->setCapability(RSC_NON_POWER_OF_2_TEXTURES);
		}

		// Check for Float textures
		if(GLEW_ATI_texture_float || GLEW_ARB_texture_float)
		{
			rsc->setCapability(RSC_TEXTURE_FLOAT);
		}

		// 3D textures should be supported by GL 1.2, which is our minimum version
		rsc->setCapability(RSC_TEXTURE_3D);

		// Check for framebuffer object extension
		if(GLEW_EXT_framebuffer_object)
		{
			// Probe number of draw buffers
			// Only makes sense with FBO support, so probe here
			if(GLEW_VERSION_2_0 || 
				GLEW_ARB_draw_buffers ||
				GLEW_ATI_draw_buffers)
			{
				GLint buffers;
				glGetIntegerv(GL_MAX_DRAW_BUFFERS_ARB, &buffers);
				rsc->setNumMultiRenderTargets(std::min<int>(buffers, (GLint)CM_MAX_MULTIPLE_RENDER_TARGETS));
				rsc->setCapability(RSC_MRT_DIFFERENT_BIT_DEPTHS);
				if(!GLEW_VERSION_2_0)
				{
					// Before GL version 2.0, we need to get one of the extensions
					if(GLEW_ARB_draw_buffers)
						rsc->setCapability(RSC_FBO_ARB);
					if(GLEW_ATI_draw_buffers)
						rsc->setCapability(RSC_FBO_ATI);
				}
				// Set FBO flag for all 3 'subtypes'
				rsc->setCapability(RSC_FBO);

			}
			rsc->setCapability(RSC_HWRENDER_TO_TEXTURE);
		}

		// Check GLSupport for PBuffer support
		if(mGLSupport->supportsPBuffers())
		{
			// Use PBuffers
			rsc->setCapability(RSC_HWRENDER_TO_TEXTURE);
			rsc->setCapability(RSC_PBUFFER);
		}

		// Point size
		if (GLEW_VERSION_1_4)
		{
		float ps;
		glGetFloatv(GL_POINT_SIZE_MAX, &ps);
		rsc->setMaxPointSize(ps);
		}
		else
		{
			GLint vSize[2];
			glGetIntegerv(GL_POINT_SIZE_RANGE,vSize);
			rsc->setMaxPointSize((float)vSize[1]);
		}

		// Vertex texture fetching
		if (mGLSupport->checkExtension("GL_ARB_vertex_shader"))
		{
		GLint vUnits;
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB, &vUnits);
		rsc->setNumVertexTextureUnits(static_cast<UINT16>(vUnits));
		if (vUnits > 0)
		{
			rsc->setCapability(RSC_VERTEX_TEXTURE_FETCH);
		}
		// GL always shares vertex and fragment texture units (for now?)
		rsc->setVertexTextureUnitsShared(true);
		}

		// Mipmap LOD biasing?
		if (GLEW_VERSION_1_4 || GLEW_EXT_texture_lod_bias)
		{
			rsc->setCapability(RSC_MIPMAP_LOD_BIAS);
		}

		// Alpha to coverage?
		if (mGLSupport->checkExtension("GL_ARB_multisample"))
		{
			// Alpha to coverage always 'supported' when MSAA is available
			// although card may ignore it if it doesn't specifically support A2C
			rsc->setCapability(RSC_ALPHA_TO_COVERAGE);
		}

		// Advanced blending operations
		if(GLEW_VERSION_2_0)
		{
			rsc->setCapability(RSC_ADVANCED_BLEND_OPERATIONS);
		}

		return rsc;
	}

	void GLRenderSystem::initialiseFromRenderSystemCapabilities(RenderSystemCapabilities* caps, RenderTarget* primary)
	{
		if(caps->getRenderSystemName() != getName())
		{
			CM_EXCEPT(InvalidParametersException, 
				"Trying to initialize GLRenderSystem from RenderSystemCapabilities that do not support OpenGL");
		}

		// set texture the number of texture units
		mFixedFunctionTextureUnits = caps->getNumTextureUnits();

		//In GL there can be less fixed function texture units than general
		//texture units. Get the minimum of the two.
		if (caps->hasCapability(RSC_FRAGMENT_PROGRAM))
		{
			GLint maxTexCoords = 0;
			glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB, &maxTexCoords);
			if (mFixedFunctionTextureUnits > maxTexCoords)
			{
				mFixedFunctionTextureUnits = maxTexCoords;
			}
		}
		
		if(caps->hasCapability(RSC_GL1_5_NOVBO))
		{
			// Assign ARB functions same to GL 1.5 version since
			// interface identical
			glBindBufferARB = glBindBuffer;
			glBufferDataARB = glBufferData;
			glBufferSubDataARB = glBufferSubData;
			glDeleteBuffersARB = glDeleteBuffers;
			glGenBuffersARB = glGenBuffers;
			glGetBufferParameterivARB = glGetBufferParameteriv;
			glGetBufferPointervARB = glGetBufferPointerv;
			glGetBufferSubDataARB = glGetBufferSubData;
			glIsBufferARB = glIsBuffer;
			glMapBufferARB = glMapBuffer;
			glUnmapBufferARB = glUnmapBuffer;
		}

		if(caps->hasCapability(RSC_VBO))
		{
			HardwareBufferManager::startUp(new GLHardwareBufferManager);
		}
		else
		{
			HardwareBufferManager::startUp(new GLDefaultHardwareBufferManager);
		}

		// GPU Program Manager setup
		GpuProgramManager::startUp(new GLGpuProgramManager());
		GLGpuProgramManager* gpuProgramManager = static_cast<GLGpuProgramManager*>(GpuProgramManager::instancePtr());

		if(caps->hasCapability(RSC_VERTEX_PROGRAM))
		{
			if(caps->isShaderProfileSupported("arbvp1"))
			{
				gpuProgramManager->registerProgramFactory("arbvp1", createGLArbGpuProgram);
			}

			if(caps->isShaderProfileSupported("vp30"))
			{
				gpuProgramManager->registerProgramFactory("vp30", createGLArbGpuProgram);
			}

			if(caps->isShaderProfileSupported("vp40"))
			{
				gpuProgramManager->registerProgramFactory("vp40", createGLArbGpuProgram);
			}

			if(caps->isShaderProfileSupported("gp4vp"))
			{
				gpuProgramManager->registerProgramFactory("gp4vp", createGLArbGpuProgram);
			}

			if(caps->isShaderProfileSupported("gpu_vp"))
			{
				gpuProgramManager->registerProgramFactory("gpu_vp", createGLArbGpuProgram);
			}
		}

		if(caps->hasCapability(RSC_GEOMETRY_PROGRAM))
		{
			//TODO : Should these be createGLArbGpuProgram or createGLGpuNVparseProgram?
			if(caps->isShaderProfileSupported("nvgp4"))
			{
				gpuProgramManager->registerProgramFactory("nvgp4", createGLArbGpuProgram);
			}
			if(caps->isShaderProfileSupported("gp4gp"))
			{
				gpuProgramManager->registerProgramFactory("gp4gp", createGLArbGpuProgram);
			}
			if(caps->isShaderProfileSupported("gpu_gp"))
			{
				gpuProgramManager->registerProgramFactory("gpu_gp", createGLArbGpuProgram);
			}
		}

		if(caps->hasCapability(RSC_FRAGMENT_PROGRAM))
		{
			if(caps->isShaderProfileSupported("ps_1_4"))
			{
				gpuProgramManager->registerProgramFactory("ps_1_4", createGL_ATI_FS_GpuProgram);
			}

			if(caps->isShaderProfileSupported("ps_1_3"))
			{
				gpuProgramManager->registerProgramFactory("ps_1_3", createGL_ATI_FS_GpuProgram);
			}

			if(caps->isShaderProfileSupported("ps_1_2"))
			{
				gpuProgramManager->registerProgramFactory("ps_1_2", createGL_ATI_FS_GpuProgram);
			}

			if(caps->isShaderProfileSupported("ps_1_1"))
			{
				gpuProgramManager->registerProgramFactory("ps_1_1", createGL_ATI_FS_GpuProgram);
			}

			if(caps->isShaderProfileSupported("arbfp1"))
			{
				gpuProgramManager->registerProgramFactory("arbfp1", createGLArbGpuProgram);
			}

			if(caps->isShaderProfileSupported("fp40"))
			{
				gpuProgramManager->registerProgramFactory("fp40", createGLArbGpuProgram);
			}

			if(caps->isShaderProfileSupported("fp30"))
			{
				gpuProgramManager->registerProgramFactory("fp30", createGLArbGpuProgram);
			}

		}

		if(caps->isShaderProfileSupported("glsl"))
		{
			// NFZ - check for GLSL vertex and fragment shader support successful
			mGLSLProgramFactory = new GLSLProgramFactory();
			HighLevelGpuProgramManager::instance().addFactory(mGLSLProgramFactory);
		}

		if(caps->isShaderProfileSupported("cg"))
		{
			// NFZ - check for GLSL vertex and fragment shader support successful
			mCgProgramFactory = new CgProgramFactory();
			HighLevelGpuProgramManager::instance().addFactory(mCgProgramFactory);
		}

		if(caps->hasCapability(RSC_HWOCCLUSION))
		{
			if(caps->hasCapability(RSC_GL1_5_NOHWOCCLUSION))
			{
				// Assign ARB functions same to GL 1.5 version since
				// interface identical
				glBeginQueryARB = glBeginQuery;
				glDeleteQueriesARB = glDeleteQueries;
				glEndQueryARB = glEndQuery;
				glGenQueriesARB = glGenQueries;
				glGetQueryObjectivARB = glGetQueryObjectiv;
				glGetQueryObjectuivARB = glGetQueryObjectuiv;
				glGetQueryivARB = glGetQueryiv;
				glIsQueryARB = glIsQuery;
			}
		}


		/// Do this after extension function pointers are initialised as the extension
		/// is used to probe further capabilities.
		ConfigOptionMap::iterator cfi = getConfigOptions().find("RTT Preferred Mode");
		// RTT Mode: 0 use whatever available, 1 use PBuffers, 2 force use copying
		int rttMode = 0;
		if (cfi != getConfigOptions().end())
		{
			if (cfi->second.currentValue == "PBuffer")
			{
				rttMode = 1;
			}
			else if (cfi->second.currentValue == "Copy")
			{
				rttMode = 2;
			}
		}




		// Check for framebuffer object extension
		if(caps->hasCapability(RSC_FBO) && rttMode < 1)
		{
			// Before GL version 2.0, we need to get one of the extensions
			if(caps->hasCapability(RSC_FBO_ARB))
				GLEW_GET_FUN(__glewDrawBuffers) = glDrawBuffersARB;
			else if(caps->hasCapability(RSC_FBO_ATI))
				GLEW_GET_FUN(__glewDrawBuffers) = glDrawBuffersATI;

			if(caps->hasCapability(RSC_HWRENDER_TO_TEXTURE))
			{
				// Create FBO manager
				// TODO LOG PORT - Log this somewhere?
				//LogManager::getSingleton().logMessage("GL: Using GL_EXT_framebuffer_object for rendering to textures (best)");
				GLRTTManager::startUp(new GLFBOManager(false));
			}

		}
		else
		{
			// Check GLSupport for PBuffer support
			if(caps->hasCapability(RSC_PBUFFER) && rttMode < 2)
			{
				if(caps->hasCapability(RSC_HWRENDER_TO_TEXTURE))
				{
					// Use PBuffers
					GLRTTManager::startUp(new GLPBRTTManager(mGLSupport, primary));

					// TODO LOG PORT - Log this somewhere?
					//LogManager::getSingleton().logMessage("GL: Using PBuffers for rendering to textures");
				}
			}
			else
			{
				// No pbuffer support either -- fallback to simplest copying from framebuffer
				GLRTTManager::startUp(new GLCopyingRTTManager());
				// TODO LOG PORT - Log this somewhere?
				//LogManager::getSingleton().logMessage("GL: Using framebuffer copy for rendering to textures (worst)");
				//LogManager::getSingleton().logMessage("GL: Warning: RenderTexture size is restricted to size of framebuffer. If you are on Linux, consider using GLX instead of SDL.");
			}

			// Downgrade number of simultaneous targets
			caps->setNumMultiRenderTargets(1);
		}

		/// Create the texture manager        
		TextureManager::startUp(new GLTextureManager(*mGLSupport)); 

		mGLInitialised = true;
	}

	void GLRenderSystem::reinitialise(void)
	{
		this->shutdown();
		this->_initialise(true);
	}

	void GLRenderSystem::shutdown(void)
	{
		RenderSystem::shutdown();

		// Deleting the GLSL program factory
		if (mGLSLProgramFactory)
		{
			// Remove from manager safely
			HighLevelGpuProgramManager::instance().removeFactory(mGLSLProgramFactory);
			delete mGLSLProgramFactory;
			mGLSLProgramFactory = 0;
		}

		// Deleting Cg GLSL program factory
		if (mCgProgramFactory)
		{
			// Remove from manager safely
			HighLevelGpuProgramManager::instance().removeFactory(mCgProgramFactory);
			delete mCgProgramFactory;
			mCgProgramFactory = 0;
		}

		// Deleting the GPU program manager and hardware buffer manager.  Has to be done before the mGLSupport->stop().
		GpuProgramManager::shutDown();
		HardwareBufferManager::shutDown();
		GLRTTManager::shutDown();

		// Delete extra threads contexts
		for (GLContextList::iterator i = mBackgroundContextList.begin(); 
			i != mBackgroundContextList.end(); ++i)
		{
			GLContext* pCurContext = *i;

			pCurContext->releaseContext();

			delete pCurContext;
		}
		mBackgroundContextList.clear();

		mGLSupport->stop();
		mStopRendering = true;

		TextureManager::shutDown();

		// There will be a new initial window and so forth, thus any call to test
		//  some params will access an invalid pointer, so it is best to reset
		//  the whole state.
		mGLInitialised = 0;
	}
	//---------------------------------------------------------------------
	bool GLRenderSystem::_createRenderWindows(const RenderWindowDescriptionList& renderWindowDescriptions, 
		RenderWindowList& createdWindows)
	{		
		// Call base render system method.
		if (false == RenderSystem::_createRenderWindows(renderWindowDescriptions, createdWindows))
			return false;

		// Simply call _createRenderWindow in a loop.
		for (size_t i = 0; i < renderWindowDescriptions.size(); ++i)
		{
			const RenderWindowDescription& curRenderWindowDescription = renderWindowDescriptions[i];			
			RenderWindow* curWindow = NULL;

			curWindow = createRenderWindow(curRenderWindowDescription.name, 
				curRenderWindowDescription.width, 
				curRenderWindowDescription.height, 
				curRenderWindowDescription.useFullScreen, 
				&curRenderWindowDescription.miscParams);

			createdWindows.push_back(curWindow);											
		}
								
		return true;
	}
	//---------------------------------------------------------------------
	RenderWindow* GLRenderSystem::createRenderWindow(const String &name, 
		unsigned int width, unsigned int height, bool fullScreen,
		const NameValuePairList *miscParams)
	{
		if (mRenderTargets.find(name) != mRenderTargets.end())
		{
			CM_EXCEPT(InvalidParametersException, 
				"Window with name '" + name + "' already exists");
		}

		// Create the window
		RenderWindow* win = mGLSupport->newWindow(name, width, height, 
			fullScreen, miscParams);

		attachRenderTarget( *win );

		if (!mGLInitialised) 
		{                

			// set up glew and GLSupport
			initialiseContext(win);

			std::vector<CamelotEngine::String> tokens = StringUtil::split(mGLSupport->getGLVersion(), ".");

			if (!tokens.empty())
			{
				mDriverVersion.major = parseInt(tokens[0]);
				if (tokens.size() > 1)
					mDriverVersion.minor = parseInt(tokens[1]);
				if (tokens.size() > 2)
					mDriverVersion.release = parseInt(tokens[2]); 
			}
			mDriverVersion.build = 0;
			// Initialise GL after the first window has been created
			// TODO: fire this from emulation options, and don't duplicate float and Current capabilities
			mRealCapabilities = createRenderSystemCapabilities();

			// use real capabilities if custom capabilities are not available
			if(!mUseCustomCapabilities)
				mCurrentCapabilities = mRealCapabilities;

			initialiseFromRenderSystemCapabilities(mCurrentCapabilities, win);

			// Initialise the main context
			_oneTimeContextInitialization();
			if(mCurrentContext)
				mCurrentContext->setInitialized();
		}

		return win;
	}

	void GLRenderSystem::initialiseContext(RenderWindow* primary)
	{
		// Set main and current context
		mMainContext = 0;
		primary->getCustomAttribute("GLCONTEXT", &mMainContext);
		mCurrentContext = mMainContext;

		// Set primary context as active
		if(mCurrentContext)
			mCurrentContext->setCurrent();

		// Setup GLSupport
		mGLSupport->initialiseExtensions();

		// Get extension function pointers
#if CM_THREAD_SUPPORT != 1
		glewContextInit(mGLSupport);
#endif
	}



	//-----------------------------------------------------------------------
	MultiRenderTarget * GLRenderSystem::createMultiRenderTarget(const String & name)
	{
		MultiRenderTarget *retval = GLRTTManager::instancePtr()->createMultiRenderTarget(name);
		attachRenderTarget( *retval );
		return retval;
	}

	//-----------------------------------------------------------------------
	void GLRenderSystem::destroyRenderWindow(RenderWindow* pWin)
	{
		// Find it to remove from list
		RenderTargetMap::iterator i = mRenderTargets.begin();

		while (i != mRenderTargets.end())
		{
			if (i->second == pWin)
			{
				mRenderTargets.erase(i);
				delete pWin;
				break;
			}
		}
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::makeGLMatrix(GLfloat gl_matrix[16], const Matrix4& m)
	{
		size_t x = 0;
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				gl_matrix[x] = m[j][i];
				x++;
			}
		}
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setPointParameters(float size, 
		bool attenuationEnabled, float constant, float linear, float quadratic,
		float minSize, float maxSize)
	{
		
		float val[4] = {1, 0, 0, 1};
		
		if(attenuationEnabled) 
		{
			// Point size is still calculated in pixels even when attenuation is
			// enabled, which is pretty awkward, since you typically want a viewport
			// independent size if you're looking for attenuation.
			// So, scale the point size up by viewport size (this is equivalent to
			// what D3D does as standard)
			size = size * mActiveViewport->getActualHeight();
			minSize = minSize * mActiveViewport->getActualHeight();
			if (maxSize == 0.0f)
				maxSize = mCurrentCapabilities->getMaxPointSize(); // pixels
			else
				maxSize = maxSize * mActiveViewport->getActualHeight();
			
			// XXX: why do I need this for results to be consistent with D3D?
			// Equations are supposedly the same once you factor in vp height
			float correction = 0.005f;
			// scaling required
			val[0] = constant;
			val[1] = linear * correction;
			val[2] = quadratic * correction;
			val[3] = 1;
			
			if (mCurrentCapabilities->hasCapability(RSC_VERTEX_PROGRAM))
				glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
			
			
		} 
		else 
		{
			if (maxSize == 0.0f)
				maxSize = mCurrentCapabilities->getMaxPointSize();
			if (mCurrentCapabilities->hasCapability(RSC_VERTEX_PROGRAM))
				glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
		}
		
		// no scaling required
		// GL has no disabled flag for this so just set to constant
		glPointSize(size);
		
		if (mCurrentCapabilities->hasCapability(RSC_POINT_EXTENDED_PARAMETERS))
		{
			glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, val);
			glPointParameterf(GL_POINT_SIZE_MIN, minSize);
			glPointParameterf(GL_POINT_SIZE_MAX, maxSize);
		} 
		else if (mCurrentCapabilities->hasCapability(RSC_POINT_EXTENDED_PARAMETERS_ARB))
		{
			glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION, val);
			glPointParameterfARB(GL_POINT_SIZE_MIN, minSize);
			glPointParameterfARB(GL_POINT_SIZE_MAX, maxSize);
		} 
		else if (mCurrentCapabilities->hasCapability(RSC_POINT_EXTENDED_PARAMETERS_EXT))
		{
			glPointParameterfvEXT(GL_POINT_DISTANCE_ATTENUATION, val);
			glPointParameterfEXT(GL_POINT_SIZE_MIN, minSize);
			glPointParameterfEXT(GL_POINT_SIZE_MAX, maxSize);
		}
		
		
		
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setTexture(size_t stage, bool enabled, const TexturePtr &texPtr)
	{
		GLTexturePtr tex = std::static_pointer_cast<GLTexture>(texPtr);

		GLenum lastTextureType = mTextureTypes[stage];

		if (!activateGLTextureUnit(stage))
			return;

		if (enabled)
		{
			if (tex)
			{
				// note used
				mTextureTypes[stage] = tex->getGLTextureTarget();
			}
			else
				// assume 2D
				mTextureTypes[stage] = GL_TEXTURE_2D;

			if(lastTextureType != mTextureTypes[stage] && lastTextureType != 0)
			{
				if (stage < mFixedFunctionTextureUnits)
				{
					glDisable( lastTextureType );
				}
			}

			if (stage < mFixedFunctionTextureUnits)
			{
				glEnable( mTextureTypes[stage] );
			}

			if(tex)
				glBindTexture( mTextureTypes[stage], tex->getGLID() );
			else
			{
				glBindTexture( mTextureTypes[stage], static_cast<GLTextureManager*>(&TextureManager::instance())->getWarningTextureID() );
			}
		}
		else
		{
			if (stage < mFixedFunctionTextureUnits)
			{
				if (lastTextureType != 0)
				{
					glDisable( mTextureTypes[stage] );
				}
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
			// bind zero texture
			glBindTexture(GL_TEXTURE_2D, 0); 
		}

		activateGLTextureUnit(0);
	}
	//-----------------------------------------------------------------------------
	GLint GLRenderSystem::getTextureAddressingMode(
		SamplerState::TextureAddressingMode tam) const
	{
		switch(tam)
		{
		default:
		case SamplerState::TAM_WRAP:
			return GL_REPEAT;
		case SamplerState::TAM_MIRROR:
			return GL_MIRRORED_REPEAT;
		case SamplerState::TAM_CLAMP:
			return GL_CLAMP_TO_EDGE;
		case SamplerState::TAM_BORDER:
			return GL_CLAMP_TO_BORDER;
		}

	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setTextureAddressingMode(size_t stage, const SamplerState::UVWAddressingMode& uvw)
	{
		if (!activateGLTextureUnit(stage))
			return;
		glTexParameteri( mTextureTypes[stage], GL_TEXTURE_WRAP_S, 
			getTextureAddressingMode(uvw.u));
		glTexParameteri( mTextureTypes[stage], GL_TEXTURE_WRAP_T, 
			getTextureAddressingMode(uvw.v));
		glTexParameteri( mTextureTypes[stage], GL_TEXTURE_WRAP_R, 
			getTextureAddressingMode(uvw.w));
		activateGLTextureUnit(0);
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setTextureBorderColour(size_t stage, const Color& colour)
	{
		GLfloat border[4] = { colour.r, colour.g, colour.b, colour.a };
		if (activateGLTextureUnit(stage))
		{
		glTexParameterfv( mTextureTypes[stage], GL_TEXTURE_BORDER_COLOR, border);
			activateGLTextureUnit(0);
	}
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setTextureMipmapBias(size_t stage, float bias)
	{
		if (mCurrentCapabilities->hasCapability(RSC_MIPMAP_LOD_BIAS))
		{
			if (activateGLTextureUnit(stage))
			{
			glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, bias);
				activateGLTextureUnit(0);
		}
		}

	}
	//-----------------------------------------------------------------------------
	GLint GLRenderSystem::getBlendMode(SceneBlendFactor ogreBlend) const
	{
		switch(ogreBlend)
		{
		case SBF_ONE:
			return GL_ONE;
		case SBF_ZERO:
			return GL_ZERO;
		case SBF_DEST_COLOUR:
			return GL_DST_COLOR;
		case SBF_SOURCE_COLOUR:
			return GL_SRC_COLOR;
		case SBF_ONE_MINUS_DEST_COLOUR:
			return GL_ONE_MINUS_DST_COLOR;
		case SBF_ONE_MINUS_SOURCE_COLOUR:
			return GL_ONE_MINUS_SRC_COLOR;
		case SBF_DEST_ALPHA:
			return GL_DST_ALPHA;
		case SBF_SOURCE_ALPHA:
			return GL_SRC_ALPHA;
		case SBF_ONE_MINUS_DEST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case SBF_ONE_MINUS_SOURCE_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
		};
		// to keep compiler happy
		return GL_ONE;
	}

	void GLRenderSystem::setSceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op )
	{
		GLint sourceBlend = getBlendMode(sourceFactor);
		GLint destBlend = getBlendMode(destFactor);
		if(sourceFactor == SBF_ONE && destFactor == SBF_ZERO)
		{
			glDisable(GL_BLEND);
		}
		else
		{
			glEnable(GL_BLEND);
			glBlendFunc(sourceBlend, destBlend);
		}

		GLint func = GL_FUNC_ADD;
		switch(op)
		{
		case SBO_ADD:
			func = GL_FUNC_ADD;
			break;
		case SBO_SUBTRACT:
			func = GL_FUNC_SUBTRACT;
			break;
		case SBO_REVERSE_SUBTRACT:
			func = GL_FUNC_REVERSE_SUBTRACT;
			break;
		case SBO_MIN:
			func = GL_MIN;
			break;
		case SBO_MAX:
			func = GL_MAX;
			break;
		}

		if(GLEW_VERSION_1_4 || GLEW_ARB_imaging)
        {
			glBlendEquation(func);
		}
		else if(GLEW_EXT_blend_minmax && (func == GL_MIN || func == GL_MAX))
        {
			glBlendEquationEXT(func);
		}
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setSeparateSceneBlending(
		SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, 
		SceneBlendFactor sourceFactorAlpha, SceneBlendFactor destFactorAlpha,
		SceneBlendOperation op, SceneBlendOperation alphaOp )
	{
		GLint sourceBlend = getBlendMode(sourceFactor);
		GLint destBlend = getBlendMode(destFactor);
		GLint sourceBlendAlpha = getBlendMode(sourceFactorAlpha);
		GLint destBlendAlpha = getBlendMode(destFactorAlpha);

		if(sourceFactor == SBF_ONE && destFactor == SBF_ZERO && 
			sourceFactorAlpha == SBF_ONE && destFactorAlpha == SBF_ZERO)
		{
			glDisable(GL_BLEND);
		}
		else
		{
			glEnable(GL_BLEND);
			glBlendFuncSeparate(sourceBlend, destBlend, sourceBlendAlpha, destBlendAlpha);
		}

		GLint func = GL_FUNC_ADD, alphaFunc = GL_FUNC_ADD;

		switch(op)
		{
		case SBO_ADD:
			func = GL_FUNC_ADD;
			break;
		case SBO_SUBTRACT:
			func = GL_FUNC_SUBTRACT;
			break;
		case SBO_REVERSE_SUBTRACT:
			func = GL_FUNC_REVERSE_SUBTRACT;
			break;
		case SBO_MIN:
			func = GL_MIN;
			break;
		case SBO_MAX:
			func = GL_MAX;
			break;
		}

		switch(alphaOp)
		{
		case SBO_ADD:
			alphaFunc = GL_FUNC_ADD;
			break;
		case SBO_SUBTRACT:
			alphaFunc = GL_FUNC_SUBTRACT;
			break;
		case SBO_REVERSE_SUBTRACT:
			alphaFunc = GL_FUNC_REVERSE_SUBTRACT;
			break;
		case SBO_MIN:
			alphaFunc = GL_MIN;
			break;
		case SBO_MAX:
			alphaFunc = GL_MAX;
			break;
		}

		if(GLEW_VERSION_2_0) {
			glBlendEquationSeparate(func, alphaFunc);
		}
		else if(GLEW_EXT_blend_equation_separate) {
			glBlendEquationSeparateEXT(func, alphaFunc);
		}
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setAlphaRejectSettings(CompareFunction func, unsigned char value, bool alphaToCoverage)
	{
		bool a2c = false;
		static bool lasta2c = false;

		if(func == CMPF_ALWAYS_PASS)
		{
			glDisable(GL_ALPHA_TEST);
		}
		else
		{
			glEnable(GL_ALPHA_TEST);
			a2c = alphaToCoverage;
			glAlphaFunc(convertCompareFunction(func), value / 255.0f);
		}

		if (a2c != lasta2c && getCapabilities()->hasCapability(RSC_ALPHA_TO_COVERAGE))
		{
			if (a2c)
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			else
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

			lasta2c = a2c;
		}

	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setViewport(Viewport *vp)
	{
		// Check if viewport is different
		if (vp != mActiveViewport)
		{
			RenderTarget* target;
			target = vp->getTarget();
			setRenderTarget(target);
			mActiveViewport = vp;

			GLsizei x, y, w, h;

			// Calculate the "lower-left" corner of the viewport
			w = vp->getActualWidth();
			h = vp->getActualHeight();
			x = vp->getActualLeft();
			y = vp->getActualTop();
			if (!target->requiresTextureFlipping())
			{
				// Convert "upper-left" corner to "lower-left"
				y = target->getHeight() - h - y;
			}
			glViewport(x, y, w, h);

			// Configure the viewport clipping
			glScissor(x, y, w, h);
		}
	}

	void GLRenderSystem::setLights()
	{
		// TODO PORT - I'm not using fixed pipeline lights. Remove this later
		//for (size_t i = 0; i < MAX_LIGHTS; ++i)
		//{
		//	if (mLights[i] != NULL)
		//	{
		//		Light* lt = mLights[i];
		//		setGLLightPositionDirection(lt, GL_LIGHT0 + i);
		//	}
		//}
	}

	//-----------------------------------------------------------------------------
	void GLRenderSystem::beginFrame(void)
	{
		if (!mActiveViewport)
			CM_EXCEPT(InvalidStateException, 
			"Cannot begin frame - no viewport selected.");

		// Activate the viewport clipping
		glEnable(GL_SCISSOR_TEST);
	}

	//-----------------------------------------------------------------------------
	void GLRenderSystem::endFrame(void)
	{
		// Deactivate the viewport clipping.
		glDisable(GL_SCISSOR_TEST);
		// unbind GPU programs at end of frame
		// this is mostly to avoid holding bound programs that might get deleted
		// outside via the resource manager
		unbindGpuProgram(GPT_VERTEX_PROGRAM);
		unbindGpuProgram(GPT_FRAGMENT_PROGRAM);
	}

	//-----------------------------------------------------------------------------
	void GLRenderSystem::setCullingMode(CullingMode mode)
	{
		mCullingMode = mode;
		// NB: Because two-sided stencil API dependence of the front face, we must
		// use the same 'winding' for the front face everywhere. As the OGRE default
		// culling mode is clockwise, we also treat anticlockwise winding as front
		// face for consistently. On the assumption that, we can't change the front
		// face by glFrontFace anywhere.

		GLenum cullMode;

		switch( mode )
		{
		case CULL_NONE:
			glDisable( GL_CULL_FACE );
			return;
		default:
		case CULL_CLOCKWISE:
			if (mActiveRenderTarget && 
				((mActiveRenderTarget->requiresTextureFlipping() && !mInvertVertexWinding) ||
				(!mActiveRenderTarget->requiresTextureFlipping() && mInvertVertexWinding)))
			{
				cullMode = GL_FRONT;
			}
			else
			{
				cullMode = GL_BACK;
			}
			break;
		case CULL_ANTICLOCKWISE:
			if (mActiveRenderTarget && 
				((mActiveRenderTarget->requiresTextureFlipping() && !mInvertVertexWinding) ||
				(!mActiveRenderTarget->requiresTextureFlipping() && mInvertVertexWinding)))
			{
				cullMode = GL_BACK;
			}
			else
			{
				cullMode = GL_FRONT;
			}
			break;
		}

		glEnable( GL_CULL_FACE );
		glCullFace( cullMode );
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setDepthBufferParams(bool depthTest, bool depthWrite, CompareFunction depthFunction)
	{
		setDepthBufferCheckEnabled(depthTest);
		setDepthBufferWriteEnabled(depthWrite);
		setDepthBufferFunction(depthFunction);
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setDepthBufferCheckEnabled(bool enabled)
	{
		if (enabled)
		{
			glClearDepth(1.0f);
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setDepthBufferWriteEnabled(bool enabled)
	{
		GLboolean flag = enabled ? GL_TRUE : GL_FALSE;
		glDepthMask( flag );  
		// Store for reference in _beginFrame
		mDepthWrite = enabled;
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setDepthBufferFunction(CompareFunction func)
	{
		glDepthFunc(convertCompareFunction(func));
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setDepthBias(float constantBias, float slopeScaleBias)
	{
		if (constantBias != 0 || slopeScaleBias != 0)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			glEnable(GL_POLYGON_OFFSET_POINT);
			glEnable(GL_POLYGON_OFFSET_LINE);
			glPolygonOffset(-slopeScaleBias, -constantBias);
		}
		else
		{
			glDisable(GL_POLYGON_OFFSET_FILL);
			glDisable(GL_POLYGON_OFFSET_POINT);
			glDisable(GL_POLYGON_OFFSET_LINE);
		}
	}
	//-----------------------------------------------------------------------------
	void GLRenderSystem::setColourBufferWriteEnabled(bool red, bool green, bool blue, bool alpha)
	{
		glColorMask(red, green, blue, alpha);
		// record this
		mColourWrite[0] = red;
		mColourWrite[1] = blue;
		mColourWrite[2] = green;
		mColourWrite[3] = alpha;
	}
	//-----------------------------------------------------------------------------
    String GLRenderSystem::getErrorDescription(long errCode) const
    {
        const GLubyte *errString = gluErrorString (errCode);
		return (errString != 0) ? String((const char*) errString) : StringUtil::BLANK;
    }

	VertexElementType GLRenderSystem::getColourVertexElementType(void) const
	{
		return VET_COLOUR_ABGR;
	}

	void GLRenderSystem::_convertProjectionMatrix(const Matrix4& matrix,
		Matrix4& dest, bool forGpuProgram)
	{
		// no any conversion request for OpenGL
		dest = matrix;
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setPolygonMode(PolygonMode level)
	{
		GLenum glmode;
		switch(level)
		{
		case PM_POINTS:
			glmode = GL_POINT;
			break;
		case PM_WIREFRAME:
			glmode = GL_LINE;
			break;
		default:
		case PM_SOLID:
			glmode = GL_FILL;
			break;
		}
		glPolygonMode(GL_FRONT_AND_BACK, glmode);
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setStencilCheckEnabled(bool enabled)
	{
		if (enabled)
		{
			glEnable(GL_STENCIL_TEST);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setStencilBufferParams(CompareFunction func, 
		UINT32 refValue, UINT32 mask, StencilOperation stencilFailOp, 
		StencilOperation depthFailOp, StencilOperation passOp, 
		bool twoSidedOperation)
	{
		bool flip;
		mStencilMask = mask;

		if (twoSidedOperation)
		{
			if (!mCurrentCapabilities->hasCapability(RSC_TWO_SIDED_STENCIL))
				CM_EXCEPT(InvalidParametersException, "2-sided stencils are not supported");

			// NB: We should always treat CCW as front face for consistent with default
			// culling mode. Therefore, we must take care with two-sided stencil settings.
			flip = (mInvertVertexWinding && !mActiveRenderTarget->requiresTextureFlipping()) ||
				(!mInvertVertexWinding && mActiveRenderTarget->requiresTextureFlipping());
			if(GLEW_VERSION_2_0) // New GL2 commands
			{
				// Back
				glStencilMaskSeparate(GL_BACK, mask);
				glStencilFuncSeparate(GL_BACK, convertCompareFunction(func), refValue, mask);
				glStencilOpSeparate(GL_BACK, 
					convertStencilOp(stencilFailOp, !flip), 
					convertStencilOp(depthFailOp, !flip), 
					convertStencilOp(passOp, !flip));
				// Front
				glStencilMaskSeparate(GL_FRONT, mask);
				glStencilFuncSeparate(GL_FRONT, convertCompareFunction(func), refValue, mask);
				glStencilOpSeparate(GL_FRONT, 
					convertStencilOp(stencilFailOp, flip),
					convertStencilOp(depthFailOp, flip), 
					convertStencilOp(passOp, flip));
			}
			else // EXT_stencil_two_side
			{
				glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
				// Back
				glActiveStencilFaceEXT(GL_BACK);
				glStencilMask(mask);
				glStencilFunc(convertCompareFunction(func), refValue, mask);
				glStencilOp(
					convertStencilOp(stencilFailOp, !flip), 
					convertStencilOp(depthFailOp, !flip), 
					convertStencilOp(passOp, !flip));
				// Front
				glActiveStencilFaceEXT(GL_FRONT);
				glStencilMask(mask);
				glStencilFunc(convertCompareFunction(func), refValue, mask);
				glStencilOp(
					convertStencilOp(stencilFailOp, flip),
					convertStencilOp(depthFailOp, flip), 
					convertStencilOp(passOp, flip));
			}
		}
		else
		{
            if(!GLEW_VERSION_2_0)
                glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);

			flip = false;
			glStencilMask(mask);
			glStencilFunc(convertCompareFunction(func), refValue, mask);
			glStencilOp(
				convertStencilOp(stencilFailOp, flip),
				convertStencilOp(depthFailOp, flip), 
				convertStencilOp(passOp, flip));
		}
	}
	//---------------------------------------------------------------------
	GLint GLRenderSystem::convertCompareFunction(CompareFunction func) const
	{
		switch(func)
		{
		case CMPF_ALWAYS_FAIL:
			return GL_NEVER;
		case CMPF_ALWAYS_PASS:
			return GL_ALWAYS;
		case CMPF_LESS:
			return GL_LESS;
		case CMPF_LESS_EQUAL:
			return GL_LEQUAL;
		case CMPF_EQUAL:
			return GL_EQUAL;
		case CMPF_NOT_EQUAL:
			return GL_NOTEQUAL;
		case CMPF_GREATER_EQUAL:
			return GL_GEQUAL;
		case CMPF_GREATER:
			return GL_GREATER;
		};
		// to keep compiler happy
		return GL_ALWAYS;
	}
	//---------------------------------------------------------------------
	GLint GLRenderSystem::convertStencilOp(StencilOperation op, bool invert) const
	{
		switch(op)
		{
		case SOP_KEEP:
			return GL_KEEP;
		case SOP_ZERO:
			return GL_ZERO;
		case SOP_REPLACE:
			return GL_REPLACE;
		case SOP_INCREMENT:
			return invert ? GL_DECR : GL_INCR;
		case SOP_DECREMENT:
			return invert ? GL_INCR : GL_DECR;
		case SOP_INCREMENT_WRAP:
			return invert ? GL_DECR_WRAP_EXT : GL_INCR_WRAP_EXT;
		case SOP_DECREMENT_WRAP:
			return invert ? GL_INCR_WRAP_EXT : GL_DECR_WRAP_EXT;
		case SOP_INVERT:
			return GL_INVERT;
		};
		// to keep compiler happy
		return SOP_KEEP;
	}
	//---------------------------------------------------------------------
	GLuint GLRenderSystem::getCombinedMinMipFilter(void) const
	{
		switch(mMinFilter)
		{
		case FO_ANISOTROPIC:
		case FO_LINEAR:
			switch(mMipFilter)
			{
			case FO_ANISOTROPIC:
			case FO_LINEAR:
				// linear min, linear mip
				return GL_LINEAR_MIPMAP_LINEAR;
			case FO_POINT:
				// linear min, point mip
				return GL_LINEAR_MIPMAP_NEAREST;
			case FO_NONE:
				// linear min, no mip
				return GL_LINEAR;
			}
			break;
		case FO_POINT:
		case FO_NONE:
			switch(mMipFilter)
			{
			case FO_ANISOTROPIC:
			case FO_LINEAR:
				// nearest min, linear mip
				return GL_NEAREST_MIPMAP_LINEAR;
			case FO_POINT:
				// nearest min, point mip
				return GL_NEAREST_MIPMAP_NEAREST;
			case FO_NONE:
				// nearest min, no mip
				return GL_NEAREST;
			}
			break;
		}

		// should never get here
		return 0;

	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setTextureUnitFiltering(size_t unit, 
		FilterType ftype, FilterOptions fo)
	{
		if (!activateGLTextureUnit(unit))
			return;
		switch(ftype)
		{
		case FT_MIN:
			mMinFilter = fo;
			// Combine with existing mip filter
			glTexParameteri(
				mTextureTypes[unit],
				GL_TEXTURE_MIN_FILTER, 
				getCombinedMinMipFilter());
			break;
		case FT_MAG:
			switch (fo)
			{
			case FO_ANISOTROPIC: // GL treats linear and aniso the same
			case FO_LINEAR:
				glTexParameteri(
					mTextureTypes[unit],
					GL_TEXTURE_MAG_FILTER, 
					GL_LINEAR);
				break;
			case FO_POINT:
			case FO_NONE:
				glTexParameteri(
					mTextureTypes[unit],
					GL_TEXTURE_MAG_FILTER, 
					GL_NEAREST);
				break;
			}
			break;
		case FT_MIP:
			mMipFilter = fo;
			// Combine with existing min filter
			glTexParameteri(
				mTextureTypes[unit],
				GL_TEXTURE_MIN_FILTER, 
				getCombinedMinMipFilter());
			break;
		}

		activateGLTextureUnit(0);
	}
	//---------------------------------------------------------------------
	GLfloat GLRenderSystem::_getCurrentAnisotropy(size_t unit)
	{
		GLfloat curAniso = 0;
		glGetTexParameterfv(mTextureTypes[unit], 
			GL_TEXTURE_MAX_ANISOTROPY_EXT, &curAniso);
		return curAniso ? curAniso : 1;
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setTextureLayerAnisotropy(size_t unit, unsigned int maxAnisotropy)
	{
		if (!mCurrentCapabilities->hasCapability(RSC_ANISOTROPY))
			return;

		if (!activateGLTextureUnit(unit))
			return;

		GLfloat largest_supported_anisotropy = 0;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
		if (maxAnisotropy > largest_supported_anisotropy)
			maxAnisotropy = largest_supported_anisotropy ? 
			static_cast<UINT32>(largest_supported_anisotropy) : 1;
		if (_getCurrentAnisotropy(unit) != maxAnisotropy)
			glTexParameterf(mTextureTypes[unit], GL_TEXTURE_MAX_ANISOTROPY_EXT, (float)maxAnisotropy);

		activateGLTextureUnit(0);
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setVertexDeclaration(VertexDeclarationPtr decl)
	{
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setVertexBufferBinding(VertexBufferBinding* binding)
	{
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::render(const RenderOperation& op)
	{
		// Call super class
		RenderSystem::render(op);

		void* pBufferData = 0;
		bool multitexturing = (getCapabilities()->getNumTextureUnits() > 1);


        const VertexDeclaration::VertexElementList& decl = 
            op.vertexData->vertexDeclaration->getElements();
        VertexDeclaration::VertexElementList::const_iterator elem, elemEnd;
        elemEnd = decl.end();
		vector<GLuint>::type attribsBound;

		for (elem = decl.begin(); elem != elemEnd; ++elem)
		{
			if (!op.vertexData->vertexBufferBinding->isBufferBound(elem->getSource()))
				continue; // skip unbound elements

			HardwareVertexBufferPtr vertexBuffer = 
				op.vertexData->vertexBufferBinding->getBuffer(elem->getSource());
			if(mCurrentCapabilities->hasCapability(RSC_VBO))
			{
				glBindBufferARB(GL_ARRAY_BUFFER_ARB, 
					static_cast<const GLHardwareVertexBuffer*>(vertexBuffer.get())->getGLBufferId());
				pBufferData = VBO_BUFFER_OFFSET(elem->getOffset());
			}
			else
			{
				pBufferData = static_cast<const GLDefaultHardwareVertexBuffer*>(vertexBuffer.get())->getDataPtr(elem->getOffset());
			}
			if (op.vertexData->vertexStart)
			{
				pBufferData = static_cast<char*>(pBufferData) + op.vertexData->vertexStart * vertexBuffer->getVertexSize();
			}

			unsigned int i = 0;
			VertexElementSemantic sem = elem->getSemantic();
 
 			bool isCustomAttrib = false;
 			if (mCurrentVertexProgram)
 				isCustomAttrib = mCurrentVertexProgram->isAttributeValid(sem, elem->getIndex());
 
 			// Custom attribute support
 			// tangents, binormals, blendweights etc always via this route
 			// builtins may be done this way too
 			if (isCustomAttrib)
 			{
 				GLint attrib = mCurrentVertexProgram->getAttributeIndex(sem, elem->getIndex());
				unsigned short typeCount = VertexElement::getTypeCount(elem->getType());
				GLboolean normalised = GL_FALSE;
				switch(elem->getType())
				{
				case VET_COLOUR:
				case VET_COLOUR_ABGR:
				case VET_COLOUR_ARGB:
					// Because GL takes these as a sequence of single unsigned bytes, count needs to be 4
					// VertexElement::getTypeCount treats them as 1 (RGBA)
					// Also need to normalise the fixed-point data
					typeCount = 4;
					normalised = GL_TRUE;
					break;
				default:
					break;
				};

 				glVertexAttribPointerARB(
 					attrib,
 					typeCount, 
  					GLHardwareBufferManager::getGLType(elem->getType()), 
 					normalised, 
  					static_cast<GLsizei>(vertexBuffer->getVertexSize()), 
  					pBufferData);
 				glEnableVertexAttribArrayARB(attrib);
 
 				attribsBound.push_back(attrib);
 			}
 			else
 			{
 				// fixed-function & builtin attribute support
 				switch(sem)
  				{
 				case VES_POSITION:
 					glVertexPointer(VertexElement::getTypeCount(
 						elem->getType()), 
  						GLHardwareBufferManager::getGLType(elem->getType()), 
  						static_cast<GLsizei>(vertexBuffer->getVertexSize()), 
  						pBufferData);
 					glEnableClientState( GL_VERTEX_ARRAY );
 					break;
 				case VES_NORMAL:
 					glNormalPointer(
 						GLHardwareBufferManager::getGLType(elem->getType()), 
  						static_cast<GLsizei>(vertexBuffer->getVertexSize()), 
  						pBufferData);
 					glEnableClientState( GL_NORMAL_ARRAY );
 					break;
 				case VES_DIFFUSE:
 					glColorPointer(4, 
  						GLHardwareBufferManager::getGLType(elem->getType()), 
  						static_cast<GLsizei>(vertexBuffer->getVertexSize()), 
  						pBufferData);
 					glEnableClientState( GL_COLOR_ARRAY );
 					break;
 				case VES_SPECULAR:
 					if (GLEW_EXT_secondary_color)
 					{
 						glSecondaryColorPointerEXT(4, 
 							GLHardwareBufferManager::getGLType(elem->getType()), 
 							static_cast<GLsizei>(vertexBuffer->getVertexSize()), 
 							pBufferData);
 						glEnableClientState( GL_SECONDARY_COLOR_ARRAY );
 					}
 					break;
 				case VES_TEXTURE_COORDINATES:
  
 					if (mCurrentVertexProgram)
 					{
 						// Programmable pipeline - direct UV assignment
 						glClientActiveTextureARB(GL_TEXTURE0 + elem->getIndex());
 						glTexCoordPointer(
 							VertexElement::getTypeCount(elem->getType()), 
 							GLHardwareBufferManager::getGLType(elem->getType()),
 							static_cast<GLsizei>(vertexBuffer->getVertexSize()), 
 							pBufferData);
 						glEnableClientState( GL_TEXTURE_COORD_ARRAY );
 					}
 					else
 					{
 						// fixed function matching to units based on tex_coord_set
 						for (i = 0; i < mDisabledTexUnitsFrom; i++)
 						{
 							// Only set this texture unit's texcoord pointer if it
 							// is supposed to be using this element's index
 							if (mTextureCoordIndex[i] == elem->getIndex() && i < mFixedFunctionTextureUnits)
 							{
								if (multitexturing)
 								glClientActiveTextureARB(GL_TEXTURE0 + i);
 								glTexCoordPointer(
 									VertexElement::getTypeCount(elem->getType()), 
 									GLHardwareBufferManager::getGLType(elem->getType()),
 									static_cast<GLsizei>(vertexBuffer->getVertexSize()), 
 									pBufferData);
 								glEnableClientState( GL_TEXTURE_COORD_ARRAY );
 							}
 						}
 					}
 					break;
 				default:
 					break;
 				};
 			} // isCustomAttrib
  
        }

		if (multitexturing)
		glClientActiveTextureARB(GL_TEXTURE0);

		// Find the correct type to render
		GLint primType;
		//Use adjacency if there is a geometry program and it requested adjacency info
		bool useAdjacency = (mGeometryProgramBound && mCurrentGeometryProgram->isAdjacencyInfoRequired());
		switch (op.operationType)
		{
		case RenderOperation::OT_POINT_LIST:
			primType = GL_POINTS;
			break;
		case RenderOperation::OT_LINE_LIST:
			primType = useAdjacency ? GL_LINES_ADJACENCY_EXT : GL_LINES;
			break;
		case RenderOperation::OT_LINE_STRIP:
			primType = useAdjacency ? GL_LINE_STRIP_ADJACENCY_EXT : GL_LINE_STRIP;
			break;
		default:
		case RenderOperation::OT_TRIANGLE_LIST:
			primType = useAdjacency ? GL_TRIANGLES_ADJACENCY_EXT : GL_TRIANGLES;
			break;
		case RenderOperation::OT_TRIANGLE_STRIP:
			primType = useAdjacency ? GL_TRIANGLE_STRIP_ADJACENCY_EXT : GL_TRIANGLE_STRIP;
			break;
		case RenderOperation::OT_TRIANGLE_FAN:
			primType = GL_TRIANGLE_FAN;
			break;
		}

		if (op.useIndexes)
		{
			if(mCurrentCapabilities->hasCapability(RSC_VBO))
			{
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 
					static_cast<GLHardwareIndexBuffer*>(
					op.indexData->indexBuffer.get())->getGLBufferId());

				pBufferData = VBO_BUFFER_OFFSET(
					op.indexData->indexStart * op.indexData->indexBuffer->getIndexSize());
			}
			else
			{
				pBufferData = static_cast<GLDefaultHardwareIndexBuffer*>(
					op.indexData->indexBuffer.get())->getDataPtr(
					op.indexData->indexStart * op.indexData->indexBuffer->getIndexSize());
			}

			GLenum indexType = (op.indexData->indexBuffer->getType() == HardwareIndexBuffer::IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

			glDrawElements(primType, op.indexData->indexCount, indexType, pBufferData);
		}
		else
		{
			glDrawArrays(primType, 0, op.vertexData->vertexCount);
		}

        glDisableClientState( GL_VERTEX_ARRAY );
		// only valid up to GL_MAX_TEXTURE_UNITS, which is recorded in mFixedFunctionTextureUnits
		if (multitexturing)
        {
			for (int i = 0; i < mFixedFunctionTextureUnits; i++)
			{
            glClientActiveTextureARB(GL_TEXTURE0 + i);
				glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			}
			glClientActiveTextureARB(GL_TEXTURE0);
		}
		else
		{
            glDisableClientState( GL_TEXTURE_COORD_ARRAY );
        }
        glDisableClientState( GL_NORMAL_ARRAY );
        glDisableClientState( GL_COLOR_ARRAY );
		if (GLEW_EXT_secondary_color)
		{
			glDisableClientState( GL_SECONDARY_COLOR_ARRAY );
		}
 		// unbind any custom attributes
		for (vector<GLuint>::type::iterator ai = attribsBound.begin(); ai != attribsBound.end(); ++ai)
 		{
 			glDisableVertexAttribArrayARB(*ai); 
 
  		}
		
		glColor4f(1,1,1,1);
		if (GLEW_EXT_secondary_color)
		{
			glSecondaryColor3fEXT(0.0f, 0.0f, 0.0f);
		}

	}
	//---------------------------------------------------------------------
	void GLRenderSystem::bindGpuProgram(GpuProgram* prg)
	{
		GLGpuProgram* glprg = static_cast<GLGpuProgram*>(prg);

		// Unbind previous gpu program first.
		//
		// Note:
		//  1. Even if both previous and current are the same object, we can't
		//     bypass re-bind completely since the object itself maybe modified.
		//     But we can bypass unbind based on the assumption that object
		//     internally GL program type shouldn't be changed after it has
		//     been created. The behavior of bind to a GL program type twice
		//     should be same as unbind and rebind that GL program type, even
		//     for difference objects.
		//  2. We also assumed that the program's type (vertex or fragment) should
		//     not be changed during it's in using. If not, the following switch
		//     statement will confuse GL state completely, and we can't fix it
		//     here. To fix this case, we must coding the program implementation
		//     itself, if type is changing (during load/unload, etc), and it's inuse,
		//     unbind and notify render system to correct for its state.
		//
		switch (glprg->getType())
		{
		case GPT_VERTEX_PROGRAM:
			if (mCurrentVertexProgram != glprg)
			{
				if (mCurrentVertexProgram)
					mCurrentVertexProgram->unbindProgram();
				mCurrentVertexProgram = glprg;
			}
			break;

		case GPT_FRAGMENT_PROGRAM:
			if (mCurrentFragmentProgram != glprg)
			{
				if (mCurrentFragmentProgram)
					mCurrentFragmentProgram->unbindProgram();
				mCurrentFragmentProgram = glprg;
			}
			break;
		case GPT_GEOMETRY_PROGRAM:
			if (mCurrentGeometryProgram != glprg)
			{
				if (mCurrentGeometryProgram)
					mCurrentGeometryProgram->unbindProgram();
				mCurrentGeometryProgram = glprg;
			}
			break;
		}

		// Bind the program
		glprg->bindProgram();

		RenderSystem::bindGpuProgram(prg);
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::unbindGpuProgram(GpuProgramType gptype)
	{

		if (gptype == GPT_VERTEX_PROGRAM && mCurrentVertexProgram)
		{
			mActiveVertexGpuProgramParameters = nullptr;
			mCurrentVertexProgram->unbindProgram();
			mCurrentVertexProgram = 0;
		}
		else if (gptype == GPT_GEOMETRY_PROGRAM && mCurrentGeometryProgram)
		{
			mActiveGeometryGpuProgramParameters = nullptr;
			mCurrentGeometryProgram->unbindProgram();
			mCurrentGeometryProgram = 0;
		}
		else if (gptype == GPT_FRAGMENT_PROGRAM && mCurrentFragmentProgram)
		{
			mActiveFragmentGpuProgramParameters = nullptr;
			mCurrentFragmentProgram->unbindProgram();
			mCurrentFragmentProgram = 0;
		}
		RenderSystem::unbindGpuProgram(gptype);

	}
	//---------------------------------------------------------------------
	void GLRenderSystem::bindGpuProgramParameters(GpuProgramType gptype, GpuProgramParametersSharedPtr params, UINT16 mask)
	{
		// Set textures
		const GpuNamedConstants& consts = params->getConstantDefinitions();
		for(auto iter = consts.map.begin(); iter != consts.map.end(); ++iter)
		{
			const GpuConstantDefinition& def = iter->second;

			if(def.variability & mask)
			{
				if(def.constType == GCT_SAMPLER2D || def.constType == GCT_SAMPLERCUBE || def.constType == GCT_SAMPLER1D 
					|| def.constType == GCT_SAMPLER2DSHADOW || def.constType == GCT_SAMPLER3D || def.constType == GCT_SAMPLER1DSHADOW)
				{
					TextureRef curTexture = params->getTexture(def.physicalIndex);
					setTexture(def.physicalIndex, true, curTexture.getInternalPtr());

					const SamplerState& samplerState = params->getSamplerState(def.physicalIndex);

					setTextureUnitSettings(def.physicalIndex, curTexture.getInternalPtr(), samplerState);
				}
			}
		}

		switch (gptype)
		{
		case GPT_VERTEX_PROGRAM:
			mActiveVertexGpuProgramParameters = params;
			mCurrentVertexProgram->bindProgramParameters(params, mask);
			break;
		case GPT_GEOMETRY_PROGRAM:
			mActiveGeometryGpuProgramParameters = params;
			mCurrentGeometryProgram->bindProgramParameters(params, mask);
			break;
		case GPT_FRAGMENT_PROGRAM:
			mActiveFragmentGpuProgramParameters = params;
			mCurrentFragmentProgram->bindProgramParameters(params, mask);
			break;
		}
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setClipPlanesImpl(const PlaneList& clipPlanes)
	{
		// A note on GL user clipping:
		// When an ARB vertex program is enabled in GL, user clipping is completely
		// disabled. There is no way around this, it's just turned off.
		// When using GLSL, user clipping can work but you have to include a 
		// glClipVertex command in your vertex shader. 
		// Thus the planes set here may not actually be respected.


		size_t i = 0;
		size_t numClipPlanes;
		GLdouble clipPlane[4];

		// Save previous modelview
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		// just load view matrix (identity world)
		GLfloat mat[16];
		makeGLMatrix(mat, mViewMatrix);
		glLoadMatrixf(mat);

		numClipPlanes = clipPlanes.size();
		for (i = 0; i < numClipPlanes; ++i)
		{
			GLenum clipPlaneId = static_cast<GLenum>(GL_CLIP_PLANE0 + i);
			const Plane& plane = clipPlanes[i];

			if (i >= 6/*GL_MAX_CLIP_PLANES*/)
			{
				CM_EXCEPT(RenderingAPIException, "Unable to set clip plane");
			}

			clipPlane[0] = plane.normal.x;
			clipPlane[1] = plane.normal.y;
			clipPlane[2] = plane.normal.z;
			clipPlane[3] = plane.d;

			glClipPlane(clipPlaneId, clipPlane);
			glEnable(clipPlaneId);
		}

		// disable remaining clip planes
		for ( ; i < 6/*GL_MAX_CLIP_PLANES*/; ++i)
		{
			glDisable(static_cast<GLenum>(GL_CLIP_PLANE0 + i));
		}

		// restore matrices
		glPopMatrix();
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setScissorTest(bool enabled, size_t left, 
		size_t top, size_t right, size_t bottom)
	{
		// If request texture flipping, use "upper-left", otherwise use "lower-left"
		bool flipping = mActiveRenderTarget->requiresTextureFlipping();
		//  GL measures from the bottom, not the top
		size_t targetHeight = mActiveRenderTarget->getHeight();
		// Calculate the "lower-left" corner of the viewport
        GLsizei x = 0, y = 0, w = 0, h = 0;

		if (enabled)
		{
			glEnable(GL_SCISSOR_TEST);
			// NB GL uses width / height rather than right / bottom
			x = left;
			if (flipping)
				y = top;
			else
				y = targetHeight - bottom;
			w = right - left;
			h = bottom - top;
			glScissor(x, y, w, h);
		}
		else
		{
			glDisable(GL_SCISSOR_TEST);
			// GL requires you to reset the scissor when disabling
			w = mActiveViewport->getActualWidth();
			h = mActiveViewport->getActualHeight();
			x = mActiveViewport->getActualLeft();
			if (flipping)
				y = mActiveViewport->getActualTop();
			else
				y = targetHeight - mActiveViewport->getActualTop() - h;
			glScissor(x, y, w, h);
		}
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::clearFrameBuffer(unsigned int buffers, 
		const Color& colour, float depth, unsigned short stencil)
	{
		bool colourMask = !mColourWrite[0] || !mColourWrite[1] 
		|| !mColourWrite[2] || !mColourWrite[3]; 

		GLbitfield flags = 0;
		if (buffers & FBT_COLOUR)
		{
			flags |= GL_COLOR_BUFFER_BIT;
			// Enable buffer for writing if it isn't
			if (colourMask)
			{
				glColorMask(true, true, true, true);
			}
			glClearColor(colour.r, colour.g, colour.b, colour.a);
		}
		if (buffers & FBT_DEPTH)
		{
			flags |= GL_DEPTH_BUFFER_BIT;
			// Enable buffer for writing if it isn't
			if (!mDepthWrite)
			{
				glDepthMask( GL_TRUE );
			}
			glClearDepth(depth);
		}
		if (buffers & FBT_STENCIL)
		{
			flags |= GL_STENCIL_BUFFER_BIT;
			// Enable buffer for writing if it isn't
			glStencilMask(0xFFFFFFFF);

			glClearStencil(stencil);
		}

		// Should be enable scissor test due the clear region is
		// relied on scissor box bounds.
		GLboolean scissorTestEnabled = glIsEnabled(GL_SCISSOR_TEST);
		if (!scissorTestEnabled)
		{
			glEnable(GL_SCISSOR_TEST);
		}

		// Sets the scissor box as same as viewport
        GLint viewport[4] = { 0, 0, 0, 0 };
        GLint scissor[4] = { 0, 0, 0, 0 };
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetIntegerv(GL_SCISSOR_BOX, scissor);
		bool scissorBoxDifference =
			viewport[0] != scissor[0] || viewport[1] != scissor[1] ||
			viewport[2] != scissor[2] || viewport[3] != scissor[3];
		if (scissorBoxDifference)
		{
			glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
		}

		// Clear buffers
		glClear(flags);

		// Restore scissor box
		if (scissorBoxDifference)
		{
			glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
		}
		// Restore scissor test
		if (!scissorTestEnabled)
		{
			glDisable(GL_SCISSOR_TEST);
		}

		// Reset buffer write state
		if (!mDepthWrite && (buffers & FBT_DEPTH))
		{
			glDepthMask( GL_FALSE );
		}
		if (colourMask && (buffers & FBT_COLOUR))
		{
			glColorMask(mColourWrite[0], mColourWrite[1], mColourWrite[2], mColourWrite[3]);
		}
		if (buffers & FBT_STENCIL)
		{
			glStencilMask(mStencilMask);
		}
	}
	//---------------------------------------------------------------------
	float GLRenderSystem::getHorizontalTexelOffset(void)
	{
		// No offset in GL
		return 0.0f;
	}
	//---------------------------------------------------------------------
	float GLRenderSystem::getVerticalTexelOffset(void)
	{
		// No offset in GL
		return 0.0f;
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::_oneTimeContextInitialization()
	{
		if (GLEW_VERSION_1_2)
		{
		// Set nicer lighting model -- d3d9 has this by default
		glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);        
		}
		if (GLEW_VERSION_1_4)
		{
		glEnable(GL_COLOR_SUM);
		glDisable(GL_DITHER);
		}

		// Check for FSAA
		// Enable the extension if it was enabled by the GLSupport
		if (mGLSupport->checkExtension("GL_ARB_multisample"))
		{
			int fsaa_active = false;
			glGetIntegerv(GL_SAMPLE_BUFFERS_ARB,(GLint*)&fsaa_active);
			if(fsaa_active)
			{
				glEnable(GL_MULTISAMPLE_ARB);
			}            
		}
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::_switchContext(GLContext *context)
	{
		// Unbind GPU programs and rebind to new context later, because
		// scene manager treat render system as ONE 'context' ONLY, and it
		// cached the GPU programs using state.
		if (mCurrentVertexProgram)
			mCurrentVertexProgram->unbindProgram();
		if (mCurrentGeometryProgram)
			mCurrentGeometryProgram->unbindProgram();
		if (mCurrentFragmentProgram)
			mCurrentFragmentProgram->unbindProgram();

		// Disable lights
		mCurrentLights = 0;

		// Disable textures
		disableTextureUnitsFrom(0);

		// It's ready for switching
		if (mCurrentContext)
			mCurrentContext->endCurrent();
		mCurrentContext = context;
		mCurrentContext->setCurrent();

		// Check if the context has already done one-time initialisation
		if(!mCurrentContext->getInitialized()) 
		{
			_oneTimeContextInitialization();
			mCurrentContext->setInitialized();
		}

		// Rebind GPU programs to new context
		if (mCurrentVertexProgram)
			mCurrentVertexProgram->bindProgram();
		if (mCurrentGeometryProgram)
			mCurrentGeometryProgram->bindProgram();
		if (mCurrentFragmentProgram)
			mCurrentFragmentProgram->bindProgram();

		// Must reset depth/colour write mask to according with user desired, otherwise,
		// clearFrameBuffer would be wrong because the value we are recorded may be
		// difference with the really state stored in GL context.
		glDepthMask(mDepthWrite);
		glColorMask(mColourWrite[0], mColourWrite[1], mColourWrite[2], mColourWrite[3]);
		glStencilMask(mStencilMask);

	}
	//---------------------------------------------------------------------
	void GLRenderSystem::setRenderTarget(RenderTarget *target)
	{
		// Unbind frame buffer object
		if(mActiveRenderTarget)
			GLRTTManager::instancePtr()->unbind(mActiveRenderTarget);

		mActiveRenderTarget = target;

		// Switch context if different from current one
		GLContext *newContext = 0;
		target->getCustomAttribute("GLCONTEXT", &newContext);
		if(newContext && mCurrentContext != newContext) 
		{
			_switchContext(newContext);
		}

		// Bind frame buffer object
		GLRTTManager::instancePtr()->bind(target);

		if (GLEW_EXT_framebuffer_sRGB)
		{
		// Enable / disable sRGB states
		if (target->isHardwareGammaEnabled())
		{
			glEnable(GL_FRAMEBUFFER_SRGB_EXT);
			
			// Note: could test GL_FRAMEBUFFER_SRGB_CAPABLE_EXT here before
			// enabling, but GL spec says incapable surfaces ignore the setting
			// anyway. We test the capability to enable isHardwareGammaEnabled.
		}
		else
		{
			glDisable(GL_FRAMEBUFFER_SRGB_EXT);
		}
	}
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::_unregisterContext(GLContext *context)
	{
		if(mCurrentContext == context) {
			// Change the context to something else so that a valid context
			// remains active. When this is the main context being unregistered,
			// we set the main context to 0.
			if(mCurrentContext != mMainContext) {
				_switchContext(mMainContext);
			} else {
				/// No contexts remain
				mCurrentContext->endCurrent();
				mCurrentContext = 0;
				mMainContext = 0;
			}
		}
	}
	//---------------------------------------------------------------------
	float GLRenderSystem::getMinimumDepthInputValue(void)
	{
		// Range [-1.0f, 1.0f]
		return -1.0f;
	}
	//---------------------------------------------------------------------
	float GLRenderSystem::getMaximumDepthInputValue(void)
	{
		// Range [-1.0f, 1.0f]
		return 1.0f;
	}
	//---------------------------------------------------------------------
	void GLRenderSystem::registerThread()
	{
		CM_LOCK_MUTEX(mThreadInitMutex)
		// This is only valid once we've created the main context
		if (!mMainContext)
		{
			CM_EXCEPT(InvalidParametersException, 
				"Cannot register a background thread before the main context has been created.");
		}

		// Create a new context for this thread. Cloning from the main context
		// will ensure that resources are shared with the main context
		// We want a separate context so that we can safely create GL
		// objects in parallel with the main thread
		GLContext* newContext = mMainContext->clone();
		mBackgroundContextList.push_back(newContext);

		// Bind this new context to this thread. 
		newContext->setCurrent();

		_oneTimeContextInitialization();
		newContext->setInitialized();


	}
	//---------------------------------------------------------------------
	void GLRenderSystem::unregisterThread()
	{
		// nothing to do here?
		// Don't need to worry about active context, just make sure we delete
		// on shutdown.

	}
	//---------------------------------------------------------------------
	bool GLRenderSystem::activateGLTextureUnit(size_t unit)
	{
		if (mActiveTextureUnit != unit)
		{
			if (GLEW_VERSION_1_2 && unit < getCapabilities()->getNumTextureUnits())
			{
				glActiveTextureARB(GL_TEXTURE0 + unit);
				mActiveTextureUnit = unit;
				return true;
			}
			else if (!unit)
			{
				// always ok to use the first unit
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	//---------------------------------------------------------------------
	unsigned int GLRenderSystem::getDisplayMonitorCount() const
	{
		return mGLSupport->getDisplayMonitorCount();
	}


}
