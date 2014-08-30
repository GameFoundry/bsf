#pragma once

#include "BsCorePrerequisites.h"
#include "BsResource.h"

namespace BansheeEngine
{
	/**
	 * @brief	Type of shader dirty flags
	 */
	enum class ShaderDirtyFlag
	{
		Shader = 0x01, /**< Internal shader data is dirty. */
		Proxy = 0x02 /**< Active proxy needs to be updated. */
	};

	/**
	 * @brief Describes a single data (int, Vector2, etc.) shader parameter.
	 *
	 * @see	Shader::addParameter.
	 */
	struct BS_CORE_EXPORT SHADER_DATA_PARAM_DESC
	{
		String name;
		String gpuVariableName;
		GpuParamDataType type;
		UINT32 rendererSemantic;
		UINT32 arraySize;
		UINT32 elementSize;
	};

	/**
	 * @brief Describes a single object (texture, sampler state, etc.) shader parameter.
	 *
	 * @see	Shader::addParameter.
	 */
	struct BS_CORE_EXPORT SHADER_OBJECT_PARAM_DESC
	{
		String name;
		Vector<String> gpuVariableNames;
		UINT32 rendererSemantic;
		GpuParamObjectType type;
	};

	/**
	 * @brief Describes a shader parameter block.
	 */
	struct BS_CORE_EXPORT SHADER_PARAM_BLOCK_DESC
	{
		String name;
		bool shared;
		UINT32 rendererSemantic;
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
	class BS_CORE_EXPORT Shader : public Resource
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
		 * @brief	Sets sorting type to use when performing sort in the render queue. Default value is sort front to back
		 *			which causes least overdraw and is preferable. Transparent objects need to be sorted back to front.
		 *			You may also specify no sorting and the elements will be rendered in the order they were added to the
		 *			render queue.
		 */
		void setQueueSortType(QueueSortType sortType);

		/**
		 * @brief	Sets a priority that allows you to control in what order are your shaders rendered.
		 *			See "QueuePriority" for a list of initial values. Shaders with higher priority will be
		 *			rendered before shaders with lower priority, and additionally render queue will only sort
		 *			elements within the same priority group.
		 *
		 * @note	This is useful when you want all your opaque objects to be rendered before you start
		 *			drawing your transparent ones. Or to render your overlays after everything else. Values
		 *			provided in "QueuePriority" are just for general guidance and feel free to increase them
		 *			or decrease them for finer tuning. (e.g. "QueuePriority::Opaque + 1").
		 */
		void setQueuePriority(UINT32 priority);

		/**
		 * @brief	Enables or disables separable passes. When separable passes are disabled
		 *			all shader passes will be executed in a sequence one after another. If it is disabled
		 *			the renderer is free to mix and match passes from different objects to achieve best
		 *			performance. (They will still be executed in sequence, but some other object may
		 *			be rendered in-between passes)
		 *
		 * @note	Shaders with transparency generally can't be separable, while opaque can.
		 */
		void setAllowSeparablePasses(bool enable);

		/**
		 * @brief	Returns currently active queue sort type.
		 *
		 * @see		setQueueSortType
		 */
		QueueSortType getQueueSortType() const { return mQueueSortType; }

		/**
		 * @brief	Returns currently active queue priority.
		 *
		 * @see		setQueuePriority
		 */
		UINT32 getQueuePriority() const { return mQueuePriority; }

		/**
		 * @brief	Returns if separable passes are allowed.
		 *
		 * @see		setAllowSeparablePasses
		 */
		bool getAllowSeparablePasses() const { return mSeparablePasses; }

		/**
		 * @brief	Registers a new data (int, Vector2, etc.) parameter you that you may then use 
		 *			via Material by providing the parameter name. All parameters internally map to 
		 *			variables defined in GPU programs.
		 *
		 * @param	name		   	 The name of the parameter. Name must be unique between all data and object parameters.
		 * @param	gpuVariableName	 Name of the GPU variable in the GpuProgram that the parameter corresponds with.
		 * @param	type		   	 The type of the parameter, must be the same as the type in GpuProgram.
		 * @param	rendererSemantic (optional) Semantic that allows you to specify the use of this parameter in the renderer. The actual value of the semantic
		 *							 depends on the current Renderer and its supported list of semantics. Elements with renderer semantics should not be updated
		 *							 by the user, and will be updated by the renderer. These semantics will also be used to determine if a shader is compatible
		 *							 with a specific renderer or not. Value of 0 signifies the parameter is not used by the renderer.
		 * @param	arraySize	   	 (optional) If the parameter is an array, the number of elements in the array. Size of 1 means its not an array.
		 * @param	elementSize	   	 (optional) Size of an individual element in the array, in bytes. You only need to set this if you are setting variable
		 * 							 length parameters, like structs.
		 */
		void addParameter(const String& name, const String& gpuVariableName, GpuParamDataType type, UINT32 rendererSemantic = 0,
			UINT32 arraySize = 1, UINT32 elementSize = 0);

		/**
		 * @brief	Registers a new object (texture, sampler state, etc.) parameter you that you may then use 
		 *			via Material by providing the parameter name. All parameters internally map to variables defined in GPU programs.
		 *			Multiple GPU variables may be mapped to a single parameter in which case the first variable actually found in the program
		 *			will be used while others will be ignored.
		 *
		 * @param	name		   	The name of the parameter. Name must be unique between all data and object parameters.
		 * @param	gpuVariableName	Name of the GPU variable in the GpuProgram that the parameter corresponds with.
		 * @param	type		   	The type of the parameter, must be the same as the type in GpuProgram.
		 * @param	rendererSemantic (optional) Semantic that allows you to specify the use of this parameter in the renderer. The actual value of the semantic
		 *							 depends on the current Renderer and its supported list of semantics. Elements with renderer semantics should not be updated
		 *							 by the user, and will be updated by the renderer. These semantics will also be used to determine if a shader is compatible
		 *							 with a specific renderer or not. Value of 0 signifies the parameter is not used by the renderer.
		 *
		 * @note	Mapping multiple GPU variables to a single parameter is useful when you are defining a shader that supports techniques across different render
		 *			systems where GPU variable names for the same parameters might differ.
		 */
		void addParameter(const String& name, const String& gpuVariableName, GpuParamObjectType type, UINT32 rendererSemantic = 0);

		/**
		 * @brief	Unregister a parameter with the specified name.
		 */
		void removeParameter(const String& name);

		/**
		 * @brief	Changes parameters of a parameter block with the specified name.
		 *
		 * @param	name	Name of the parameter block. This should correspond with the name specified in the GPU program code.
		 * @param	shared	If parameter block is marked as shared it will not be automatically created by the Material. You will need
		 *					to create it elsewhere and then assign it manually.
		 * @param	usage	Specified how often do we plan on modifying the buffer, which determines how is the buffer internally stored
		 *					for best performance.
		 * @param	rendererSemantic (optional) Semantic that allows you to specify the use of this parameter block in the renderer. The actual value of the 
		 *							 semantic depends on the current Renderer and its supported list of semantics. Elements with a renderer semantic
		 *							 will not have their parameter block automatically created (similar to "shared" argument), but instead a Renderer will
		 *							 create an assign it instead. Be aware that renderers have strict policies on what and how are parameters stored in the 
		 *							 buffer and you will need to respect them. If you don't respect them your shader will be deemed incompatible and won't be used. 
		 *							 Value of 0 signifies the parameter block is not used by the renderer.
		 */
		void setParamBlockAttribs(const String& name, bool shared, GpuParamBlockUsage usage, UINT32 rendererSemantic = 0);

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

		/************************************************************************/
		/* 								CORE PROXY                      		*/
		/************************************************************************/

		/**
		 * @brief	Checks is the core dirty flag set. This is used by external systems 
		 *			to know when internal data has changed and core thread potentially needs to be notified.
		 */
		bool _isCoreDirty(ShaderDirtyFlag flag) const { return (mCoreDirtyFlags & (UINT32)flag) != 0; }

		/**
		 * @brief	Marks the core dirty flag as clean.
		 */
		void _markCoreClean(ShaderDirtyFlag flag) { mCoreDirtyFlags &= ~(UINT32)flag; }

		/**
		 * @brief	Gets the currently active proxy of this material.
		 */
		ShaderProxyPtr _getActiveProxy() const { return mActiveProxy; }

		/**
		 * @brief	Sets an active proxy for this material.
		 */
		void _setActiveProxy(const ShaderProxyPtr& proxy) { mActiveProxy = proxy; }

		/**
		 * @brief	Creates a new core proxy from the currently set shader data. Core proxies ensure
		 *			that the core thread has all the necessary data, while avoiding the need
		 *			to manage Shader itself on the core thread.
		 *
		 * @note	Sim thread only. 
		 *			You generally need to update the core thread with a new proxy whenever core 
		 *			dirty flag is set.
		 */
		ShaderProxyPtr _createProxy();

	private:
		Shader(const String& name);

		/**
		 * @brief	Marks the core data as dirty.
		 */
		void markCoreDirty() { mCoreDirtyFlags = 0xFFFFFFFF; }

	private:
		String mName;
		QueueSortType mQueueSortType;
		UINT32 mQueuePriority;
		bool mSeparablePasses;
		Vector<TechniquePtr> mTechniques;
		UINT32 mCoreDirtyFlags;

		Map<String, SHADER_DATA_PARAM_DESC> mDataParams;
		Map<String, SHADER_OBJECT_PARAM_DESC> mObjectParams;
		Map<String, SHADER_PARAM_BLOCK_DESC> mParamBlocks;

		ShaderProxyPtr mActiveProxy;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		
	public:
		friend class ShaderRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}