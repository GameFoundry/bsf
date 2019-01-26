//********************************** Banshee Engine (www.banshee4d.com) **************************************************//
//**************** Copyright (c) 2018 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "Particles/BsParticleDistribution.h"

namespace bs
{
	/** @addtogroup ScriptInteropEngine
	*  @{
	*/
	/** @cond SCRIPT_EXTENSIONS */

	/** Extension class for ColorDistribution, for adding additional functionality for the script interface. */
	class BS_SCRIPT_EXPORT(e:ColorDistribution) ColorDistributionEx
	{
	public:
		/** @copydoc ColorDistribution::evaluate(float, float) */
		BS_SCRIPT_EXPORT(e:ColorDistribution)
		static Color evaluate(const SPtr<ColorDistribution>& thisPtr, float t, float factor);

		/** @copydoc ColorDistribution::evaluate(float, const Random&) */
		BS_SCRIPT_EXPORT(e:ColorDistribution)
		static Color evaluate(const SPtr<ColorDistribution>& thisPtr, float t, Random& factor);
	}; 

	/** @endcond */
	/** @} */
}
