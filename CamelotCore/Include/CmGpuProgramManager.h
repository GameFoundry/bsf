#pragma once

#include "CmPrerequisites.h"
#include "CmException.h"
#include "CmGpuProgram.h"
#include "CmModule.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Handles creation of GPU programs for a specific language. Program source
	 *			will be parses and a usable GPU program which you may bind to the pipeline
	 *			will be returned.
	 */
	class CM_EXPORT GpuProgramManager : public Module<GpuProgramManager>
	{
	public:
		GpuProgramManager();
		virtual ~GpuProgramManager();
	
		/**
		 * @brief	Converts a generic GpuProgramProfile identifier into a render-system specific one.
		 *			Obviously this depends on the currently set render system.
		 *			Returns an empty string if conversion can't be done.
		 */
		virtual String gpuProgProfileToRSSpecificProfile(GpuProgramProfile gpuProgProfile) const;
        
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
		GpuProgramPtr createProgram(const String& source, const String& entryPoint, GpuProgramType gptype, GpuProgramProfile profile);

	protected:
		/**
		 * @brief	Internal create method that you should override in GpuProgramManager implementation for a specific language.
		 */
		virtual GpuProgramPtr create(const String& source, const String& entryPoint, GpuProgramType gptype, GpuProgramProfile profile) = 0;
	};
}