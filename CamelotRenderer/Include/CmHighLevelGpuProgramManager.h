/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __HighLevelGpuProgramManager_H__
#define __HighLevelGpuProgramManager_H__

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmException.h"
#include "CmHighLevelGpuProgram.h"

namespace CamelotEngine {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Resources
	*  @{
	*/
	/** Interface definition for factories of HighLevelGpuProgram. */
	class CM_EXPORT HighLevelGpuProgramFactory
	{
	public:
        HighLevelGpuProgramFactory() {}
        virtual ~HighLevelGpuProgramFactory();
		/// Get the name of the language this factory creates programs for
		virtual const String& getLanguage(void) const = 0;
        virtual HighLevelGpuProgram* create(const String& source, const String& entryPoint, GpuProgramProfile profile) = 0;
		virtual HighLevelGpuProgram* create() = 0;

		void destroy(HighLevelGpuProgram* prog);
		virtual void destroy_internal(HighLevelGpuProgram* prog) = 0;
	};
	/** This ResourceManager manages high-level vertex and fragment programs. 
	@remarks
		High-level vertex and fragment programs can be used instead of assembler programs
		as managed by GpuProgramManager; however they typically result in a GpuProgram
		being created as a derivative of the high-level program. High-level programs are
		easier to write, and can often be API-independent, unlike assembler programs. 
	@par
		This class not only manages the programs themselves, it also manages the factory
		classes which allow the creation of high-level programs using a variety of high-level
		syntaxes. Plugins can be created which register themselves as high-level program
		factories and as such the engine can be extended to accept virtually any kind of
		program provided a plugin is written.
	*/
	class CM_EXPORT HighLevelGpuProgramManager : public Module<HighLevelGpuProgramManager>
	{
	public:
		typedef map<String, HighLevelGpuProgramFactory*>::type FactoryMap;
	protected:
		/// Factories capable of creating HighLevelGpuProgram instances
		FactoryMap mFactories;

		/// Factory for dealing with programs for languages we can't create
		HighLevelGpuProgramFactory* mNullFactory;

		HighLevelGpuProgramFactory* getFactory(const String& language);
	public:
		HighLevelGpuProgramManager();
		~HighLevelGpuProgramManager();
		/** Add a new factory object for high-level programs of a given language. */
		void addFactory(HighLevelGpuProgramFactory* factory);
		/** Remove a factory object for high-level programs of a given language. */
		void removeFactory(HighLevelGpuProgramFactory* factory);

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
		HighLevelGpuProgramPtr create(const String& source, const String& entryPoint, const String& language, GpuProgramType gptype, GpuProgramProfile profile);

		/** Create a new HighLevelGpuProgram. 
		@par
			This method creates a new program of the specified language. You need to set other 
			properties like source, entry point, type, profile manually.
		@param language Code of the language to use (e.g. "cg")
		*/
		HighLevelGpuProgramPtr create(const String& language);

		/**
		 * @brief	Creates a completely empty and uninitialized HighLevelGpuProgram.
		 * 			Should only be used for VERY specific purposes, like deserialization,
		 * 			as it requires additional manual initialization that is not required normally.
		 */
		HighLevelGpuProgramPtr createEmpty(const String& language);
	};
	/** @} */
	/** @} */

}

#endif
