#pragma once

#include "BsCorePrerequisites.h"
#include "BsResource.h"

namespace BansheeEngine
{
	/**
	 * @brief Describes a single data (int, Vector2, etc.) shader parameter.
	 *
	 * @see	Shader::addParameter.
	 */
	struct SHADER_DATA_PARAM_DESC
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
	struct SHADER_OBJECT_PARAM_DESC
	{
		String name;
		Vector<String> gpuVariableNames;
		UINT32 rendererSemantic;
		GpuParamObjectType type;
	};

	/**
	 * @brief Describes a shader parameter block.
	 */
	struct SHADER_PARAM_BLOCK_DESC
	{
		String name;
		bool shared;
		UINT32 rendererSemantic;
		GpuParamBlockUsage usage;
	};

	/**
	 * @brief Structure used for initializing a shader.
	 */
	struct BS_CORE_EXPORT SHADER_DESC
	{
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
		 * @brief	sorting type to use when performing sort in the render queue. Default value is sort front to back
		 *			which causes least overdraw and is preferable. Transparent objects need to be sorted back to front.
		 *			You may also specify no sorting and the elements will be rendered in the order they were added to the
		 *			render queue.
		 */
		QueueSortType queueSortType;

		/**
		 * @brief	Priority that allows you to control in what order are your shaders rendered.
		 *			See "QueuePriority" for a list of initial values. Shaders with higher priority will be
		 *			rendered before shaders with lower priority, and additionally render queue will only sort
		 *			elements within the same priority group.
		 *
		 * @note	This is useful when you want all your opaque objects to be rendered before you start
		 *			drawing your transparent ones. Or to render your overlays after everything else. Values
		 *			provided in "QueuePriority" are just for general guidance and feel free to increase them
		 *			or decrease them for finer tuning. (e.g. "QueuePriority::Opaque + 1").
		 */
		UINT32 queuePriority;

		/**
		 * @brief	Enables or disables separable passes. When separable passes are disabled
		 *			all shader passes will be executed in a sequence one after another. If it is disabled
		 *			the renderer is free to mix and match passes from different objects to achieve best
		 *			performance. (They will still be executed in sequence, but some other object may
		 *			be rendered in-between passes)
		 *
		 * @note	Shaders with transparency generally can't be separable, while opaque can.
		 */
		bool separablePasses;

		Map<String, SHADER_DATA_PARAM_DESC> dataParams;
		Map<String, SHADER_OBJECT_PARAM_DESC> objectParams;
		Map<String, SHADER_PARAM_BLOCK_DESC> paramBlocks;
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
	class BS_CORE_EXPORT ShaderBase
	{
	public:
		virtual ~ShaderBase() { }

		/**
		 * @brief	Returns currently active queue sort type.
		 *
		 * @see		SHADER_DESC::queueSortType
		 */
		QueueSortType getQueueSortType() const { return mDesc.queueSortType; }

		/**
		 * @brief	Returns currently active queue priority.
		 *
		 * @see		SHADER_DESC::queuePriority
		 */
		UINT32 getQueuePriority() const { return mDesc.queuePriority; }

		/**
		 * @brief	Returns if separable passes are allowed.
		 *
		 * @see		SHADER_DESC::separablePasses
		 */
		bool getAllowSeparablePasses() const { return mDesc.separablePasses; }

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
		 */
		const Map<String, SHADER_DATA_PARAM_DESC>& getDataParams() const { return mDesc.dataParams; }

		/** 
		 * @brief	Returns a map of all object parameters in the shader. 
		 */
		const Map<String, SHADER_OBJECT_PARAM_DESC>& getObjectParams() const { return mDesc.objectParams; }

		/** 
		 * @brief	Returns a map of all parameter blocks.
		 */
		const Map<String, SHADER_PARAM_BLOCK_DESC>& getParamBlocks() const { return mDesc.paramBlocks; }

	protected:
		ShaderBase() { }
		ShaderBase(const String& name, const SHADER_DESC& desc);

		String mName;
		SHADER_DESC mDesc;
	};

	/**
	 * @copydoc	ShaderBase
	 *
	 * @note	Templated version of Shader used for implementing both 
	 *			sim and core thread variants.
	 */
	template<bool Core>
	class BS_CORE_EXPORT TShader : public ShaderBase
	{
	public:
		template<bool Core> struct TTechniqueType {};
		template<> struct TTechniqueType < false > { typedef Technique Type; };
		template<> struct TTechniqueType < true > { typedef TechniqueCore Type; };

		typedef typename TTechniqueType<Core>::Type TechniqueType;

		TShader() { }
		TShader(const String& name, const SHADER_DESC& desc, const Vector<SPtr<TechniqueType>>& techniques);
		virtual ~TShader();
	
		/**
		 * @brief	Returns the total number of techniques in this shader.
		 */
		UINT32 getNumTechniques() const { return (UINT32)mTechniques.size(); }

		/**
		 * @brief	Gets the best supported technique based on current render and other systems.
		 * 			Returns null if not a single technique is supported.
		 */
		SPtr<TechniqueType> getBestTechnique() const;

	protected:
		Vector<SPtr<TechniqueType>> mTechniques;
	};

	/**
	 * @copydoc	ShaderBase
	 */
	class BS_CORE_EXPORT ShaderCore : public CoreObjectCore, public TShader<true>
	{
	public:
		/**
		 * @copydoc	Shader::create
		 */
		static SPtr<ShaderCore> create(const String& name, const SHADER_DESC& desc, const Vector<SPtr<TechniqueCore>>& techniques);

	protected:
		friend class Shader;

		ShaderCore(const String& name, const SHADER_DESC& desc, const Vector<SPtr<TechniqueCore>>& techniques);
	};

	/**
	 * @copydoc	ShaderBase
	 */
	class BS_CORE_EXPORT Shader : public Resource, public TShader<false>
	{
	public:
		/**
		 * @brief	Retrieves an implementation of a shader usable only from the
		 *			core thread.
		 */
		SPtr<ShaderCore> getCore() const;

		static bool isSampler(GpuParamObjectType type);
		static bool isTexture(GpuParamObjectType type);
		static bool isBuffer(GpuParamObjectType type);

		/**
		 * @brief	Returns an empty shader object with the specified name. Caller must register
		 *			techniques with the shader before using it in a Material.
		 */
		static HShader create(const String& name, const SHADER_DESC& desc, const Vector<SPtr<Technique>>& techniques);

		/**
		 * @brief	Returns a shader object but doesn't initialize it.
		 */
		static ShaderPtr createEmpty();

	private:
		Shader(const String& name, const SHADER_DESC& desc, const Vector<SPtr<Technique>>& techniques);

		/**
		 * @copydoc	CoreObject::getCoreDependencies
		 */
		void getCoreDependencies(Vector<SPtr<CoreObject>>& dependencies);

		/**
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

	private:
		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
		Shader() { }

	public:
		friend class ShaderRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}