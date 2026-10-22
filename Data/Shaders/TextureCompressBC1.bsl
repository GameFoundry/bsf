#include "$ENGINE$\TextureCompression\TextureCompressBC1.bslinc"
#include "$ENGINE$\TextureCompression\TextureCompressBC4.bslinc"

shader TextureCompressBC1
{
	featureset = HighEnd;

	mixin TextureCompressBC1;
	mixin TextureCompressBC4;

	variations
	{
		// 0 - BC1 (RGB, 64-bit block), which may use the 3-color mode
		// 1 - BC3 (RGBA: BC4 alpha block + BC1 color block, 128-bit), whose color block is always 4-color
		FORMAT = { 0, 1 };
	};

	code
	{
		#if FORMAT == 0
			#define BLOCK_TYPE uint2
		#else
			#define BLOCK_TYPE uint4
		#endif

		Texture2D gInput;
		RWTexture2D<BLOCK_TYPE> gOutput;

		[internal]
		cbuffer Parameters
		{
			int2 gTextureSize;
			int2 gBlockCount;
			int2 gBlockOffset;
			int2 gScratchBlockCount;
			int4 gStageParameters;
		}

		// gGroupTexels holds the texels (TextureCompressBC1.bslinc), and gSortedPrefixSums the 17 prefix sums of the
		// sorted texels, per block
		groupshared float gGroupAlphas[BLOCKS_PER_GROUP * 16u];
		groupshared float gSortKeys[64];
		groupshared float3 gSortedTexels[64];
		groupshared float3 gSortedPrefixSums[BLOCKS_PER_GROUP * TEXEL_STRIDE];

		// Each thread's best split, then its best BC3 alpha endpoint candidate
		groupshared float gSplitErrors[64];
		groupshared uint gSplitIndices[64];
		groupshared float3 gSplitEndpoints0[64];
		groupshared float3 gSplitEndpoints1[64];

		// Each group encodes 4 consecutive blocks of one row, with 16 threads per block. The block's endpoints come from a
		// cluster fit: the texels are sorted along the block's principal axis, and every way of splitting the sorted run
		// into the 4 palette levels gets a closed-form least-squares endpoint pair. The threads of a block share the 969
		// splits. Refitting along the axis of the best split doesn't pay: it lowers the unquantized error, but not the
		// error after quantization and polish, and doubles the cost. The endpoints are then quantized and polished,
		// with the threads sharing the polish candidates. BC1 also tries the 3-color mode, and BC3 encodes the alpha block,
		// with the threads sharing its endpoint window.
		[numthreads(64, 1, 1)]
		void csmain(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID)
		{
			const uint threadIndex = groupThreadId.x;
			const uint blockInGroup = threadIndex / 16u;
			const uint lane = threadIndex % 16u;
			const uint lastBlockColumn = (uint)gScratchBlockCount.x - 1u;
			const uint blockColumn = groupId.x * BLOCKS_PER_GROUP + blockInGroup;
			const bool isValidBlock = blockColumn <= lastBlockColumn;
			const uint2 blockId = uint2(min(blockColumn, lastBlockColumn), groupId.y) + (uint2)gBlockOffset;
			const uint texelBase = blockInGroup * TEXEL_STRIDE;
			const uint laneBase = blockInGroup * 16u;

			// One texel per thread. Groups extending past the end of the row repeat its last block, and never write it.
			{
				const uint2 maxCoordinate = (uint2)gTextureSize - 1u;
				const uint2 coordinate = min(blockId * 4u + uint2(lane & 3u, lane >> 2u), maxCoordinate);
				const float4 texel = gInput.Load(int3((int2)coordinate, 0));
				gGroupTexels[texelBase + lane] = texel.rgb;
				gGroupAlphas[laneBase + lane] = texel.a;
			}

			GroupMemoryBarrierWithGroupSync();

			// Mean, covariance and bounding box of the block, computed by every thread of the block
			float3 mean = float3(0, 0, 0);
			float3 minimum = float3(1, 1, 1);
			float3 maximum = float3(0, 0, 0);
			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
			{
				const float3 texel = gGroupTexels[texelBase + texelIndex];
				mean += texel;
				minimum = min(minimum, texel);
				maximum = max(maximum, texel);
			}

			mean *= 1.0f / 16.0f;

			// The cluster fit works on the texels relative to the block mean. The splits' errors subtract large sums of
			// squares from each other, which would cancel away the precision near-tied splits are ranked by. The palette
			// is an affine blend of the endpoints, so this only shifts the endpoints by the mean.
			float squaredSum = 0.0f;
			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
			{
				const float3 centeredTexel = gGroupTexels[texelBase + texelIndex] - mean;
				squaredSum += dot(centeredTexel, centeredTexel);
			}

			const float3 axis = FindBC1PrincipalAxis(texelBase, mean, minimum, maximum);

			// Fallback endpoints, the extreme projections onto the axis, if every split turns out degenerate
			float3 bestEndpoint0;
			float3 bestEndpoint1;
			{
				float2 range = float2(1e20f, -1e20f);
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float projection = dot(gGroupTexels[texelBase + texelIndex] - mean, axis);
					range = float2(min(range.x, projection), max(range.y, projection));
				}

				bestEndpoint0 = saturate(mean + axis * range.y);
				bestEndpoint1 = saturate(mean + axis * range.x);
			}

			// Stable ascending sort of the texels by their projection onto the axis, one thread per texel
			gSortKeys[threadIndex] = dot(gGroupTexels[texelBase + lane] - mean, axis);

			GroupMemoryBarrierWithGroupSync();

			{
				const float key = gSortKeys[threadIndex];
				uint rank = 0u;
				for (uint otherLane = 0u; otherLane < 16u; ++otherLane)
				{
					const float otherKey = gSortKeys[laneBase + otherLane];
					rank += (otherKey < key || (otherKey == key && otherLane < lane)) ? 1u : 0u;
				}

				gSortedTexels[laneBase + rank] = gGroupTexels[texelBase + lane] - mean;
			}

			GroupMemoryBarrierWithGroupSync();

			// Prefix sums of the sorted, centered texels: entry N is the sum of the first N
			{
				float3 prefixSum = float3(0, 0, 0);
				for (uint sortedIndex = 0u; sortedIndex <= lane; ++sortedIndex)
					prefixSum += gSortedTexels[laneBase + sortedIndex];

				gSortedPrefixSums[texelBase + lane + 1u] = prefixSum;
				if (lane == 0u)
					gSortedPrefixSums[texelBase] = float3(0, 0, 0);
			}

			GroupMemoryBarrierWithGroupSync();

			// Every split (n0, n1, n2, n3) of the sorted texels into the palette levels {0, 1/3, 2/3, 1}, numbered in order
			// of n0, n1 and n2. Thread N of the block evaluates the splits whose number modulo 16 is N.
			float splitError = 1e30f;
			uint splitIndex = 0xFFFFFFFFu;
			float3 splitEndpoint0 = float3(0, 0, 0);
			float3 splitEndpoint1 = float3(0, 0, 0);
			{
				const float3 sum = gSortedPrefixSums[texelBase + 16u];
				uint rowStart = 0u;
				for (uint count0 = 0u; count0 <= 16u; ++count0)
				{
					const float3 sum0 = gSortedPrefixSums[texelBase + count0];
					for (uint count1 = 0u; count0 + count1 <= 16u; ++count1)
					{
						const float3 prefix01 = gSortedPrefixSums[texelBase + count0 + count1];
						const float3 sum1 = prefix01 - sum0;
						const uint rowLength = 17u - count0 - count1;
						for (uint count2 = (lane + 16u - rowStart % 16u) % 16u; count2 < rowLength; count2 += 16u)
						{
							const float3 prefix012 = gSortedPrefixSums[texelBase + count0 + count1 + count2];
							const float3 sum2 = prefix012 - prefix01;
							const float3 sum3 = sum - prefix012;
							const float count3 = (float)(16u - count0 - count1 - count2);

							float3 endpoint0;
							float3 endpoint1;
							const float error = SolveBC1Split((float)count0, (float)count1, (float)count2, count3, sum0, sum1, sum2, sum3, squaredSum, endpoint0, endpoint1);
							if (error < splitError)
							{
								splitError = error;
								splitIndex = rowStart + count2;
								splitEndpoint0 = endpoint0;
								splitEndpoint1 = endpoint1;
							}
						}

						rowStart += rowLength;
					}
				}
			}

			gSplitErrors[threadIndex] = splitError;
			gSplitIndices[threadIndex] = splitIndex;
			gSplitEndpoints0[threadIndex] = splitEndpoint0;
			gSplitEndpoints1[threadIndex] = splitEndpoint1;

			GroupMemoryBarrierWithGroupSync();

			// The best split, with ties going to the first split, replaces the fallback endpoints unless every split was
			// degenerate
			{
				uint winnerThread = laneBase;
				for (uint otherLane = 1u; otherLane < 16u; ++otherLane)
				{
					const float otherError = gSplitErrors[laneBase + otherLane];
					const float winnerError = gSplitErrors[winnerThread];
					if (otherError < winnerError || (otherError == winnerError && gSplitIndices[laneBase + otherLane] < gSplitIndices[winnerThread]))
						winnerThread = laneBase + otherLane;
				}

				if (gSplitErrors[winnerThread] < 1e30f)
				{
					bestEndpoint0 = saturate(gSplitEndpoints0[winnerThread] + mean);
					bestEndpoint1 = saturate(gSplitEndpoints1[winnerThread] + mean);
				}
			}

			// The quantized blocks, which every thread of the block ends up with
			float colorError;
			uint2 colorBlock = FinishBC1FourColor(texelBase, blockInGroup, lane, bestEndpoint0, bestEndpoint1, colorError);

		#if FORMAT == 0
			// The 3-color block replaces the 4-color one only if it has a lower error
			float threeColorError;
			const uint2 threeColorBlock = FinishBC1ThreeColor(texelBase, blockInGroup, lane, bestEndpoint0, bestEndpoint1, threeColorError);
			if (threeColorError < colorError)
				colorBlock = threeColorBlock;
		#else
			// The alpha block. The threads of the block split the endpoint window search, and the lowest-error candidate,
			// with ties going to the first, wins.
			float alphas[16];
			[unroll]
			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				alphas[texelIndex] = gGroupAlphas[laneBase + texelIndex];

			uint baseCode0;
			uint baseCode1;
			RefineBC4Endpoints(alphas, baseCode0, baseCode1);

			float windowError = 1e30f;
			uint windowCandidate = 0xFFFFFFFFu;
			for (uint candidate = lane; candidate < BC4_WINDOW_CANDIDATE_COUNT; candidate += 16u)
			{
				const uint2 codes = GetBC4WindowCodes(baseCode0, baseCode1, candidate);
				const float error = GetBC4Error(alphas, codes.x, codes.y);
				if (error < windowError)
				{
					windowError = error;
					windowCandidate = candidate;
				}
			}

			gSplitErrors[threadIndex] = windowError;
			gSplitIndices[threadIndex] = windowCandidate;

			GroupMemoryBarrierWithGroupSync();

			uint winnerThread = laneBase;
			for (uint otherLane = 1u; otherLane < 16u; ++otherLane)
			{
				const float otherError = gSplitErrors[laneBase + otherLane];
				const float winnerError = gSplitErrors[winnerThread];
				if (otherError < winnerError || (otherError == winnerError && gSplitIndices[laneBase + otherLane] < gSplitIndices[winnerThread]))
					winnerThread = laneBase + otherLane;
			}

			const uint2 alphaCodes = GetBC4WindowCodes(baseCode0, baseCode1, gSplitIndices[winnerThread]);
		#endif

			if (lane != 0u || !isValidBlock)
				return;

		#if FORMAT == 0
			gOutput[blockId] = colorBlock;
		#else
			gOutput[blockId] = uint4(PackBC4(alphas, alphaCodes.x, alphaCodes.y), colorBlock);
		#endif
		}
	};
};
