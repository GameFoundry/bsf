#include "$ENGINE$\TextureCompression\TextureCompressBC7Moments.bslinc"

shader TextureCompressBC7TwoSubsetEstimate
{
	featureset = HighEnd;

	mixin TextureCompressBC7Moments;

	code
	{
		Texture2D gInput;
		StructuredBuffer<uint> gBC7TwoSubsetPartitions; // subset 1 texel mask | (subset 1 anchor texel << 16)
		RWStructuredBuffer<uint> gBC7TwoSubsetCandidates;

		[internal]
		cbuffer Parameters
		{
			int2 gTextureSize;
			int2 gBlockCount;
			int2 gBlockOffset;
			int2 gScratchBlockCount;

			// x, y, z: number of candidates of modes 1, 3 and 7. Any may be 0. w: 1 if mode 3 is encoded, so mode 7 can
			// skip opaque blocks.
			int4 gStageParameters;
		}

		groupshared float4 gBlockTexels[16];
		groupshared float gScores[3 * 64];

		// Returns how many partitions of list @p listIndex score lower than @p partition, with ties going to the lower
		// partition index
		uint RankBC7Partition(uint listIndex, uint partition)
		{
			const float score = gScores[listIndex * 64u + partition];
			uint rank = 0u;
			for (uint otherPartition = 0u; otherPartition < 64u; ++otherPartition)
			{
				const float otherScore = gScores[listIndex * 64u + otherPartition];
				rank += (otherScore < score || (otherScore == score && otherPartition < partition)) ? 1u : 0u;
			}

			return rank;
		}

		// Scores a partition by the squared distance of each subset's texels to the subset's best-fit line, plus the error
		// of snapping them to @p indexLevels evenly spaced points spanning the subset along the line. Texels are relative
		// to @p blockMean, and only the channels in @p channelMask count. @p mean0, @p axis0, @p mean1 and @p axis1 are
		// the lines of subsets 0 and 1.
		float ScoreBC7Partition(uint subset1Mask, float4 blockMean, float4 channelMask, float4 mean0, float4 axis0, float4 mean1, float4 axis1, float indexLevels)
		{
			float score = 0.0f;
			float2 range0 = float2(1e30f, -1e30f);
			float2 range1 = float2(1e30f, -1e30f);
			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
			{
				const float4 texel = (gBlockTexels[texelIndex] - blockMean) * channelMask;
				float projection;
				if (((subset1Mask >> texelIndex) & 1u) != 0u)
				{
					score += GetBC7LineDistance(texel - mean1, axis1, projection);
					range1 = float2(min(range1.x, projection), max(range1.y, projection));
				}
				else
				{
					score += GetBC7LineDistance(texel - mean0, axis0, projection);
					range0 = float2(min(range0.x, projection), max(range0.y, projection));
				}
			}

			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
			{
				const float4 texel = (gBlockTexels[texelIndex] - blockMean) * channelMask;
				if (((subset1Mask >> texelIndex) & 1u) != 0u)
					score += GetBC7RampDistance(dot(texel - mean1, axis1), range1, indexLevels);
				else
					score += GetBC7RampDistance(dot(texel - mean0, axis0), range0, indexLevels);
			}

			return score;
		}

		// One group per block and one thread per partition. Each partition is scored per mode by how well a line and an
		// index ramp fit its subsets (see ScoreBC7Partition), which ignores endpoint quantization: over RGB with 3-bit
		// indices for mode 1, over RGB with 2-bit indices for mode 3 and over RGBA with 2-bit indices for mode 7. The best
		// partitions of each mode are written out, best first: the list of mode 1, then mode 3, then mode 7.
		[numthreads(64, 1, 1)]
		void csmain(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID)
		{
			const uint2 localBlockId = groupId.xy;
			const uint2 blockId = localBlockId + (uint2)gBlockOffset;
			const uint partition = groupThreadId.x;
			const uint3 candidateCounts = (uint3)gStageParameters.xyz;

			if (partition < 16u)
			{
				const uint2 maxCoordinate = (uint2)gTextureSize - 1u;
				const uint2 coordinate = min(blockId * 4u + uint2(partition & 3u, partition >> 2u), maxCoordinate);
				gBlockTexels[partition] = round(saturate(gInput.Load(int3((int2)coordinate, 0))) * 255.0f);
			}

			GroupMemoryBarrierWithGroupSync();

			// Texels are taken relative to the block mean. The scores don't change, but the raw second moments stay on the
			// scale of the block's own spread, so centering them per subset doesn't cancel away the float precision that
			// near-tied partitions are ranked by. Without this the ranking differed between GPU vendors.
			float4 blockMean = float4(0, 0, 0, 0);
			float minimumAlpha = 255.0f;
			for (uint meanTexelIndex = 0u; meanTexelIndex < 16u; ++meanTexelIndex)
			{
				const float4 texel = gBlockTexels[meanTexelIndex];
				blockMean += texel;
				minimumAlpha = min(minimumAlpha, texel.w);
			}

			blockMean *= 1.0f / 16.0f;

			// Moments of the whole block and of subset 1. Subset 0's are their difference.
			const uint subset1Mask = gBC7TwoSubsetPartitions[partition] & 0xFFFFu;
			BC7Moments blockMoments = CreateBC7Moments();
			BC7Moments subset1Moments = CreateBC7Moments();
			float4 blockSum = float4(0, 0, 0, 0);
			float4 subset1Sum = float4(0, 0, 0, 0);
			float subset1Count = 0.0f;
			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
			{
				const float4 texel = gBlockTexels[texelIndex] - blockMean;
				const float subset1Weight = (float)((subset1Mask >> texelIndex) & 1u);

				blockSum += texel;
				AccumulateBC7Moments(blockMoments, texel, 1.0f);

				subset1Count += subset1Weight;
				subset1Sum += subset1Weight * texel;
				AccumulateBC7Moments(subset1Moments, texel, subset1Weight);
			}

			// Every two-subset partition has texels in both subsets
			const BC7Moments subset0Moments = SubtractBC7Moments(blockMoments, subset1Moments);
			const float4 subset0Sum = blockSum - subset1Sum;
			const float subset0Count = 16.0f - subset1Count;
			const float4 mean0 = subset0Sum / subset0Count;
			const float4 mean1 = subset1Sum / subset1Count;

			if (candidateCounts.x + candidateCounts.y > 0u)
			{
				const float4 rgbMask = float4(1, 1, 1, 0);
				const float4 axis0 = FindBC7PrincipalAxis(CenterBC7Moments(MaskBC7MomentsToRGB(subset0Moments), subset0Sum * rgbMask, subset0Count));
				const float4 axis1 = FindBC7PrincipalAxis(CenterBC7Moments(MaskBC7MomentsToRGB(subset1Moments), subset1Sum * rgbMask, subset1Count));

				if (candidateCounts.x > 0u)
					gScores[partition] = ScoreBC7Partition(subset1Mask, blockMean, rgbMask, mean0 * rgbMask, axis0, mean1 * rgbMask, axis1, 8.0f);

				if (candidateCounts.y > 0u)
					gScores[64u + partition] = ScoreBC7Partition(subset1Mask, blockMean, rgbMask, mean0 * rgbMask, axis0, mean1 * rgbMask, axis1, 4.0f);
			}

			if (candidateCounts.z > 0u)
			{
				const float4 axis0 = FindBC7PrincipalAxis(CenterBC7Moments(subset0Moments, subset0Sum, subset0Count));
				const float4 axis1 = FindBC7PrincipalAxis(CenterBC7Moments(subset1Moments, subset1Sum, subset1Count));
				gScores[128u + partition] = ScoreBC7Partition(subset1Mask, blockMean, float4(1, 1, 1, 1), mean0, axis0, mean1, axis1, 4.0f);
			}

			GroupMemoryBarrierWithGroupSync();

			// Ranks are computed from the scores read back from groupshared memory rather than kept from above: the
			// compiler may evaluate the expression differently for the stored copy (e.g. fused multiply-adds), and any
			// mismatch gives two partitions the same rank and leaves a candidate slot unwritten.
			uint candidateBase = (localBlockId.y * (uint)gScratchBlockCount.x + localBlockId.x) * (candidateCounts.x + candidateCounts.y + candidateCounts.z);
			for (uint listIndex = 0u; listIndex < 2u; ++listIndex)
			{
				const uint candidateCount = listIndex == 0u ? candidateCounts.x : candidateCounts.y;
				if (candidateCount > 0u)
				{
					const uint rank = RankBC7Partition(listIndex, partition);
					if (rank < candidateCount)
						gBC7TwoSubsetCandidates[candidateBase + rank] = partition;
				}

				candidateBase += candidateCount;
			}

			if (candidateCounts.z > 0u)
			{
				// Mode 7 never beats mode 3 on an opaque block: both use the same partitions and 2-bit indices, and mode 3
				// stores RGB with more precision. An empty list (all entries 64) makes the mode 7 finalize skip the block.
				const bool skipsBlock = gStageParameters.w != 0 && minimumAlpha >= 255.0f;
				const uint rank = RankBC7Partition(2u, partition);
				if (rank < candidateCounts.z)
					gBC7TwoSubsetCandidates[candidateBase + rank] = skipsBlock ? 64u : partition;
			}
		}
	};
};
