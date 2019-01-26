//********************************** Banshee Engine (www.banshee4d.com) **************************************************//
//**************** Copyright (c) 2018 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "Image/BsColorGradient.h"

namespace bs
{
	/** @addtogroup ScriptInteropEngine
	*  @{
	*/
	/** @cond SCRIPT_EXTENSIONS */

	/** Extension class for ColorGradient, for adding additional functionality for the script interface. */
	class BS_SCRIPT_EXPORT(e:ColorGradient) ColorGradientEx
	{
	public:
		/** @copydoc ColorGradient::evaluate */
		BS_SCRIPT_EXPORT(e:ColorGradient)
		static Color evaluate(const SPtr<ColorGradient>& thisPtr, float t);
	}; 

	/** @endcond */
	/** @} */
}