#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmCommonEnums.h"

namespace BansheeEngine
{
	struct CM_EXPORT SHADER_DATA_PARAM_DESC
	{
		String name;
		String gpuVariableName;
		GpuParamDataType type;
		UINT32 arraySize;
		bool hidden;
		UINT32 elementSize;
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
		String name;
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
		TechniquePtr addTechnique(const String& renderSystem, const String& renderer);
		
		void removeTechnique(UINT32 idx);
		void removeTechnique(TechniquePtr technique);

		UINT32 getNumTechniques() const { return (UINT32)mTechniques.size(); }

		/**
		 * @brief	Gets the best supported technique based on current render and other systems.
		 * 			Throws an exception if not a single technique is supported.
		 */
		TechniquePtr getBestTechnique() const;

		/**
		 * @brief	Registers a new parameter you can use for easily setting GpuProgram constants via Material.
		 * 			Only data types may be set using this method. Use the other overload of the method if you want
		 * 			to add object parameters.
		 *
		 * @param	name		   	The name of the parameter.
		 * @param	gpuVariableName	Name of the GPU variable in the GpuProgram that the parameter corresponds with.
		 * @param	type		   	The type of the parameter, must be the same as the type in GpuProgram.
		 * @param	arraySize	   	(optional) If the parameter is an array, the number of elements in the array. Size of 1 means its not an array.
		 * @param	elementSize	   	(optional) Size of an individual element in the array, in bytes. You only need to set this if you are setting variable
		 * 							length parameters, like structs.
		 * @param	hidden		   	(optional) Property that is not directly used by the material system, but can be useful if you need to mark certain parameters
		 * 							as hidden to some system. (e.g. hiding internal engine-managed parameters from the user in the Editor)
		 */
		void addParameter(const String& name, const String& gpuVariableName, GpuParamDataType type, UINT32 arraySize = 1, UINT32 elementSize = 0, bool hidden = false);
		void addParameter(const String& name, const String& gpuVariableName, GpuParamObjectType type, bool hidden = false);
		void removeParameter(const String& name);
		void setParamBlockAttribs(const String& name, bool shared, GpuParamBlockUsage usage);

		GpuParamType getParamType(const String& name) const;
		const SHADER_DATA_PARAM_DESC& getDataParamDesc(const String& name) const;
		const SHADER_OBJECT_PARAM_DESC& getObjectParamDesc(const String& name) const;

		bool hasDataParam(const String& name) const;
		bool hasObjectParam(const String& name) const;

		const Map<String, SHADER_DATA_PARAM_DESC>::type& getDataParams() const { return mDataParams; }
		const Map<String, SHADER_OBJECT_PARAM_DESC>::type& getObjectParams() const { return mObjectParams; }
		const Map<String, SHADER_PARAM_BLOCK_DESC>::type& getParamBlocks() const { return mParamBlocks; }

		static bool isSampler(GpuParamObjectType type);
		static bool isTexture(GpuParamObjectType type);
		static bool isBuffer(GpuParamObjectType type);

		static ShaderPtr create(const String& name);
	private:
		String mName;
		Vector<TechniquePtr>::type mTechniques;

		Map<String, SHADER_DATA_PARAM_DESC>::type mDataParams;
		Map<String, SHADER_OBJECT_PARAM_DESC>::type mObjectParams;
		Map<String, SHADER_PARAM_BLOCK_DESC>::type mParamBlocks;

		Shader(const String& name);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
	public:
		friend class ShaderRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}