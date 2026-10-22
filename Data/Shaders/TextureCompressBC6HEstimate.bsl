#include "$ENGINE$\TextureCompression\TextureCompressBC6Common.bslinc"

shader TextureCompressBC6HEstimate
{
	featureset = HighEnd;

	mixin TextureCompressBC6Common;

	code
	{
		Texture2D gInput;
		StructuredBuffer<uint> gBC7TwoSubsetPartitions; // subset 1 texel mask | (subset 1 anchor texel << 16); BC6H uses the first 32
		RWStructuredBuffer<uint> gBC6HCandidates;

		[internal]
		cbuffer Parameters
		{
			int2 gTextureSize;
			int2 gBlockCount;
			int2 gBlockOffset;
			int2 gScratchBlockCount;

			// x: number of candidates written per block
			int4 gStageParameters;
		}

		#define BLOCKS_PER_GROUP 2u

		groupshared float4 gGroupTexels[BLOCKS_PER_GROUP * 16u];
		groupshared float gPartitionErrors[BLOCKS_PER_GROUP * 32u];

		// Scores a partition by the squared distance of each subset's texels to the subset's best-fit line, plus the error
		// of snapping them to the 8 evenly spaced points of a 3-bit index ramp spanning the subset along the line. Texels
		// start at @p texelBase and are taken relative to @p blockMean. @p mean0, @p axis0, @p mean1 and @p axis1 are the
		// lines of subsets 0 and 1.
		float ScoreBC6Partition(uint texelBase, uint subset1Mask, float4 blockMean, float4 mean0, float4 axis0, float4 mean1, float4 axis1)
		{
			float score = 0.0f;
			float2 range0 = float2(1e30f, -1e30f);
			float2 range1 = float2(1e30f, -1e30f);
			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
			{
				const float4 texel = gGroupTexels[texelBase + texelIndex] - blockMean;
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
				const float4 texel = gGroupTexels[texelBase + texelIndex] - blockMean;
				if (((subset1Mask >> texelIndex) & 1u) != 0u)
					score += GetBC7RampDistance(dot(texel - mean1, axis1), range1, 8.0f);
				else
					score += GetBC7RampDistance(dot(texel - mean0, axis0), range0, 8.0f);
			}

			return score;
		}

		// Each group ranks the 32 two-region partitions of two consecutive blocks of one row, with one thread per (block,
		// partition). Each partition is scored by how well a line and an index ramp fit its subsets, with texels as
		// half-float bits (see ScoreBC6Partition), which ignores endpoint quantization. The best partitions are written
		// out, best first, and shared by all ten two-region modes.
		[numthreads(64, 1, 1)]
		void csmain(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID)
		{
			const uint threadIndex = groupThreadId.x;
			const uint blockInGroup = threadIndex / 32u;
			const uint partition = threadIndex % 32u;
			const uint lastBlockColumn = (uint)gScratchBlockCount.x - 1u;
			const uint blockColumn = groupId.x * BLOCKS_PER_GROUP + blockInGroup;
			const uint2 localBlockId = uint2(min(blockColumn, lastBlockColumn), groupId.y);
			const uint2 blockId = localBlockId + (uint2)gBlockOffset;
			const uint candidateCount = (uint)gStageParameters.x;

			// The second block of a group extending past the end of the row repeats the last block, and never writes it
			if (partition < 16u)
			{
				const uint2 maxCoordinate = (uint2)gTextureSize - 1u;
				const uint2 coordinate = min(blockId * 4u + uint2(partition & 3u, partition >> 2u), maxCoordinate);
				gGroupTexels[blockInGroup * 16u + partition] = GetBC6HalfBits(gInput.Load(int3((int2)coordinate, 0)));
			}

			GroupMemoryBarrierWithGroupSync();

			// Texels are taken relative to the block mean, so the raw second moments of values up to 0x7BFF don't lose the
			// precision the partitions are ranked by
			const uint texelBase = blockInGroup * 16u;
			float4 blockMean = float4(0, 0, 0, 0);
			for (uint meanTexelIndex = 0u; meanTexelIndex < 16u; ++meanTexelIndex)
				blockMean += gGroupTexels[texelBase + meanTexelIndex];

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
				const float4 texel = gGroupTexels[texelBase + texelIndex] - blockMean;
				const float subset1Weight = (float)((subset1Mask >> texelIndex) & 1u);

				blockSum += texel;
				AccumulateBC7Moments(blockMoments, texel, 1.0f);

				subset1Count += subset1Weight;
				subset1Sum += subset1Weight * texel;
				AccumulateBC7Moments(subset1Moments, texel, subset1Weight);
			}

			// Every two-region partition has texels in both subsets
			const float4 subset0Sum = blockSum - subset1Sum;
			const float subset0Count = 16.0f - subset1Count;
			const float4 axis0 = FindBC7PrincipalAxis(CenterBC7Moments(SubtractBC7Moments(blockMoments, subset1Moments), subset0Sum, subset0Count));
			const float4 axis1 = FindBC7PrincipalAxis(CenterBC7Moments(subset1Moments, subset1Sum, subset1Count));
			gPartitionErrors[threadIndex] = ScoreBC6Partition(texelBase, subset1Mask, blockMean, subset0Sum / subset0Count, axis0, subset1Sum / subset1Count, axis1);

			GroupMemoryBarrierWithGroupSync();

			if (blockColumn > lastBlockColumn)
				return;

			// Rank among the block's partitions, with ties going to the lower partition index. The own error is read back from
			// groupshared memory (see TextureCompressBC7TwoSubsetEstimate).
			const uint firstErrorIndex = blockInGroup * 32u;
			const float partitionError = gPartitionErrors[threadIndex];
			uint rank = 0u;
			for (uint otherPartition = 0u; otherPartition < 32u; ++otherPartition)
			{
				const float otherError = gPartitionErrors[firstErrorIndex + otherPartition];
				rank += (otherError < partitionError || (otherError == partitionError && otherPartition < partition)) ? 1u : 0u;
			}

			if (rank < candidateCount)
				gBC6HCandidates[(localBlockId.y * (uint)gScratchBlockCount.x + localBlockId.x) * candidateCount + rank] = partition;
		}
	};
};
