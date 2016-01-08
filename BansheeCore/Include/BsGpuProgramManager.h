#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsException.h"
#include "BsGpuProgram.h"

namespace BansheeEngine 
{
	/** @cond INTERNAL */
	/** @addtogroup RenderAPI
	 *  @{
	 */

	/** Factory responsible for creating GPU programs of a certain type. */
	class BS_CORE_EXPORT GpuProgramFactory
	{
	public:
        GpuProgramFactory() {}
		virtual ~GpuProgramFactory() { }

		/**	Returns GPU program language this factory is capable creating GPU programs from. */
		virtual const String& getLanguage() const = 0;

		/**
		 * Creates a new GPU program using the provided source code. If compilation fails or program is not supported
		 * GpuProgram::isCompiled() method on the returned program will return false, and you will be able to retrieve 
		 * the error message via GpuProgram::getCompileErrorMessage().
		 *
		 * @param[in]	source				Source code to compile the shader from.
		 * @param[in]	entryPoint			Name of the entry point function, e.g. "main".
		 * @param[in]	gptype				Type of the program, e.g. vertex or fragment.
		 * @param[in]	profile				Program profile specifying supported feature-set. Must match the type.
		 * @param[in]	requiresAdjacency	If true then adjacency information will be provided when rendering using this 
		 *									program.
		 */
		virtual SPtr<GpuProgramCore> create(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, bool requiresAdjacencyInformation) = 0;

		/** @copydoc GpuProgramManager::createEmpty */
		virtual SPtr<GpuProgramCore> create(GpuProgramType type) = 0;
	};

	/**
	 * Manager responsible for creating GPU programs. It will automatically try to find the appropriate handler for a 
	 * specific GPU program language and create the program if possible.
	 *
	 * @note	Sim thread only.
	 */
	class BS_CORE_EXPORT GpuProgramManager : public Module<GpuProgramManager>
	{
	public:
		/**
		 * Creates a new GPU program using the provided source code. If compilation fails or program is not supported
		 * GpuProgram::isCompiled() method on the returned program will return false, and you will be able to retrieve the 
		 * error message via GpuProgram::getCompileErrorMessage().
		 *
		 * @param[in]	source				Source code to compile the shader from.
		 * @param[in]	entryPoint			Name of the entry point function, e.g. "main".
		 * @param[in]	language			Language the source is written in, e.g. "hlsl" or "glsl".
		 * @param[in]	gptype				Type of the program, e.g. vertex or fragment.
		 * @param[in]	profile				Program profile specifying supported feature-set. Must match the type.
		 * @param[in]	requiresAdjacency	If true then adjacency information will be provided when rendering using this 
		 *									program.
		 */
		GpuProgramPtr create(const String& source, const String& entryPoint, const String& language, 
			GpuProgramType gptype, GpuProgramProfile profile, bool requiresAdjacency = false);

		/**
		 * Creates a completely empty and uninitialized GpuProgram. Should only be used for specific purposes, like 
		 * deserialization, as it requires additional manual initialization that is not required normally.
		 */
		GpuProgramPtr createEmpty(const String& language, GpuProgramType type);
	};

	/**
	 * Manager responsible for creating GPU programs. It will automatically	try to find the appropriate handler for a 
	 * specific GPU program language and create the program if possible.
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT GpuProgramCoreManager : public Module<GpuProgramCoreManager>
	{
	public:
		GpuProgramCoreManager();
		virtual ~GpuProgramCoreManager();

		/**
		 * Registers a new factory that is able to create GPU programs for a certain language. If any other factory for the
		 * same language exists, it will overwrite it.
		 */
		void addFactory(GpuProgramFactory* factory);

		/**
		 * Unregisters a GPU program factory, essentially making it not possible to create GPU programs using the language 
		 * the factory supported.
		 */
		void removeFactory(GpuProgramFactory* factory);

		/** Query if a GPU program language is supported. (.e.g. "hlsl", "glsl"). */
		bool isLanguageSupported(const String& lang);

		/**
		 * Creates a new GPU program using the provided source code. If compilation fails or program is not supported
		 * GpuProgramCore::isCompiled() method on the returned program will return false, and you will be able to retrieve 
		 * the error message via GpuProgramCore::getCompileErrorMessage().
		 *
		 * @param[in]	source				Source code to compile the shader from.
		 * @param[in]	entryPoint			Name of the entry point function, e.g. "main".
		 * @param[in]	language			Language the source is written in, e.g. "hlsl" or "glsl".
		 * @param[in]	gptype				Type of the program, e.g. vertex or fragment.
		 * @param[in]	profile				Program profile specifying supported feature-set. Must match the type.
		 * @param[in]	requiresAdjacency	If true then adjacency information will be provided when rendering using this 
		 *									program.
		 */
		SPtr<GpuProgramCore> create(const String& source, const String& entryPoint, const String& language, 
			GpuProgramType gptype, GpuProgramProfile profile, bool requiresAdjacency = false);

	protected:
		friend class GpuProgram;

		/**
		 * Creates a GPU program without initializing it.
		 *
		 * @see		create
		 */
		SPtr<GpuProgramCore> createInternal(const String& source, const String& entryPoint, const String& language,
			GpuProgramType gptype, GpuProgramProfile profile, bool requiresAdjacency = false);

		/** Attempts to find a factory for the specified language. Returns null if it cannot find one. */
		GpuProgramFactory* getFactory(const String& language);

	protected:
		typedef Map<String, GpuProgramFactory*> FactoryMap;

		FactoryMap mFactories;
		GpuProgramFactory* mNullFactory; /**< Factory for dealing with GPU programs that can't be created. */
	};

	/** @} */
	/** @endcond */
}