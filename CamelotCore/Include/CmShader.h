#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"

namespace BansheeEngine
{
	/**
	 * @brief Describes a single data (int, Vector2, etc.) shader parameter.
	 *
	 * @see	Shader::addParameter.
	 */
	struct CM_EXPORT SHADER_DATA_PARAM_DESC
	{
		String name;
		String gpuVariableName;
		GpuParamDataType type;
		UINT32 arraySize;
		bool hidden;
		UINT32 elementSize;
	};

	/**
	 * @brief Describes a single object (texture, sampler state, etc.) shader parameter.
	 *
	 * @see	Shader::addParameter.
	 */
	struct CM_EXPORT SHADER_OBJECT_PARAM_DESC
	{
		String name;
		String gpuVariableName;
		GpuParamObjectType type;
		bool hidden;
	};

	/**
	 * @brief Describes a shader parameter block.
	 */
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
		/**
		 * @brief	Adds a new technique that supports the provided render system
		 *			and renderer to the shader. It's up to the caller to populate the
		 *			returned object with valid data.
		 */
		TechniquePtr addTechnique(const String& renderSystem, const String& renderer);
		
		/**
		 * @brief	Removes a technique at the specified index.
		 */
		void removeTechnique(UINT32 idx);

		/**
		 * @brief	Removes the specified technique.
		 */
		void removeTechnique(TechniquePtr technique);

		/**
		 * @brief	Returns the total number of techniques in this shader.
		 */
		UINT32 getNumTechniques() const { return (UINT32)mTechniques.size(); }

		/**
		 * @brief	Gets the best supported technique based on current render and other systems.
		 * 			Returns null if not a single technique is supported.
		 */
		TechniquePtr getBestTechnique() const;

		/**
		 * @brief	Registers a new data (int, Vector2, etc.) parameter you that you may then use 
		 *			via Material by providing the parameter name. All parameters internally map to 
		 *			variables defined in GPU programs.
		 *
		 * @param	name		   	The name of the parameter. Name must be unique between all data and object parameters.
		 * @param	gpuVariableName	Name of the GPU variable in the GpuProgram that the parameter corresponds with.
		 * @param	type		   	The type of the parameter, must be the same as the type in GpuProgram.
		 * @param	arraySize	   	(optional) If the parameter is an array, the number of elements in the array. Size of 1 means its not an array.
		 * @param	elementSize	   	(optional) Size of an individual element in the array, in bytes. You only need to set this if you are setting variable
		 * 							length parameters, like structs.
		 * @param	hidden		   	(optional) Property that is not directly used by the material system, but can be useful if you need to mark certain parameters
		 * 							as hidden to some system. (e.g. hiding internal engine-managed parameters from the user in the Editor)
		 */
		void addParameter(const String& name, const String& gpuVariableName, GpuParamDataType type, UINT32 arraySize = 1, UINT32 elementSize = 0, bool hidden = false);

		/**
		 * @brief	Registers a new object (texture, sampler state, etc.) parameter you that you may then use 
		 *			via Material by providing the parameter name. All parameters internally map to variables defined in GPU programs.
		 *
		 * @param	name		   	The name of the parameter. Name must be unique between all data and object parameters.
		 * @param	gpuVariableName	Name of the GPU variable in the GpuProgram that the parameter corresponds with.
		 * @param	type		   	The type of the parameter, must be the same as the type in GpuProgram.
		 * @param	hidden		   	(optional) Property that is not directly used by the material system, but can be useful if you need to mark certain parameters
		 * 							as hidden to some system. (e.g. hiding internal engine-managed parameters from the user in the Editor)
		 */
		void addParameter(const String& name, const String& gpuVariableName, GpuParamObjectType type, bool hidden = false);

		/**
		 * @brief	Unregister a parameter with the specified name.
		 */
		void removeParameter(const String& name);

		/**
		 * @brief	Changes parameters of a parameter block with the specified name.
		 */
		void setParamBlockAttribs(const String& name, bool shared, GpuParamBlockUsage usage);

		/**
		 * @brief	Returns type of the parameter with the specified name. Throws exception if
		 *			the parameter doesn't exist.
		 */
		GpuParamType getParamType(const String& name) const;

		/**
		 * @brief	Returns description for a data parameter with the specified name. Throws exception if
		 *			the parameter doesn't exist.
		 */
		const SHADER_DATA_PARAM_DESC& getDataParamDesc(const String& name) const;

		/**
		 * @brief	Returns description for an object parameter with the specified name. Throws exception if
		 *			the parameter doesn't exist.
		 */
		const SHADER_OBJECT_PARAM_DESC& getObjectParamDesc(const String& name) const;

		/** 
		 * @brief	Checks if the parameter with the specified name exists, and is a data parameter.
		 */
		bool hasDataParam(const String& name) const;

		/** 
		 * @brief	Checks if the parameter with the specified name exists, and is an object parameter.
		 */
		bool hasObjectParam(const String& name) const;

		/** 
		 * @brief	Returns a map of all data parameters in the shader.
		 * 			
		 * @note	Internal method.
		 */
		const Map<String, SHADER_DATA_PARAM_DESC>& _getDataParams() const { return mDataParams; }

		/** 
		 * @brief	Returns a map of all object parameters in the shader.
		 * 			
		 * @note	Internal method.
		 */
		const Map<String, SHADER_OBJECT_PARAM_DESC>& _getObjectParams() const { return mObjectParams; }

		/** 
		 * @brief	Returns a map of all parameter blocks.
		 * 			
		 * @note	Internal method.
		 */
		const Map<String, SHADER_PARAM_BLOCK_DESC>& _getParamBlocks() const { return mParamBlocks; }

		static bool isSampler(GpuParamObjectType type);
		static bool isTexture(GpuParamObjectType type);
		static bool isBuffer(GpuParamObjectType type);

		/** 
		 * @brief	Returns an empty shader object with the specified name. Caller must register
		 *			techniques with the shader before using it in a Material.
		 */
		static ShaderPtr create(const String& name);
	private:
		String mName;
		Vector<TechniquePtr> mTechniques;

		Map<String, SHADER_DATA_PARAM_DESC> mDataParams;
		Map<String, SHADER_OBJECT_PARAM_DESC> mObjectParams;
		Map<String, SHADER_PARAM_BLOCK_DESC> mParamBlocks;

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