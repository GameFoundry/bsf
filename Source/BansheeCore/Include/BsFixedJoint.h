//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsJoint.h"

namespace BansheeEngine
{
	/** @addtogroup Physics
	 *  @{
	 */

	/** Physics joint that will maintain a fixed distance and orientation between its two attached bodies. */
	class BS_CORE_EXPORT FixedJoint : public Joint
	{
	public:
		virtual ~FixedJoint() { }

		/** Creates a new fixed joint. */
		static SPtr<FixedJoint> create();
	};

	/** @} */
}