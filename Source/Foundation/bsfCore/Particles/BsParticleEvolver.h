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
		virtual void evolve(Random& random, const ParticleSystemState& state, ParticleSet& set) const = 0;

		/** 
		 * Returns true if the evolver can be evaluated analytically. This means the exact particle state can be retrieved
		 * based on just the time value. Non-analytical (numerical) evolvers require the previous state of the particle
		 * and will incrementally update the particle state.
		 */
		virtual bool isAnalytical() const = 0;
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
		void evolve(Random& random, const ParticleSystemState& state, ParticleSet& set) const override;

		/** @copydoc ParticleEvolver::evolve */
		bool isAnalytical() const override { return true; }
	private:
		PARTICLE_TEXTURE_ANIMATION_DESC mDesc;
	};

	/** Types of collision modes that ParticleCollisions evolver can operate in. */
	enum class ParticleCollisionMode
	{
		/** Particles will collide with a user-provided set of planes. */
		Plane,

		/** Particles will collide with physics colliders in the scene. */
		World,
	};

	/** Structure used for initializing a ParticleCollisions object. */
	struct PARTICLE_COLLISONS_DESC
	{
		/** Collision mode determining with which geometry the particles will interact with. */
		ParticleCollisionMode mode = ParticleCollisionMode::Plane;

		/** 
		 * Determines the elasticity (bounciness) of the particle collision. Lower values make the collision less bouncy
		 * and higher values more. 
		 */
		float restitution = 1.0f;

		/**
		 * Determines how much velocity should a particle lose after a collision, in percent of its current velocity. In
		 * range [0, 1].
		 */
		float dampening = 0.5f;

		/**
		 * Determines how much should the particle lifetime be reduced after a collision, in percent of its original
		 * lifetime. In range [0, 1].
		 */
		float lifetimeLoss = 0.0f;

		/** Radius of every individual particle used for collisions, in meters. */
		float radius = 0.01f;

		/** 
		 * Physics layers that determine which objects will particle collide with. Only relevant when using the World
		 * collision mode.
		 */
		UINT64 layer = 0xFFFFFFFFFFFFFFFF;

		/** 
		 * Determines should particle collisions be cached. Caching increases performance but will yield approximate 
		 * collision results if nearby geometry is changing/moving. Only relevant if using the World collision mode.
		 */
		bool useCache = false;

		/**
		 * When using collision caching, determines how many cache entries should be re-calculated every frame. Higher
		 * values increase precision but reduce performance. Only relevant when using the World collision mode and caching
		 * is enabled.
		 */
		UINT32 numPreciseRays = 64;

		/**
		 * Size of individual grid voxel that is used for collision caching. Each voxel is approximated by a single plane
		 * which is then used for collisions.
		 */
		float voxelSize = 0.1f;
	};

	/** Particle evolver that allows particles to collide with the world. */
	class BS_CORE_EXPORT ParticleCollisions : public ParticleEvolver
	{
	public:
		ParticleCollisions(const PARTICLE_COLLISONS_DESC& desc);

		/** @copydoc ParticleEvolver::evolve */
		void evolve(Random& random, const ParticleSystemState& state, ParticleSet& set) const override;

		/** @copydoc ParticleEvolver::evolve */
		bool isAnalytical() const override { return false; }

		/** 
		 * Determines a set of planes to use when using the Plane collision mode. Planes are expected to be in world 
		 * space. 
		 */
		void setPlanes(Vector<Plane> planes) { mCollisionPlanes = std::move(planes); }

	private:
		PARTICLE_COLLISONS_DESC mDesc;

		Vector<Plane> mCollisionPlanes;
	};

	/** @} */
}
