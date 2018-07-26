//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Math/BsMatrix4.h"

namespace bs
{
	struct EvaluatedAnimationData;

	/** @addtogroup Particles-Internal
	 *  @{
	 */

	/** Contains particle system state that varies from frame to frame. */
	struct ParticleSystemState
	{
		float time;
		float nrmTime;
		float length;
		float timeStep;
		UINT32 maxParticles;
		bool worldSpace;
		Matrix4 localToWorld;
		Matrix4 worldToLocal;
		const EvaluatedAnimationData* animData;
	};

	/** Module that in some way modified or effects a ParticleSystem. */
	class BS_CORE_EXPORT ParticleModule : public IReflectable
	{
	protected:
		friend class ParticleSystem;

		ParticleModule() = default;
		virtual ~ParticleModule() = default;

		/** Assigns the parent ParticleSystem of this module. */
		void setParent(ParticleSystem* parent) { mParent = parent; }

		ParticleSystem* mParent = nullptr;
	};

	/** @} */
}