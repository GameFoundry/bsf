#pragma once

#include "CmPrerequisites.h"
#include "CmCommon.h"
#include "CmColor.h"
#include "CmIReflectable.h"

namespace CamelotEngine
{
	/** Class defining a single pass of a Technique (of a Material), i.e.
        a single rendering call.
    @remarks
        Rendering can be repeated with many passes for more complex effects.
        Each pass is a programmable pass (meaning it does
        use either a vertex and fragment program, or both).
    */
	class CM_EXPORT Pass : public IReflectable
    {
    protected:
        //-------------------------------------------------------------------------
        // Blending factors
        SceneBlendFactor mSourceBlendFactor;
        SceneBlendFactor mDestBlendFactor;
		SceneBlendFactor mSourceBlendFactorAlpha;
		SceneBlendFactor mDestBlendFactorAlpha;

		// Used to determine if separate alpha blending should be used for color and alpha channels
		bool mSeparateBlend;

		//-------------------------------------------------------------------------
		// Blending operations
		SceneBlendOperation mBlendOperation;
		SceneBlendOperation mAlphaBlendOperation;

		// Determines if we should use separate blending operations for color and alpha channels
		bool mSeparateBlendOperation;

        //-------------------------------------------------------------------------
        // Depth buffer settings
        bool mDepthCheck;
        bool mDepthWrite;
        CompareFunction mDepthFunc;
        float mDepthBiasConstant;
		float mDepthBiasSlopeScale;
		float mDepthBiasPerIteration;

        // Colour buffer settings
        bool mColourWrite;

		// Alpha reject settings
		CompareFunction mAlphaRejectFunc;
		unsigned char mAlphaRejectVal;
		bool mAlphaToCoverageEnabled;

        //-------------------------------------------------------------------------

        //-------------------------------------------------------------------------
        // Culling mode
        CullingMode mCullMode;

        //-------------------------------------------------------------------------
		/// Polygon mode
		PolygonMode mPolygonMode;
        //-------------------------------------------------------------------------

		// Vertex program
		GpuProgramHandle mVertexProgram;
		// Fragment program
		GpuProgramHandle mFragmentProgram;
		// Geometry program
		GpuProgramHandle mGeometryProgram;

		// point size, applies when not using per-vertex point size
		float mPointSize;
		float mPointMinSize;
		float mPointMaxSize;
    public:
        /// Default constructor
		Pass();
        /// Copy constructor
        Pass(const Pass& oth );
        /// Operator = overload
        Pass& operator=(const Pass& oth);
        virtual ~Pass();

        /// Returns true if this pass uses a programmable vertex pipeline
        bool hasVertexProgram(void) const { return mVertexProgram != nullptr; }
        /// Returns true if this pass uses a programmable fragment pipeline
        bool hasFragmentProgram(void) const { return mFragmentProgram != nullptr; }
        /// Returns true if this pass uses a programmable geometry pipeline
        bool hasGeometryProgram(void) const { return mGeometryProgram != nullptr; }

        /** Gets the point size of the pass.
		@remarks
			This property determines what point size is used to render a point
			list.
        */
        float getPointSize(void) const;

		/** Sets the point size of this pass.
		@remarks
			This setting allows you to change the size of points when rendering
			a point list, or a list of point sprites. The interpretation of this
			command depends on the Pass::setPointSizeAttenuation option - if it
			is off (the default), the point size is in screen pixels, if it is on,
			it expressed as normalised screen coordinates (1.0 is the height of
			the screen) when the point is at the origin.
		@note
			Some drivers have an upper limit on the size of points they support
			- this can even vary between APIs on the same card! Don't rely on
			point sizes that cause the point sprites to get very large on screen,
			since they may get clamped on some cards. Upper sizes can range from
			64 to 256 pixels.
		*/
		void setPointSize(float ps);

		/** Set the minimum point size, when point attenuation is in use. */
		void setPointMinSize(float min);
		/** Get the minimum point size, when point attenuation is in use. */
		float getPointMinSize(void) const;
		/** Set the maximum point size, when point attenuation is in use.
		@remarks Setting this to 0 indicates the max size supported by the card.
		*/
		void setPointMaxSize(float max);
		/** Get the maximum point size, when point attenuation is in use.
		@remarks 0 indicates the max size supported by the card.
		*/
		float getPointMaxSize(void) const;

        /** Allows very fine control of blending this Pass with the existing contents of the scene.
        @remarks
        Whereas the texture blending operations seen in the TextureUnitState class are concerned with
        blending between texture layers, this blending is about combining the output of the material
        as a whole with the existing contents of the rendering target. This blending therefore allows
        object transparency and other special effects.
        @par
        This version of the method allows complete control over the blending operation, by specifying the
        source and destination blending factors. The result of the blending operation is:
        <span align="center">
        final = (texture * sourceFactor) + (pixel * destFactor)
        </span>
        @par
        Each of the factors is specified as one of a number of options, as specified in the SceneBlendFactor
        enumerated type.
        @param
        sourceFactor The source factor in the above calculation, i.e. multiplied by the texture colour components.
        @param
        destFactor The destination factor in the above calculation, i.e. multiplied by the pixel colour components.
        @note
        This method is applicable for both the fixed-function and programmable pipelines.
        */
        void setSceneBlending( const SceneBlendFactor sourceFactor, const SceneBlendFactor destFactor);

        /** Allows very fine control of blending this Pass with the existing contents of the scene.
        @remarks
        Wheras the texture blending operations seen in the TextureUnitState class are concerned with
        blending between texture layers, this blending is about combining the output of the material
        as a whole with the existing contents of the rendering target. This blending therefore allows
        object transparency and other special effects.
        @par
        This version of the method allows complete control over the blending operation, by specifying the
        source and destination blending factors. The result of the blending operation is:
        <span align="center">
        final = (texture * sourceFactor) + (pixel * destFactor)
        </span>
        @par
        Each of the factors is specified as one of a number of options, as specified in the SceneBlendFactor
        enumerated type.
        @param
        sourceFactor The source factor in the above calculation, i.e. multiplied by the texture colour components.
        @param
        destFactor The destination factor in the above calculation, i.e. multiplied by the pixel colour components.
        @param
        sourceFactorAlpha The alpha source factor in the above calculation, i.e. multiplied by the texture alpha component.
        @param
        destFactorAlpha The alpha destination factor in the above calculation, i.e. multiplied by the pixel alpha component.
		@note
        This method is applicable for both the fixed-function and programmable pipelines.
        */
		void setSeparateSceneBlending( const SceneBlendFactor sourceFactor, const SceneBlendFactor destFactor, const SceneBlendFactor sourceFactorAlpha, const SceneBlendFactor destFactorAlpha );

		/** Return true if this pass uses separate scene blending */
		bool hasSeparateSceneBlending() const;

        /** Retrieves the source blending factor for the material (as set using Materiall::setSceneBlending).
        */
        SceneBlendFactor getSourceBlendFactor() const;

        /** Retrieves the destination blending factor for the material (as set using Materiall::setSceneBlending).
        */
        SceneBlendFactor getDestBlendFactor() const;

	    /** Retrieves the alpha source blending factor for the material (as set using Materiall::setSeparateSceneBlending).
        */
		SceneBlendFactor getSourceBlendFactorAlpha() const;

	    /** Retrieves the alpha destination blending factor for the material (as set using Materiall::setSeparateSceneBlending).
        */
		SceneBlendFactor getDestBlendFactorAlpha() const;

		/** Sets the specific operation used to blend source and destination pixels together.
			@remarks 
			By default this operation is +, which creates this equation
			<span align="center">
			final = (texture * sourceFactor) + (pixel * destFactor)
			</span>
			By setting this to something other than SBO_ADD you can change the operation to achieve
			a different effect.
			@param op The blending operation mode to use for this pass
		*/
		void setSceneBlendingOperation(SceneBlendOperation op);

		/** Sets the specific operation used to blend source and destination pixels together.
			@remarks 
			By default this operation is +, which creates this equation
			<span align="center">
			final = (texture * sourceFactor) + (pixel * destFactor)
			</span>
			By setting this to something other than SBO_ADD you can change the operation to achieve
			a different effect.
			This function allows more control over blending since it allows you to select different blending
			modes for the color and alpha channels
			@param op The blending operation mode to use for color channels in this pass
			@param op The blending operation mode to use for alpha channels in this pass
		*/
		void setSeparateSceneBlendingOperation(SceneBlendOperation op, SceneBlendOperation alphaOp);

		/** Returns true if this pass uses separate scene blending operations. */
		bool hasSeparateSceneBlendingOperations() const;

		/** Returns the current blending operation */
		SceneBlendOperation getSceneBlendingOperation() const;

		/** Returns the current alpha blending operation */
		SceneBlendOperation getSceneBlendingOperationAlpha() const;

		/** Returns true if this pass has some element of transparency. */
		bool isTransparent(void) const;

		/** Sets whether or not this pass renders with depth-buffer checking on or not.
        @remarks
        If depth-buffer checking is on, whenever a pixel is about to be written to the frame buffer
        the depth buffer is checked to see if the pixel is in front of all other pixels written at that
        point. If not, the pixel is not written.
        @par
        If depth checking is off, pixels are written no matter what has been rendered before.
        Also see setDepthFunction for more advanced depth check configuration.
        @see
        setDepthFunction
        */
        void setDepthCheckEnabled(bool enabled);

        /** Returns whether or not this pass renders with depth-buffer checking on or not.
        @see
        setDepthCheckEnabled
        */
        bool getDepthCheckEnabled(void) const;

        /** Sets whether or not this pass renders with depth-buffer writing on or not.
        @remarks
        If depth-buffer writing is on, whenever a pixel is written to the frame buffer
        the depth buffer is updated with the depth value of that new pixel, thus affecting future
        rendering operations if future pixels are behind this one.
        @par
        If depth writing is off, pixels are written without updating the depth buffer Depth writing should
        normally be on but can be turned off when rendering static backgrounds or when rendering a collection
        of transparent objects at the end of a scene so that they overlap each other correctly.
        */
        void setDepthWriteEnabled(bool enabled);

        /** Returns whether or not this pass renders with depth-buffer writing on or not.
        @see
        setDepthWriteEnabled
        */
        bool getDepthWriteEnabled(void) const;

        /** Sets the function used to compare depth values when depth checking is on.
        @remarks
        If depth checking is enabled (see setDepthCheckEnabled) a comparison occurs between the depth
        value of the pixel to be written and the current contents of the buffer. This comparison is
        normally CMPF_LESS_EQUAL, i.e. the pixel is written if it is closer (or at the same distance)
        than the current contents. If you wish you can change this comparison using this method.
        */
        void setDepthFunction( CompareFunction func );
        /** Returns the function used to compare depth values when depth checking is on.
        @see
        setDepthFunction
        */
        CompareFunction getDepthFunction(void) const;

		/** Sets whether or not colour buffer writing is enabled for this Pass.
		@remarks
			For some effects, you might wish to turn off the colour write operation
			when rendering geometry; this means that only the depth buffer will be
			updated (provided you have depth buffer writing enabled, which you
			probably will do, although you may wish to only update the stencil
			buffer for example - stencil buffer state is managed at the RenderSystem
			level only, not the Material since you are likely to want to manage it
			at a higher level).
		*/
		void setColourWriteEnabled(bool enabled);
		/** Determines if colour buffer writing is enabled for this pass. */
		bool getColourWriteEnabled(void) const;

        /** Sets the culling mode for this pass  based on the 'vertex winding'.
        @remarks
        A typical way for the rendering engine to cull triangles is based on the 'vertex winding' of
        triangles. Vertex winding refers to the direction in which the vertices are passed or indexed
        to in the rendering operation as viewed from the camera, and will wither be clockwise or
        anticlockwise (that's 'counterclockwise' for you Americans out there ;) The default is
        CULL_CLOCKWISE i.e. that only triangles whose vertices are passed/indexed in anticlockwise order
        are rendered - this is a common approach and is used in 3D studio models for example. You can
        alter this culling mode if you wish but it is not advised unless you know what you are doing.
        @par
        You may wish to use the CULL_NONE option for mesh data that you cull yourself where the vertex
        winding is uncertain.
        */
        void setCullingMode( CullingMode mode );

        /** Returns the culling mode for geometry rendered with this pass. See setCullingMode for more information.
        */
        CullingMode getCullingMode(void) const;

		/** Sets the type of polygon rendering required
		@note
		The default shading method is Solid
		*/
		void setPolygonMode( PolygonMode mode );

		/** Returns the type of light shading to be used.
		*/
		PolygonMode getPolygonMode(void) const;

        /** Sets the depth bias to be used for this material.
        @remarks
        When polygons are coplanar, you can get problems with 'depth fighting' where
        the pixels from the two polys compete for the same screen pixel. This is particularly
        a problem for decals (polys attached to another surface to represent details such as
        bulletholes etc.).
        @par
        A way to combat this problem is to use a depth bias to adjust the depth buffer value
        used for the decal such that it is slightly higher than the true value, ensuring that
        the decal appears on top. There are two aspects to the biasing, a constant
		bias value and a slope-relative biasing value, which varies according to the
		maximum depth slope relative to the camera, ie:
		<pre>finalBias = maxSlope * slopeScaleBias + constantBias</pre>
		Note that slope scale bias, whilst more accurate, may be ignored by old hardware.
        @param constantBias The constant bias value, expressed as a factor of the
			minimum observable depth
		@param slopeScaleBias The slope-relative bias value, expressed as a factor
			of the depth slope
		*/
        void setDepthBias(float constantBias, float slopeScaleBias = 0.0f);

        /** Retrieves the const depth bias value as set by setDepthBias. */
        float getDepthBiasConstant(void) const;
		/** Retrieves the slope-scale depth bias value as set by setDepthBias. */
		float getDepthBiasSlopeScale(void) const;
		/** Sets a factor which derives an additional depth bias from the number 
			of times a pass is iterated.
		@remarks
			The Final depth bias will be the constant depth bias as set through
			setDepthBias, plus this value times the iteration number. 
		*/
		void setIterationDepthBias(float biasPerIteration);
		/** Gets a factor which derives an additional depth bias from the number 
			of times a pass is iterated.
		*/
		float getIterationDepthBias() const;

        /** Sets the way the pass will have use alpha to totally reject pixels from the pipeline.
        @remarks
			The default is CMPF_ALWAYS_PASS i.e. alpha is not used to reject pixels.
        @param func The comparison which must pass for the pixel to be written.
        @param value 1 byte value against which alpha values will be tested(0-255)
		@param alphaToCoverageEnabled Whether to enable alpha to coverage support
        @note
			This option applies in both the fixed function and the programmable pipeline.
        */
        void setAlphaRejectSettings(CompareFunction func, unsigned char value, bool alphaToCoverageEnabled = false);

		/** Sets the alpha reject function. See setAlphaRejectSettings for more information.
		*/
		void setAlphaRejectFunction(CompareFunction func);

		/** Gets the alpha reject value. See setAlphaRejectSettings for more information.
		*/
		void setAlphaRejectValue(unsigned char val);

		/** Gets the alpha reject function. See setAlphaRejectSettings for more information.
        */
		CompareFunction getAlphaRejectFunction(void) const { return mAlphaRejectFunc; }

        /** Gets the alpha reject value. See setAlphaRejectSettings for more information.
        */
		unsigned char getAlphaRejectValue(void) const { return mAlphaRejectVal; }

		/** Sets whether to use alpha to coverage (A2C) when blending alpha rejected values. 
		@remarks
			Alpha to coverage performs multisampling on the edges of alpha-rejected
			textures to produce a smoother result. It is only supported when multisampling
			is already enabled on the render target, and when the hardware supports
			alpha to coverage (see RenderSystemCapabilities). 
		*/
		void setAlphaToCoverageEnabled(bool enabled);

		/** Gets whether to use alpha to coverage (A2C) when blending alpha rejected values. 
		*/
		bool isAlphaToCoverageEnabled() const { return mAlphaToCoverageEnabled; }

		/** Sets the details of the vertex program to use.
		*/
		void setVertexProgram(GpuProgramHandle gpuProgram);

		/** Gets the vertex program used by this pass, only available after _load(). */
		const GpuProgramHandle& getVertexProgram(void) const;

		/** Sets the details of the fragment program to use.
		*/
		void setFragmentProgram(GpuProgramHandle gpuProgram);
		
		/** Gets the fragment program used by this pass, only available after _load(). */
		const GpuProgramHandle& getFragmentProgram(void) const;

		/** Sets the details of the geometry program to use.
		*/
		void setGeometryProgram(GpuProgramHandle gpuProgram);
		
		/** Gets the geometry program used by this pass, only available after _load(). */
		const GpuProgramHandle& getGeometryProgram(void) const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class PassRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
    };
}