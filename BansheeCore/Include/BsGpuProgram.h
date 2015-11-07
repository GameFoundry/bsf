#pragma once

#include "BsCorePrerequisites.h"
#include "BsDrawOps.h"
#include "BsCoreObject.h"
#include "BsIReflectable.h"
#include "BsGpuParamDesc.h"

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
		GPP_FS_1_1,
		GPP_FS_1_2,
		GPP_FS_1_3,
		GPP_FS_1_4,
		GPP_FS_2_0,
		GPP_FS_2_x,
		GPP_FS_2_a,
		GPP_FS_2_b,
		GPP_FS_3_0,
		GPP_FS_3_x,
		GPP_FS_4_0,
		GPP_FS_4_1,
		GPP_FS_5_0,
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
	 * @brief	Data describing a GpuProgram.
	 */
	class BS_CORE_EXPORT GpuProgramProperties
	{
	public:
		GpuProgramProperties(const String& source, const String& entryPoint,
			GpuProgramType gptype, GpuProgramProfile profile);

		virtual ~GpuProgramProperties() { }

        /**
         * @brief	Source used for creating this program.
         */
        const String& getSource() const { return mSource; }
        
		/**
		 * @brief	Type of GPU program (e.g. fragment, vertex)
		 */
        GpuProgramType getType() const { return mType; }

		/**
		 * @brief	Profile of the GPU program (e.g. VS_4_0, VS_5_0)
		 */
		GpuProgramProfile getProfile() const { return mProfile; }

		/**
		 * @brief	Name of the program entry method (e.g. "main")
		 */
		const String& getEntryPoint() const { return mEntryPoint; }

	protected:
		friend class GpuProgramRTTI;

		GpuProgramType mType;
		String mEntryPoint;
		GpuProgramProfile mProfile;
		String mSource;
	};

	/**
	 * @brief	Core thread version of a GpuProgram.
	 *
	 * @see	GpuProgram
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT GpuProgramCore : public CoreObjectCore
	{
	public:
		virtual ~GpuProgramCore() { }

		/**
		 * @brief	Returns whether this program can be supported on the current renderer and hardware.
		 */
        virtual bool isSupported() const;

		/**
		 * @brief	Returns true if shader was successfully compiled. 
		 */
		virtual bool isCompiled() const { return mIsCompiled; }

		/**
		 * @brief	Returns an error message returned by the compiler, if the compilation failed.
		 */
		virtual String getCompileErrorMessage() const { return mCompileError; }

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
		virtual bool isAdjacencyInfoRequired() const { return mNeedsAdjacencyInfo; }

		/**
		 * @copydoc	GpuProgram::createParameters
		 */
		virtual SPtr<GpuParamsCore> createParameters();

		/**
		 * @copydoc	GpuProgram::getParamDesc
		 */
		GpuParamDescPtr getParamDesc() const { return mParametersDesc; }

		/**
		 * @brief	Returns properties that contain information about the GPU program.
		 */
		const GpuProgramProperties& getProperties() const { return mProperties; }

		/**
		 * @copydoc	GpuProgram::create
		 */
		static SPtr<GpuProgramCore> create(const String& source, const String& entryPoint, const String& language, GpuProgramType gptype,
			GpuProgramProfile profile, bool requiresAdjacency = false);

	protected:
		GpuProgramCore(const String& source, const String& entryPoint,
			GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired = false);

		/**
		 * @brief	Returns whether required capabilities for this program is supported.
		 */
		bool isRequiredCapabilitiesSupported() const;

		bool mNeedsAdjacencyInfo;

		bool mIsCompiled;
		String mCompileError;

		GpuParamDescPtr mParametersDesc;
		GpuProgramProperties mProperties;
	};

	/**
	 * @brief	Contains a GPU program such as vertex or fragment program which gets
	 *			compiled from the provided source code.
	 *
	 * @note	Sim thread only.
	 */
	class BS_CORE_EXPORT GpuProgram : public IReflectable, public CoreObject
	{
	public:
		virtual ~GpuProgram() { }

		/**
		 * @brief	Returns true if shader was successfully compiled. 
		 *
		 * @note	Only valid after core thread has initialized the program.
		 */
		bool isCompiled() const;

		/**
		 * @brief	Returns an error message returned by the compiler, if the compilation failed.
		 *
		 * @note	Only valid after core thread has initialized the program.
		 */
		String getCompileErrorMessage() const;

		/**
		 * @brief	Creates a new parameters object compatible with this program definition. You
		 *			may populate the returned object with actual parameter values and bind it
		 *			to the pipeline to render an object using those values and this program.
		 *
		 * @note	Only valid after core thread has initialized the program.
		 */
		GpuParamsPtr createParameters();

		/**
		 * @brief	Returns description of all parameters in this GPU program.
		 *
		 * @note	Only valid after core thread has initialized the program.
		 */
		GpuParamDescPtr getParamDesc() const;

		/**
		 * @brief	Retrieves a core implementation of a gpu program usable only from the
		 *			core thread.
		 */
		SPtr<GpuProgramCore> getCore() const;

		/**
		 * @brief	Returns properties that contain information about the GPU program.
		 */
		const GpuProgramProperties& getProperties() const { return mProperties; }

		/**
		 * @brief	Creates a new GPU program using the provided source code. If compilation fails or program is not supported
		 *			"isCompiled" with return false, and you will be able to retrieve the error message via "getCompileErrorMessage".
		 *
		 * @param	source		Source code to compile the shader from.
		 * @param	entryPoint	Name of the entry point function, e.g. "main".
		 * @param	language	Language the source is written in, e.g. "hlsl" or "glsl".
		 * @param	gptype		Type of the program, e.g. vertex or fragment.
		 * @param	profile		Program profile specifying supported feature-set. Must match the type.
		 * @param	requiresAdjacency	If true then adjacency information will be provided when rendering using this program.
		 */
		static GpuProgramPtr create(const String& source, const String& entryPoint, const String& language, GpuProgramType gptype,
			GpuProgramProfile profile, bool requiresAdjacency = false);

	protected:
		friend class GpuProgramManager;

		GpuProgram(const String& source, const String& entryPoint, const String& language,
			GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired = false);

		/**
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

		/**
		 * @copydoc Resource::calculateSize
		 */
		size_t calculateSize() const { return 0; } // TODO 

	protected:
		bool mNeedsAdjacencyInfo;
		String mLanguage;
		GpuProgramProperties mProperties;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class GpuProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}