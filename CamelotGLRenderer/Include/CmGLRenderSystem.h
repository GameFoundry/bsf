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
#ifndef __GLRenderSystem_H__
#define __GLRenderSystem_H__

#include "CmGLPrerequisites.h"
#include "CmRenderSystem.h"
#include "CmGLHardwareBufferManager.h"
#include "CmGLGpuProgramManager.h"
#include "CmGLSLProgramFactory.h"
#include "CmConfigOptionMap.h"
#include "CmCgProgramFactory.h"
#include "CmVector4.h"

namespace CamelotEngine {
    /**
      Implementation of GL as a rendering system.
     */
    class CM_RSGL_EXPORT GLRenderSystem : public RenderSystem
    {
    public:
        // Default constructor / destructor
        GLRenderSystem();
        ~GLRenderSystem();

        // ----------------------------------
        // Overridden RenderSystem functions
        // ----------------------------------
        /** See
          RenderSystem
         */
        const String& getName(void) const;

        // -----------------------------
        // Low-level overridden members
        // -----------------------------
		/** See
          RenderSystem
         */
		void createRenderWindow_internal(const String &name, unsigned int width, unsigned int height, 
			bool fullScreen, const NameValuePairList& miscParams, AsyncOp& asyncOp);
        /**
         * Set current render target to target, enabling its GL context if needed
         */
        void setRenderTarget(RenderTarget *target);
		/** See
          RenderSystem
         */
		void bindGpuProgram(GpuProgramHandle prg);
        /** See
          RenderSystem
         */
		void unbindGpuProgram(GpuProgramType gptype);
		/** See
          RenderSystem
         */
		void bindGpuProgramParameters(GpuProgramType gptype, GpuProgramParametersSharedPtr params, UINT16 mask);
		/** See
          RenderSystem
         */
        void setTexture(UINT16 unit, bool enabled, const TexturePtr &tex);
        
		/**
		 * @copydoc RenderSystem::setSamplerState()
		 */
		void setSamplerState(UINT16 unit, const SamplerState& state);

		/**
		 * @copydoc RenderSystem::setBlendState()
		 */
		void setBlendState(const BlendState& blendState);

        /** See
          RenderSystem
         */
        void setViewport(const Viewport& vp);
        /** See
          RenderSystem
         */
        void beginFrame(void);
        /** See
          RenderSystem
         */
        void endFrame(void);
        /** See
          RenderSystem
         */
        void setCullingMode(CullingMode mode);
        /** See
          RenderSystem
         */
        void setDepthBufferParams(bool depthTest = true, bool depthWrite = true, CompareFunction depthFunction = CMPF_LESS_EQUAL);
        /** See
          RenderSystem
         */
        void setDepthBufferCheckEnabled(bool enabled = true);
        /** See
          RenderSystem
         */
        void setDepthBufferWriteEnabled(bool enabled = true);
        /** See
          RenderSystem
         */
        void setDepthBufferFunction(CompareFunction func = CMPF_LESS_EQUAL);
        /** See
          RenderSystem
         */
        void setDepthBias(float constantBias, float slopeScaleBias);
        /** See
          RenderSystem
         */
        void convertProjectionMatrix(const Matrix4& matrix,
            Matrix4& dest, bool forGpuProgram = false);
        /** See
          RenderSystem
         */
        void setPolygonMode(PolygonMode level);
        /** See
          RenderSystem
         */
        void setStencilCheckEnabled(bool enabled);
        /** See
          RenderSystem.
         */
        void setStencilBufferParams(CompareFunction func = CMPF_ALWAYS_PASS, 
            UINT32 refValue = 0, UINT32 mask = 0xFFFFFFFF, 
            StencilOperation stencilFailOp = SOP_KEEP, 
            StencilOperation depthFailOp = SOP_KEEP,
            StencilOperation passOp = SOP_KEEP, 
            bool twoSidedOperation = false);
        /** See
          RenderSystem
         */
		void setVertexDeclaration(VertexDeclarationPtr decl);
        /** See
          RenderSystem
         */
		void setVertexBufferBinding(VertexBufferBinding* binding);
        /** See
          RenderSystem
         */
        void render(const RenderOperation& op);

        /** See
          RenderSystem
         */
        void setScissorTest(bool enabled, UINT32 left = 0, UINT32 top = 0, UINT32 right = 800, UINT32 bottom = 600) ;
        void clearFrameBuffer(unsigned int buffers, 
            const Color& colour = Color::Black, 
            float depth = 1.0f, unsigned short stencil = 0);

		/** See
          RenderSystem
         */
        VertexElementType getColorVertexElementType(void) const;
        float getHorizontalTexelOffset(void);
        float getVerticalTexelOffset(void);
        float getMinimumDepthInputValue(void);
        float getMaximumDepthInputValue(void);

        void _unregisterContext(GLContext *context);

    private:
        /// Rendering loop control
        bool mStopRendering;

        /// View matrix to set world against
        Matrix4 mViewMatrix;
        Matrix4 mWorldMatrix;
        Matrix4 mTextureMatrix;

        /// Last min & mip filtering options, so we can combine them
        FilterOptions mMinFilter;
        FilterOptions mMipFilter;

        /// What texture coord set each texture unit is using
        UINT32 mTextureCoordIndex[CM_MAX_TEXTURE_LAYERS];

        /// Holds texture type settings for every stage
        GLenum mTextureTypes[CM_MAX_TEXTURE_LAYERS];

		/// Number of fixed-function texture units
		unsigned short mFixedFunctionTextureUnits;

        void initConfigOptions(void);
        void initInputDevices(void);
        void processInputDevices(void);

        void makeGLMatrix(GLfloat gl_matrix[16], const Matrix4& m);
 
        GLint getBlendMode(SceneBlendFactor ogreBlend) const;
		GLint getTextureAddressingMode(TextureAddressingMode tam) const;
		void initialiseContext(RenderWindow* primary);

		/** See
          RenderSystem
         */
		virtual RenderSystemCapabilities* createRenderSystemCapabilities() const;
        /** See
          RenderSystem
         */
		void initialiseFromRenderSystemCapabilities(RenderSystemCapabilities* caps, RenderTarget* primary);

        /// Store last depth write state
        bool mDepthWrite;
		/// Store last stencil mask state
		UINT32 mStencilMask;
		/// Store last colour write state
		bool mColourWrite[4];

        GLint convertCompareFunction(CompareFunction func) const;
        GLint convertStencilOp(StencilOperation op, bool invert = false) const;

		/// Internal method for anisotropy validation
		GLfloat _getCurrentAnisotropy(UINT16 unit);
		
        /// GL support class, used for creating windows etc.
        GLSupport* mGLSupport;

        bool mUseAutoTextureMatrix;
        GLfloat mAutoTextureMatrix[16];

        /// Check if the GL system has already been initialised
        bool mGLInitialised;

		GLSLProgramFactory* mGLSLProgramFactory;
		CgProgramFactory* mCgProgramFactory;

        unsigned short mCurrentLights;

        GLuint getCombinedMinMipFilter(void) const;

        GLGpuProgram* mCurrentVertexProgram;
        GLGpuProgram* mCurrentFragmentProgram;
		GLGpuProgram* mCurrentGeometryProgram;

		/* The main GL context - main thread only */
        GLContext *mMainContext;
        /* The current GL context  - main thread only */
        GLContext *mCurrentContext;
		typedef list<GLContext*>::type GLContextList;
		/// List of background thread contexts
		GLContextList mBackgroundContextList;

		UINT16 mActiveTextureUnit;

	protected:
		/** See
          RenderSystem
         */
		void startUp_internal();
		/** See
          RenderSystem
         */
        void shutdown_internal(void);

		void setClipPlanesImpl(const PlaneList& clipPlanes);
		bool activateGLTextureUnit(UINT16 unit);

		/// @copydoc RenderSystem::createMultiRenderTarget
		virtual MultiRenderTarget * createMultiRenderTarget(const String & name); 
		
        /** See
          RenderSystem
         */
        String getErrorDescription(long errorNumber) const;

		/** See
          RenderSystem
         */
        void setClipPlane (UINT16 index, float A, float B, float C, float D);
        /** See
          RenderSystem
         */
        void enableClipPlane (UINT16 index, bool enable);

		/************************************************************************/
		/* 								Sampler states                     		*/
		/************************************************************************/

		/** Sets the texture addressing mode for a texture unit.*/
        void setTextureAddressingMode(UINT16 stage, const UVWAddressingMode& uvw);

		/** Sets the texture border color for a texture unit.*/
        void setTextureBorderColor(UINT16 stage, const Color& color);

		/** Sets the mipmap bias value for a given texture unit.
		@remarks
		This allows you to adjust the mipmap calculation up or down for a
		given texture unit. Negative values force a larger mipmap to be used, 
		positive values force a smaller mipmap to be used. Units are in numbers
		of levels, so +1 forces the mipmaps to one smaller level.
		@note Only does something if render system has capability RSC_MIPMAP_LOD_BIAS.
		*/
		void setTextureMipmapBias(UINT16 unit, float bias);

		/** Sets a single filter for a given texture unit.
		@param unit The texture unit to set the filtering options for
		@param ftype The filter type
		@param filter The filter to be used
		*/
        void setTextureFiltering(UINT16 unit, FilterType ftype, FilterOptions filter);

		/** Sets the maximal anisotropy for the specified texture unit.*/
		void setTextureAnisotropy(UINT16 unit, unsigned int maxAnisotropy);

		/************************************************************************/
		/* 								Blend states                      		*/
		/************************************************************************/

		/** Sets the global blending factors for combining subsequent renders with the existing frame contents.
		The result of the blending operation is:</p>
		<p align="center">final = (texture * sourceFactor) + (pixel * destFactor)</p>
		Each of the factors is specified as one of a number of options, as specified in the SceneBlendFactor
		enumerated type.
		By changing the operation you can change addition between the source and destination pixels to a different operator.
		@param sourceFactor The source factor in the above calculation, i.e. multiplied by the texture colour components.
		@param destFactor The destination factor in the above calculation, i.e. multiplied by the pixel colour components.
		@param op The blend operation mode for combining pixels
		*/
		void setSceneBlending( SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendOperation op );

		/** Sets the global blending factors for combining subsequent renders with the existing frame contents.
		The result of the blending operation is:</p>
		<p align="center">final = (texture * sourceFactor) + (pixel * destFactor)</p>
		Each of the factors is specified as one of a number of options, as specified in the SceneBlendFactor
		enumerated type.
		@param sourceFactor The source factor in the above calculation, i.e. multiplied by the texture colour components.
		@param destFactor The destination factor in the above calculation, i.e. multiplied by the pixel colour components.
		@param sourceFactorAlpha The source factor in the above calculation for the alpha channel, i.e. multiplied by the texture alpha components.
		@param destFactorAlpha The destination factor in the above calculation for the alpha channel, i.e. multiplied by the pixel alpha components.
		@param op The blend operation mode for combining pixels
		@param alphaOp The blend operation mode for combining pixel alpha values
		*/
		void setSceneBlending( SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceFactorAlpha, 
			SceneBlendFactor destFactorAlpha, SceneBlendOperation op, SceneBlendOperation alphaOp );

		/** Sets the global alpha rejection approach for future renders.
		By default images are rendered regardless of texture alpha. This method lets you change that.
		@param func The comparison function which must pass for a pixel to be written.
		@param val The value to compare each pixels alpha value to (0-255)
		*/
		void setAlphaTest(CompareFunction func, unsigned char value);

		/**
		 * @brief	Enable alpha coverage if supported.
		 */
		void setAlphaToCoverage(bool enabled);

		/** Sets whether or not colour buffer writing is enabled, and for which channels. 
		@remarks
		For some advanced effects, you may wish to turn off the writing of certain colour
		channels, or even all of the colour channels so that only the depth buffer is updated
		in a rendering pass. However, the chances are that you really want to use this option
		through the Material class.
		@param red, green, blue, alpha Whether writing is enabled for each of the 4 colour channels. */
		void setColorBufferWriteEnabled(bool red, bool green, bool blue, bool alpha);

		// ----------------------------------
        // GLRenderSystem specific members
        // ----------------------------------
        /** One time initialization for the RenderState of a context. Things that
            only need to be set once, like the LightingModel can be defined here.
         */
        void oneTimeContextInitialization();
        /** Switch GL context, dealing with involved internal cached states too
        */
        void switchContext(GLContext *context);
        /** Unregister a render target->context mapping. If the context of target 
            is the current context, change the context to the main context so it
            can be destroyed safely. 
            
            @note This is automatically called by the destructor of 
            GLContext.
         */

		/** Returns the main context */
		GLContext* _getMainContext() {return mMainContext;} 
    };
}
#endif

