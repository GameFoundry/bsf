#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmException.h"
#include "CmGpuProgram.h"

namespace BansheeEngine 
{
	class CM_EXPORT GpuProgramFactory
	{
	public:
        GpuProgramFactory() {}
        virtual ~GpuProgramFactory();
		/// Get the name of the language this factory creates programs for
		virtual const String& getLanguage(void) const = 0;
		virtual GpuProgramPtr create(const String& source, const String& entryPoint,
			GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes,
			bool requiresAdjacencyInformation) = 0;
		virtual GpuProgramPtr create(GpuProgramType type) = 0;
	};

	class CM_EXPORT GpuProgramManager : public Module<GpuProgramManager>
	{
	public:
		typedef Map<String, GpuProgramFactory*> FactoryMap;
	protected:
		/// Factories capable of creating HighLevelGpuProgram instances
		FactoryMap mFactories;

		/// Factory for dealing with programs for languages we can't create
		GpuProgramFactory* mNullFactory;

		GpuProgramFactory* getFactory(const String& language);
	public:
		GpuProgramManager();
		~GpuProgramManager();
		/** Add a new factory object for high-level programs of a given language. */
		void addFactory(GpuProgramFactory* factory);
		/** Remove a factory object for high-level programs of a given language. */
		void removeFactory(GpuProgramFactory* factory);

		/** Returns whether a given high-level language is supported. */
		bool isLanguageSupported(const String& lang);


        /** Create a new HighLevelGpuProgram. 
		@par
			This method creates a new program of the type specified as the second and third parameters.
		@param name The identifying name of the program
        @param groupName The name of the resource group which this program is
            to be a member of
		@param language Code of the language to use (e.g. "cg")
		@param gptype The type of program to create
		*/
		GpuProgramPtr create(const String& source, const String& entryPoint, const String& language, 
			GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes,
			bool requiresAdjacencyInformation = false);

		/**
		 * @brief	Creates a completely empty and uninitialized HighLevelGpuProgram.
		 * 			Should only be used for VERY specific purposes, like deserialization,
		 * 			as it requires additional manual initialization that is not required normally.
		 */
		GpuProgramPtr createEmpty(const String& language, GpuProgramType type);
	};
}