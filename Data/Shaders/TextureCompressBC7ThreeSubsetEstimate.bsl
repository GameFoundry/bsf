#include "$ENGINE$\TextureCompression\TextureCompressBC7Moments.bslinc"

shader TextureCompressBC7ThreeSubsetEstimate
{
	featureset = HighEnd;

	mixin TextureCompressBC7Moments;

	code
	{
		Texture2D gInput;

		// Two entries per partition: subset 1 texel mask | (subset 2 texel mask << 16), then subset 1 anchor texel |
		// (subset 2 anchor texel << 8)
		StructuredBuffer<uint> gBC7ThreeSubsetPartitions;
		RWStructuredBuffer<uint> gBC7ThreeSubsetCandidates;

		[internal]
		cbuffer Parameters
		{
			int2 gTextureSize;
			int2 gBlockCount;
			int2 gBlockOffset;
			int2 gScratchBlockCount;

			// x: number of mode 0 candidates (out of the first 16 partitions), y: number of mode 2 candidates (out of all
			// 64). Either may be 0.
			int4 gStageParameters;
		}

		groupshared float4 gBlockTexels[16];
		groupshared float gScores[2 * 64];

		// Returns how many of the first @p partitionCount partitions of list @p listIndex score lower than @p partition,
		// with ties going to the lower partition index
		uint RankBC7Partition(uint listIndex, uint partition, uint partitionCount)
		{
			const float score = gScores[listIndex * 64u + partition];
			uint rank = 0u;
			for (uint otherPartition = 0u; otherPartition < partitionCount; ++otherPartition)
			{
				const float otherScore = gScores[listIndex * 64u + otherPartition];
				rank += (otherScore < score || (otherScore == score && otherPartition < partition)) ? 1u : 0u;
			}

			return rank;
		}

		// Scores a partition by the squared distance of each subset's texels to the subset's best-fit line, plus the error
		// of snapping them to @p indexLevels evenly spaced points spanning the subset along the line. Texels are relative
		// to @p blockMean. @p means and @p axes hold the lines of subsets 0, 1 and 2.
		float ScoreBC7Partition(uint subsetMasks, float4 blockMean, float4 mean0, float4 axis0, float4 mean1, float4 axis1, float4 mean2, float4 axis2, float indexLevels)
		{
			float score = 0.0f;
			float2 range0 = float2(1e30f, -1e30f);
			float2 range1 = float2(1e30f, -1e30f);
			float2 range2 = float2(1e30f, -1e30f);
			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
			{
				const float4 texel = gBlockTexels[texelIndex] - blockMean;
				float projection;
				if (((subsetMasks >> (16u + texelIndex)) & 1u) != 0u)
				{
					score += GetBC7LineDistance(texel - mean2, axis2, projection);
					range2 = float2(min(range2.x, projection), max(range2.y, projection));
				}
				else if (((subsetMasks >> texelIndex) & 1u) != 0u)
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
				const float4 texel = gBlockTexels[texelIndex] - blockMean;
				if (((subsetMasks >> (16u + texelIndex)) & 1u) != 0u)
					score += GetBC7RampDistance(dot(texel - mean2, axis2), range2, indexLevels);
				else if (((subsetMasks >> texelIndex) & 1u) != 0u)
					score += GetBC7RampDistance(dot(texel - mean1, axis1), range1, indexLevels);
				else
					score += GetBC7RampDistance(dot(texel - mean0, axis0), range0, indexLevels);
			}

			return score;
		}

		// One group per block and one thread per partition. Each partition is scored per mode by how well a line and an
		// index ramp fit its subsets over RGB (see ScoreBC7Partition), which ignores endpoint quantization: with 3-bit
		// indices for mode 0 and 2-bit indices for mode 2. Mode 0 can only use the first 16 partitions and mode 2 all 64.
		// The best partitions of each mode are written out, best first: the mode 0 list, then the mode 2 list.
		[numthreads(64, 1, 1)]
		void csmain(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID)
		{
			const uint2 localBlockId = groupId.xy;
			const uint2 blockId = localBlockId + (uint2)gBlockOffset;
			const uint partition = groupThreadId.x;
			const uint mode0CandidateCount = (uint)gStageParameters.x;
			const uint mode2CandidateCount = (uint)gStageParameters.y;

			if (partition < 16u)
			{
				const uint2 maxCoordinate = (uint2)gTextureSize - 1u;
				const uint2 coordinate = min(blockId * 4u + uint2(partition & 3u, partition >> 2u), maxCoordinate);
				gBlockTexels[partition] = float4(round(saturate(gInput.Load(int3((int2)coordinate, 0)).rgb) * 255.0f), 0.0f);
			}

			GroupMemoryBarrierWithGroupSync();

			// Texels are taken relative to the block mean, which keeps the ranking precision independent of the GPU vendor
			// (see TextureCompressBC7TwoSubsetEstimate)
			float4 blockMean = float4(0, 0, 0, 0);
			for (uint meanTexelIndex = 0u; meanTexelIndex < 16u; ++meanTexelIndex)
				blockMean += gBlockTexels[meanTexelIndex];

			blockMean *= 1.0f / 16.0f;

			// Moments of the whole block and of subsets 1 and 2. Subset 0's are the remainder.
			const uint subsetMasks = gBC7ThreeSubsetPartitions[partition * 2u];
			BC7Moments blockMoments = CreateBC7Moments();
			BC7Moments subset1Moments = CreateBC7Moments();
			BC7Moments subset2Moments = CreateBC7Moments();
			float4 blockSum = float4(0, 0, 0, 0);
			float4 subset1Sum = float4(0, 0, 0, 0);
			float4 subset2Sum = float4(0, 0, 0, 0);
			float subset1Count = 0.0f;
			float subset2Count = 0.0f;
			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
			{
				const float4 texel = gBlockTexels[texelIndex] - blockMean;
				const float subset1Weight = (float)((subsetMasks >> texelIndex) & 1u);
				const float subset2Weight = (float)((subsetMasks >> (16u + texelIndex)) & 1u);

				blockSum += texel;
				AccumulateBC7Moments(blockMoments, texel, 1.0f);

				subset1Count += subset1Weight;
				subset1Sum += subset1Weight * texel;
				AccumulateBC7Moments(subset1Moments, texel, subset1Weight);

				subset2Count += subset2Weight;
				subset2Sum += subset2Weight * texel;
				AccumulateBC7Moments(subset2Moments, texel, subset2Weight);
			}

			// Every three-subset partition has texels in all subsets
			const BC7Moments subset0Moments = SubtractBC7Moments(SubtractBC7Moments(blockMoments, subset1Moments), subset2Moments);
			const float4 subset0Sum = blockSum - subset1Sum - subset2Sum;
			const float subset0Count = 16.0f - subset1Count - subset2Count;
			const float4 mean0 = subset0Sum / subset0Count;
			const float4 mean1 = subset1Sum / subset1Count;
			const float4 mean2 = subset2Sum / subset2Count;
			const float4 axis0 = FindBC7PrincipalAxis(CenterBC7Moments(subset0Moments, subset0Sum, subset0Count));
			const float4 axis1 = FindBC7PrincipalAxis(CenterBC7Moments(subset1Moments, subset1Sum, subset1Count));
			const float4 axis2 = FindBC7PrincipalAxis(CenterBC7Moments(subset2Moments, subset2Sum, subset2Count));

			if (mode0CandidateCount > 0u && partition < 16u)
				gScores[partition] = ScoreBC7Partition(subsetMasks, blockMean, mean0, axis0, mean1, axis1, mean2, axis2, 8.0f);

			if (mode2CandidateCount > 0u)
				gScores[64u + partition] = ScoreBC7Partition(subsetMasks, blockMean, mean0, axis0, mean1, axis1, mean2, axis2, 4.0f);

			GroupMemoryBarrierWithGroupSync();

			// Ranks are computed from the scores read back from groupshared memory (see TextureCompressBC7TwoSubsetEstimate)
			const uint candidateBase = (localBlockId.y * (uint)gScratchBlockCount.x + localBlockId.x) * (mode0CandidateCount + mode2CandidateCount);
			if (mode0CandidateCount > 0u && partition < 16u)
			{
				const uint rank = RankBC7Partition(0u, partition, 16u);
				if (rank < mode0CandidateCount)
					gBC7ThreeSubsetCandidates[candidateBase + rank] = partition;
			}

			if (mode2CandidateCount > 0u)
			{
				const uint rank = RankBC7Partition(1u, partition, 64u);
				if (rank < mode2CandidateCount)
					gBC7ThreeSubsetCandidates[candidateBase + mode0CandidateCount + rank] = partition;
			}
		}
	};
};
