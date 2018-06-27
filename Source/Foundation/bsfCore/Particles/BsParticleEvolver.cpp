//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Particles/BsParticleEvolver.h"
#include "Private/Particles/BsParticleSet.h"
#include "Image/BsSpriteTexture.h"
#include "BsParticleSystem.h"
#include "Material/BsMaterial.h"

namespace bs
{
	// Arbitrary random numbers to add variation to different random particle properties, since we use just a single
	// seed value per particle
	static constexpr UINT32 PARTICLE_ROW_VARIATION = 0x1e8b2f4a;

	ParticleTextureAnimation::ParticleTextureAnimation(const PARTICLE_TEXTURE_ANIMATION_DESC& desc)
		:mDesc(desc)
	{ }

	void ParticleTextureAnimation::evolve(float t, Random& random, ParticleSet& set) const
	{
		const UINT32 count = set.getParticleCount();
		ParticleSetData& particles = set.getParticles();

		SpriteTexture* texture = nullptr;
		if(mParent)
		{
			const HMaterial& material = mParent->getMaterial();
			if(material.isLoaded(false))
			{
				const HSpriteTexture& spriteTex = material->getSpriteTexture("gTexture");
				if(spriteTex.isLoaded(true))
					texture = spriteTex.get();
			}
		}

		bool hasValidAnimation = texture != nullptr;
		if(hasValidAnimation)
		{
			const SpriteSheetGridAnimation& gridAnim = texture->getAnimation();
			hasValidAnimation = gridAnim.numRows > 0 && gridAnim.numColumns > 0 && gridAnim.count > 0;
		}

		if(!hasValidAnimation)
		{
			for (UINT32 i = 0; i < count; i++)
				particles.frame[i] = 0.0f;

			return;
		}
		
		const SpriteSheetGridAnimation& gridAnim = texture->getAnimation();

		for (UINT32 i = 0; i < count; i++)
		{
			UINT32 frameOffset;
			UINT32 numFrames;
			if (mDesc.randomizeRow)
			{
				const UINT32 rowSeed = particles.seed[i] + PARTICLE_ROW_VARIATION;
				const UINT32 row = Random(rowSeed).getRange(0, gridAnim.numRows);

				frameOffset = row * gridAnim.numColumns;
				numFrames = gridAnim.numColumns;
			}
			else
			{
				frameOffset = 0;
				numFrames = gridAnim.count;
			}

			float particleT = (particles.initialLifetime[i] - particles.lifetime[i]) / particles.initialLifetime[i];
			particleT = Math::repeat(mDesc.numCycles * particleT, 1.0f);

			const float frame = particleT * numFrames;
			particles.frame[i] = frameOffset + Math::clamp(frame, 0.0f, (float)(numFrames - 1));
		}
	}
}
