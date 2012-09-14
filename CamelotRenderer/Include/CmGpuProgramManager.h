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
#ifndef __GpuProgramManager_H_
#define __GpuProgramManager_H_

// Precompiler options
#include "CmPrerequisites.h"
#include "CmException.h"
#include "CmGpuProgram.h"
#include "CmModule.h"

namespace CamelotEngine {

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Resources
	*  @{
	*/
	class CM_EXPORT GpuProgramManager : public Module<GpuProgramManager>
	{
	public:

		typedef set<String>::type SyntaxCodes;
		typedef map<String, GpuSharedParametersPtr>::type SharedParametersMap;


	protected:

		SharedParametersMap mSharedParametersMap;

		/** General create method
        */
        virtual GpuProgram* create(GpuProgramType gptype, const String& syntaxCode) = 0;

	public:
		GpuProgramManager();
		virtual ~GpuProgramManager();

		/** Loads a GPU program from a string of assembly code.
		@remarks
			The assembly code must be compatible with this manager - call the 
			getSupportedSyntax method for details of the supported syntaxes 
		@param name The identifying name to give this program, which can be used to
			retrieve this program later with getByName.
		@param groupName The name of the resource group
		@param code A string of assembly code which will form the program to run
		@param gptype The type of program to create.
        @param syntaxCode The name of the syntax to be used for this program e.g. arbvp1, vs_1_1
		*/
		virtual GpuProgramPtr load(const String& code, GpuProgramType gptype,
            const String& syntaxCode);

		/** Returns the syntaxes that this manager supports. */
		virtual const SyntaxCodes& getSupportedSyntax(void) const;
		 

        /** Returns whether a given syntax code (e.g. "ps_1_3", "fp20", "arbvp1") is supported. */
        virtual bool isSyntaxSupported(const String& syntaxCode) const;
		
		/** Creates a new GpuProgramParameters instance which can be used to bind
            parameters to your programs.
        @remarks
            Program parameters can be shared between multiple programs if you wish.
        */
        virtual GpuProgramParametersSharedPtr createParameters(void);
        
		/** Create a GPU program from a string of assembly code.
        @remarks    
            Use this method in preference to the 'load' methods if you wish to define
            a GpuProgram, but not load it yet; useful for saving memory.
		@par
			The assembly code must be compatible with this manager - call the 
			getSupportedSyntax method for details of the supported syntaxes 
		@param name The identifying name to give this program, which can be used to
			retrieve this program later with getByName.
		@param groupName The name of the resource group
		@param code A string of assembly code which will form the program to run
		@param gptype The type of program to create.
        @param syntaxCode The name of the syntax to be used for this program e.g. arbvp1, vs_1_1
		*/
		virtual GpuProgramPtr createProgram(const String& code, 
            GpuProgramType gptype, const String& syntaxCode);

		/** Create a new set of shared parameters, which can be used across many 
			GpuProgramParameters objects of different structures.
		@param name The name to give the shared parameters so you can refer to them
			later.
		*/
		virtual GpuSharedParametersPtr createSharedParameters(const String& name);

		/** Retrieve a set of shared parameters, which can be used across many 
		GpuProgramParameters objects of different structures.
		*/
		virtual GpuSharedParametersPtr getSharedParameters(const String& name) const;

		/** Get (const) access to the available shared parameter sets. 
		*/
		virtual const SharedParametersMap& getAvailableSharedParameters() const;
	};

	/** @} */
	/** @} */
}

#endif
