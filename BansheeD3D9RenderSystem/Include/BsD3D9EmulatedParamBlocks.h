//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsD3D9Prerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a parameter block that can be used with GPU programs
	 *			that do not support them natively. Each block is defined with a unique
	 *			name (user defined) and a set of parameter names belonging to that block
	 *			(names must reference actual GPU variables).
	 */
	struct D3D9EmulatedParamBlock
	{
		String blockName;
		Vector<String> paramNames;
	};

	/**
	 * @brief	Parses GPU program source and retrieves 
	 *			parameter blocks from it.
	 */
	class D3D9EmulatedParamBlockParser
	{
	public:
		/**
		 * Parses GPU program source and retrieves parameter blocks from it. Returns
		 * source without parameter block code.
		 *
		 * Parameter blocks can be anywhere in the source file and they must follow this exact structure:
		 *  BS_PARAM_BLOCK blockName { param1, param2, etc }
		 */
		static String parse(const String& gpuProgSource, Vector<D3D9EmulatedParamBlock>& paramBlocks);
	};
}