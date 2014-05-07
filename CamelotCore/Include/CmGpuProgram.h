#pragma once

#include "CmPrerequisites.h"
#include "CmDrawOps.h"
#include "CmGpuProgramParams.h"
#include "CmResource.h"
#include "CmGpuParamDesc.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Types of programs that may run on GPU.
	 */
	enum GpuProgramType
	{
		GPT_VERTEX_PROGRAM,
		GPT_FRAGMENT_PROGRAM,
		GPT_GEOMETRY_PROGRAM,
		GPT_DOMAIN_PROGRAM,
		GPT_HULL_PROGRAM,
		GPT_COMPUTE_PROGRAM
	};

	/**
	 * @brief	GPU program profiles representing supported
	 *			feature sets.
	 */
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
		GPP_PS_4_1,
		GPP_PS_5_0,
		GPP_VS_1_1,
		GPP_VS_2_0,
		GPP_VS_2_x,
		GPP_VS_2_a,
		GPP_VS_3_0,
		GPP_VS_4_0,
		GPP_VS_4_1,
		GPP_VS_5_0,
		GPP_GS_4_0,
		GPP_GS_4_1,
		GPP_GS_5_0,
		GPP_HS_5_0,
		GPP_DS_5_0,
		GPP_CS_5_0
	};

	/**
	 * @brief	Contains a low level GPU program such as vertex or fragment program.
	 *			Internal implementation of this class is render system specific,
	 *			but will normally store a compiled program.
	 *
	 * @note	For higher level programs see HighLevelGpuProgram.
	 *			Core thread only.
	 */
	class CM_EXPORT GpuProgram : public Resource
	{
	public:
		virtual ~GpuProgram();

        /**
         * @brief	Source used for creating this program.
         */
        virtual const String& getSource() const { return mSource; }
        
		/**
		 * @brief	Type of GPU program (e.g. fragment, vertex)
		 */
        virtual GpuProgramType getType() const { return mType; }

		/**
		 * @brief	Profile of the GPU program (e.g. VS_4_0, VS_5_0)
		 */
		virtual GpuProgramProfile getProfile() const { return mProfile; }

		/**
		 * @brief	Name of the program entry method (e.g. "main")
		 */
		virtual const String& getEntryPoint() const { return mEntryPoint; }

		/**
		 * @brief	Returns a delegate that will be used for actually binding the program to the pipeline.
		 */
        virtual GpuProgramPtr getBindingDelegate() { return std::static_pointer_cast<GpuProgram>(getThisPtr()); }

		/**
		 * @brief	Returns whether this program can be supported on the current renderer and hardware.
		 */
        virtual bool isSupported() const;

		/**
		 * @brief	Sets whether this geometry program requires adjacency information
		 *			from the input primitives.
		 *
		 * @note	Only relevant for geometry programs.
		 */
		virtual void setAdjacencyInfoRequired(bool required) { mNeedsAdjacencyInfo = required; }

		/**
		 * @brief	Returns whether this geometry program requires adjacency information
		 *			from the input primitives.
		 *
		 * @note	Only relevant for geometry programs.
		 */
		virtual bool isAdjacencyInfoRequired(void) const { return mNeedsAdjacencyInfo; }

		/**
		 * @brief	Creates a new parameters object compatible with this program definition. You
		 *			may populate the returned object with actual parameter values and bind it
		 *			to the pipeline to render an object using those values and this program.
		 */
		virtual GpuParamsPtr createParameters();

		/**
		 * @brief	Returns description of all parameters in this GPU program.
		 */
		const GpuParamDesc& getParamDesc() const { return mParametersDesc; }

		/**
		* @brief	Language this shader was created from (e.g. HLSL, GLSL).
		*/
        virtual const String& getLanguage() const;

	protected:
		friend class GpuProgramManager;

		GpuProgram(const String& source, const String& entryPoint, 
			GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, 
			bool isAdjacencyInfoRequired = false);

		/**
		 * @brief	Returns whether required capabilities for this program is supported.
		 */
        bool isRequiredCapabilitiesSupported(void) const;

		/**
		 * @copydoc Resource::calculateSize
		 */
		size_t calculateSize(void) const { return 0; } // TODO 

	protected:
		GpuProgramType mType;
		bool mNeedsAdjacencyInfo;
		String mEntryPoint;
		GpuProgramProfile mProfile;
        String mSource;

		GpuParamDesc mParametersDesc;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class GpuProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}