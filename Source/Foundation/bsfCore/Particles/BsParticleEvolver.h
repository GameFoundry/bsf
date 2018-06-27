//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Particles/BsParticleModule.h"
#include "BsParticleDistribution.h"

namespace bs
{
	class Random;
	class ParticleSet;

	/** @addtogroup Particles
	 *  @{
	 */

	/** Updates properties of all active particles in a particle system in some way. */
	class BS_CORE_EXPORT ParticleEvolver : public ParticleModule
	{
	public:
		ParticleEvolver() = default;
		virtual ~ParticleEvolver() = default;

		/** Updates properties of all particles in the @p set according to the ruleset of the evolver. */
		virtual void evolve(float t, Random& random, ParticleSet& set) const = 0;
	};

	/** Structure used for initializing a ParticleTextureAnimation object. */
	struct PARTICLE_TEXTURE_ANIMATION_DESC
	{
		/**
		 * Randomly pick a row to use for animation when the particle is first spawned. This implies that only a single row
		 * of the grid will be used for individual particle's animation.
		 */
		bool randomizeRow = false;

		/** Number of cycles to loop the animation during particle's lifetime. */
		UINT32 numCycles = 1;
	};

	/** 
	 * Provides functionality for particle texture animation. Uses the sprite texture assigned to the particle's material
	 * to determine animation properties.
	 */
	class BS_CORE_EXPORT ParticleTextureAnimation : public ParticleEvolver
	{
	public:
		ParticleTextureAnimation(const PARTICLE_TEXTURE_ANIMATION_DESC& desc);

		/** @copydoc ParticleEvolver::evolve */
		void evolve(float t, Random& random, ParticleSet& set) const override;
	private:
		PARTICLE_TEXTURE_ANIMATION_DESC mDesc;
	};

	/** @} */
}
