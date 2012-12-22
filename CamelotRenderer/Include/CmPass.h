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
        // Depth buffer settings
        bool mDepthCheck;
        bool mDepthWrite;
        CompareFunction mDepthFunc;
        float mDepthBiasConstant;
		float mDepthBiasSlopeScale;
		float mDepthBiasPerIteration;

        //-------------------------------------------------------------------------
        // Culling mode
        CullingMode mCullMode;

        //-------------------------------------------------------------------------
		/// Polygon mode
		PolygonMode mPolygonMode;
        //-------------------------------------------------------------------------

		BlendStatePtr mBlendState;
		RasterizerStatePtr mRasterizerState;

		// Vertex program
		GpuProgramHandle mVertexProgram;
		// Fragment program
		GpuProgramHandle mFragmentProgram;
		// Geometry program
		GpuProgramHandle mGeometryProgram;
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

		/** Returns true if this pass has some element of transparency. */
		bool isTransparent(void) const;

		/**
		 * @brief	Sets a blend state used for all active render targets.
		 */
		void setBlendState(BlendStatePtr blendState);
		BlendStatePtr getBlendState() const;

		void setRasterizerState(RasterizerStatePtr rasterizerState);
		RasterizerStatePtr getRasterizerState() const;

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