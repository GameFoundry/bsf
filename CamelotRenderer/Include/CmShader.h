#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmCommonEnums.h"

namespace CamelotEngine
{
	struct CM_EXPORT SHADER_DATA_PARAM_DESC
	{
		String name;
		String gpuVariableName;
		GpuParamDataType type;
		UINT32 arraySize;
		bool hidden;
	};

	struct CM_EXPORT SHADER_OBJECT_PARAM_DESC
	{
		String name;
		String gpuVariableName;
		GpuParamObjectType type;
		bool hidden;
	};

	struct CM_EXPORT SHADER_PARAM_BLOCK_DESC
	{
		bool shared;
		GpuParamBlockUsage usage;
	};

	/**
	 * @brief	Shader represents a collection of techniques. They are used in Materials,
	 * 			which can be considered as instances of a Shader. Multiple materials
	 * 			may share the same shader but provide different parameters to it.
	 * 			
	 *			Shader will always choose the first supported technique based on the current render
	 *			system, render manager and other properties. So make sure to add most important techniques
	 *			first so you make sure they are used if they are supported.
	 */
	class CM_EXPORT Shader : public Resource
	{
	public:
		Shader(const String& name);

		/**
		 * @brief	Inherited from Resource.
		 */
		virtual void initialize_internal();

		TechniquePtr addTechnique(const String& renderSystem, const String& renderer);
		
		void removeTechnique(UINT32 idx);
		void removeTechnique(TechniquePtr technique);

		UINT32 getNumTechniques() const { return (UINT32)mTechniques.size(); }

		/**
		 * @brief	Gets the best supported technique based on current render and other systems.
		 * 			Throws an exception if not a single technique is supported.
		 */
		TechniquePtr getBestTechnique() const;

		void addParameter(const String& name, const String& gpuVariableName, GpuParamDataType type, UINT32 arraySize = 1, bool hidden = false);
		void addParameter(const String& name, const String& gpuVariableName, GpuParamObjectType type, bool hidden = false);
		void removeParameter(const String& name);
		void setParamBlockAttribs(const String& name, bool shared, GpuParamBlockUsage usage);

		const map<String, SHADER_DATA_PARAM_DESC>::type& getDataParams() const { return mDataParams; }
		const map<String, SHADER_OBJECT_PARAM_DESC>::type& getObjectParams() const { return mObjectParams; }
		const map<String, SHADER_PARAM_BLOCK_DESC>::type& getParamBlocks() const { return mParamBlocks; }

	private:
		String mName;
		vector<TechniquePtr>::type mTechniques;

		map<String, SHADER_DATA_PARAM_DESC>::type mDataParams;
		map<String, SHADER_OBJECT_PARAM_DESC>::type mObjectParams;
		map<String, SHADER_PARAM_BLOCK_DESC>::type mParamBlocks;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
		/**
		 * @brief	Serialization only
		 */
		Shader() {}

	public:
		friend class ShaderRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}