#include "$ENGINE$\TextureCompression\TextureCompressBC7SubsetFit.bslinc"

shader TextureCompressBC7ThreeSubsetFinalize
{
	featureset = HighEnd;

	mixin TextureCompressBC7SubsetFit;

	variations
	{
		BC7_MODE = { 0, 2 };
		CANDIDATE_COUNT = { 2, 4, 8, 16, 32, 64 };
	};

	code
	{
		// The three-subset modes are RGB only and differ in their endpoint and index precision (block layouts LSB->MSB,
		// endpoint codes ordered subset 0 endpoint 0, subset 0 endpoint 1, ..., subset 2 endpoint 1 within each channel;
		// the anchor texels store their index with one bit less):
		//   mode 0: "1"   | partition(4) | RGB codes (4 bits) | 1 P-bit per endpoint | 3-bit indices
		//   mode 2: "001" | partition(6) | RGB codes (5 bits) | no P-bits            | 2-bit indices
		// Both decode alpha as 255, so their error includes the block's distance from opaque.
		#if BC7_MODE == 0
			#define MODE_PREFIX 0x1u
			#define MODE_PREFIX_BITS 1u
			#define PARTITION_BITS 4u
			#define ENDPOINT_BITS 4u
			#define PBIT_MODE BC7_PBIT_PER_ENDPOINT
			#define INDEX_BITS 3u
		#else
			#define MODE_PREFIX 0x4u
			#define MODE_PREFIX_BITS 3u
			#define PARTITION_BITS 6u
			#define ENDPOINT_BITS 5u
			#define PBIT_MODE BC7_PBIT_NONE
			#define INDEX_BITS 2u
		#endif

		#define CHANNEL_MASK float4(1, 1, 1, 0)

		Texture2D gInput;
		RWTexture2D<uint4> gOutput;
		RWTexture2D<float> gBestErr;

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

			// x: number of candidates the estimate wrote per block, over all lists. y: first candidate of this mode's list.
			int4 gStageParameters;
		}

		#define BLOCKS_PER_GROUP (64 / CANDIDATE_COUNT)

		// Texels per block in groupshared memory. One more than a block has, so that threads reading the same texel of
		// different blocks hit different banks.
		#define TEXEL_STRIDE 17

		groupshared float4 gGroupTexels[BLOCKS_PER_GROUP * TEXEL_STRIDE];
		groupshared float gCandidateErrors[64];

		// Picks the palette of the subset a texel belongs to
		float4 SelectBC7Subset(float subset1Weight, float subset2Weight, float4 subset0Value, float4 subset1Value, float4 subset2Value)
		{
			return (subset2Weight != 0.0f) ? subset2Value : ((subset1Weight != 0.0f) ? subset1Value : subset0Value);
		}

		// Each group encodes BLOCKS_PER_GROUP consecutive blocks of one row, with one thread per (block, candidate
		// partition). Every thread fits, quantizes and refines its partition. The lowest-error candidate of each block is
		// packed and replaces the block's current encoding if it has a lower error.
		[numthreads(64, 1, 1)]
		void csmain(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID)
		{
			const uint threadIndex = groupThreadId.x;
			const uint blockInGroup = threadIndex / CANDIDATE_COUNT;
			const uint candidateIndex = threadIndex % CANDIDATE_COUNT;
			const uint firstBlockColumn = groupId.x * BLOCKS_PER_GROUP;
			const uint lastBlockColumn = (uint)gScratchBlockCount.x - 1u;
			const uint2 maxCoordinate = (uint2)gTextureSize - 1u;

			// Groups extending past the end of the row repeat its last block, and never write it. Alpha is only kept as the
			// squared distance of each texel from opaque, which the decoder produces.
			for (uint loadIndex = threadIndex; loadIndex < BLOCKS_PER_GROUP * 16u; loadIndex += 64u)
			{
				const uint2 loadBlockId = uint2(min(firstBlockColumn + loadIndex / 16u, lastBlockColumn), groupId.y) + (uint2)gBlockOffset;
				const uint texelIndex = loadIndex % 16u;
				const uint2 coordinate = min(loadBlockId * 4u + uint2(texelIndex & 3u, texelIndex >> 2u), maxCoordinate);
				const float4 texel = round(saturate(gInput.Load(int3((int2)coordinate, 0))) * 255.0f);
				const float alphaDelta = texel.w - 255.0f;
				gGroupTexels[(loadIndex / 16u) * TEXEL_STRIDE + texelIndex] = float4(texel.rgb, alphaDelta * alphaDelta);
			}

			GroupMemoryBarrierWithGroupSync();

			const bool isValidBlock = firstBlockColumn + blockInGroup <= lastBlockColumn;
			const uint2 localBlockId = uint2(min(firstBlockColumn + blockInGroup, lastBlockColumn), groupId.y);
			const uint2 blockId = localBlockId + (uint2)gBlockOffset;
			const uint candidateBase = (localBlockId.y * (uint)gScratchBlockCount.x + localBlockId.x) * (uint)gStageParameters.x + (uint)gStageParameters.y;
			const uint partition = gBC7ThreeSubsetCandidates[candidateBase + candidateIndex];
			const uint subsetMasks = gBC7ThreeSubsetPartitions[partition * 2u];
			const uint anchors = gBC7ThreeSubsetPartitions[partition * 2u + 1u];
			const uint subset1AnchorTexelIndex = anchors & 0xFFu;
			const uint subset2AnchorTexelIndex = anchors >> 8u;
			const uint texelBase = blockInGroup * TEXEL_STRIDE;

			// Initial endpoints: each subset's principal axis, spanning the extreme projections of the subset's texels.
			// Endpoint 0 is the high end of the axis.
			float4 subset0Endpoint0;
			float4 subset0Endpoint1;
			float4 subset1Endpoint0;
			float4 subset1Endpoint1;
			float4 subset2Endpoint0;
			float4 subset2Endpoint1;
			{
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
					const float4 texel = gGroupTexels[texelBase + texelIndex] * CHANNEL_MASK;
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

				const float subset0Count = 16.0f - subset1Count - subset2Count;
				const float4 subset0Sum = blockSum - subset1Sum - subset2Sum;
				const float4 subset0Mean = subset0Sum / max(subset0Count, 1.0f);
				const float4 subset1Mean = subset1Sum / max(subset1Count, 1.0f);
				const float4 subset2Mean = subset2Sum / max(subset2Count, 1.0f);
				const BC7Moments subset0Moments = SubtractBC7Moments(SubtractBC7Moments(blockMoments, subset1Moments), subset2Moments);
				const float4 subset0Axis = FindBC7PrincipalAxis(CenterBC7Moments(subset0Moments, subset0Sum, subset0Count));
				const float4 subset1Axis = FindBC7PrincipalAxis(CenterBC7Moments(subset1Moments, subset1Sum, subset1Count));
				const float4 subset2Axis = FindBC7PrincipalAxis(CenterBC7Moments(subset2Moments, subset2Sum, subset2Count));

				// Minimum and maximum projection onto the axis
				float2 subset0Range = float2(1e30f, -1e30f);
				float2 subset1Range = float2(1e30f, -1e30f);
				float2 subset2Range = float2(1e30f, -1e30f);
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float4 texel = gGroupTexels[texelBase + texelIndex] * CHANNEL_MASK;
					if (((subsetMasks >> (16u + texelIndex)) & 1u) != 0u)
					{
						const float projection = dot(texel - subset2Mean, subset2Axis);
						subset2Range = float2(min(subset2Range.x, projection), max(subset2Range.y, projection));
					}
					else if (((subsetMasks >> texelIndex) & 1u) != 0u)
					{
						const float projection = dot(texel - subset1Mean, subset1Axis);
						subset1Range = float2(min(subset1Range.x, projection), max(subset1Range.y, projection));
					}
					else
					{
						const float projection = dot(texel - subset0Mean, subset0Axis);
						subset0Range = float2(min(subset0Range.x, projection), max(subset0Range.y, projection));
					}
				}

				subset0Endpoint0 = clamp(subset0Mean + subset0Axis * subset0Range.y, 0.0f, 255.0f) * CHANNEL_MASK;
				subset0Endpoint1 = clamp(subset0Mean + subset0Axis * subset0Range.x, 0.0f, 255.0f) * CHANNEL_MASK;
				subset1Endpoint0 = clamp(subset1Mean + subset1Axis * subset1Range.y, 0.0f, 255.0f) * CHANNEL_MASK;
				subset1Endpoint1 = clamp(subset1Mean + subset1Axis * subset1Range.x, 0.0f, 255.0f) * CHANNEL_MASK;
				subset2Endpoint0 = clamp(subset2Mean + subset2Axis * subset2Range.y, 0.0f, 255.0f) * CHANNEL_MASK;
				subset2Endpoint1 = clamp(subset2Mean + subset2Axis * subset2Range.x, 0.0f, 255.0f) * CHANNEL_MASK;
			}

			// Each pass quantizes the endpoints, picks the best index for every texel, and (on all but the last pass) solves
			// the endpoints for those indices by least squares
			BC7QuantizedSubset subset0;
			BC7QuantizedSubset subset1;
			BC7QuantizedSubset subset2;
			float candidateError = 0.0f;
			for (uint refinePass = 0u; refinePass < BCN_REFINE_PASSES; ++refinePass)
			{
				subset0 = QuantizeBC7Subset(subset0Endpoint0, subset0Endpoint1, ENDPOINT_BITS, PBIT_MODE, CHANNEL_MASK);
				subset1 = QuantizeBC7Subset(subset1Endpoint0, subset1Endpoint1, ENDPOINT_BITS, PBIT_MODE, CHANNEL_MASK);
				subset2 = QuantizeBC7Subset(subset2Endpoint0, subset2Endpoint1, ENDPOINT_BITS, PBIT_MODE, CHANNEL_MASK);

				BC7LeastSquares subset0LeastSquares = CreateBC7LeastSquares();
				BC7LeastSquares subset1LeastSquares = CreateBC7LeastSquares();
				BC7LeastSquares subset2LeastSquares = CreateBC7LeastSquares();
				candidateError = 0.0f;
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float4 texel = gGroupTexels[texelBase + texelIndex] * CHANNEL_MASK;
					const float subset1Weight = (float)((subsetMasks >> texelIndex) & 1u);
					const float subset2Weight = (float)((subsetMasks >> (16u + texelIndex)) & 1u);
					const float subset0Weight = 1.0f - subset1Weight - subset2Weight;

					uint index;
					candidateError += FindBC7Index(texel,
						SelectBC7Subset(subset1Weight, subset2Weight, GetBC7PaletteBase(subset0), GetBC7PaletteBase(subset1), GetBC7PaletteBase(subset2)),
						SelectBC7Subset(subset1Weight, subset2Weight, GetBC7PaletteDelta(subset0), GetBC7PaletteDelta(subset1), GetBC7PaletteDelta(subset2)),
						INDEX_BITS, index);

					const float weightB = (float)GetBC7IndexWeight(index, INDEX_BITS) * (1.0f / 64.0f);
					AccumulateBC7LeastSquares(subset0LeastSquares, texel, weightB, subset0Weight);
					AccumulateBC7LeastSquares(subset1LeastSquares, texel, weightB, subset1Weight);
					AccumulateBC7LeastSquares(subset2LeastSquares, texel, weightB, subset2Weight);
				}

				if (refinePass + 1u < BCN_REFINE_PASSES)
				{
					SolveBC7LeastSquares(subset0LeastSquares, subset0Endpoint0, subset0Endpoint1);
					SolveBC7LeastSquares(subset1LeastSquares, subset1Endpoint0, subset1Endpoint1);
					SolveBC7LeastSquares(subset2LeastSquares, subset2Endpoint0, subset2Endpoint1);
				}
			}

			gCandidateErrors[threadIndex] = candidateError;

			GroupMemoryBarrierWithGroupSync();

			// The block's lowest-error candidate wins, with ties going to the better-ranked estimate
			const uint firstCandidateThread = blockInGroup * CANDIDATE_COUNT;
			uint winnerThread = firstCandidateThread;
			float winnerError = gCandidateErrors[firstCandidateThread];
			for (uint otherCandidate = 1u; otherCandidate < CANDIDATE_COUNT; ++otherCandidate)
			{
				const float otherError = gCandidateErrors[firstCandidateThread + otherCandidate];
				if (otherError < winnerError)
				{
					winnerError = otherError;
					winnerThread = firstCandidateThread + otherCandidate;
				}
			}

			if (!isValidBlock || threadIndex != winnerThread)
				return;

			// The mode decodes alpha as 255
			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				winnerError += gGroupTexels[texelBase + texelIndex].w;

			const float previousError = gBestErr[blockId];
			if (!(winnerError < previousError))
				return;

			// Anchor fix-up: the anchor texel of each subset stores its index without the top bit, so a subset whose anchor
			// index has the top bit set swaps its endpoints (and P-bits) and mirrors its indices
			const uint highestIndex = (1u << INDEX_BITS) - 1u;

			uint subset0AnchorIndex;
			uint subset1AnchorIndex;
			uint subset2AnchorIndex;
			FindBC7Index(gGroupTexels[texelBase] * CHANNEL_MASK, GetBC7PaletteBase(subset0), GetBC7PaletteDelta(subset0), INDEX_BITS, subset0AnchorIndex);
			FindBC7Index(gGroupTexels[texelBase + subset1AnchorTexelIndex] * CHANNEL_MASK, GetBC7PaletteBase(subset1), GetBC7PaletteDelta(subset1), INDEX_BITS, subset1AnchorIndex);
			FindBC7Index(gGroupTexels[texelBase + subset2AnchorTexelIndex] * CHANNEL_MASK, GetBC7PaletteBase(subset2), GetBC7PaletteDelta(subset2), INDEX_BITS, subset2AnchorIndex);

			const bool swapsSubset0 = subset0AnchorIndex > highestIndex / 2u;
			const bool swapsSubset1 = subset1AnchorIndex > highestIndex / 2u;
			const bool swapsSubset2 = subset2AnchorIndex > highestIndex / 2u;
			const uint packedCodes0 = swapsSubset0 ? subset0.Codes1 : subset0.Codes0;
			const uint packedCodes1 = swapsSubset0 ? subset0.Codes0 : subset0.Codes1;
			const uint packedCodes2 = swapsSubset1 ? subset1.Codes1 : subset1.Codes0;
			const uint packedCodes3 = swapsSubset1 ? subset1.Codes0 : subset1.Codes1;
			const uint packedCodes4 = swapsSubset2 ? subset2.Codes1 : subset2.Codes0;
			const uint packedCodes5 = swapsSubset2 ? subset2.Codes0 : subset2.Codes1;

			uint4 block = uint4(0, 0, 0, 0);
			uint position = 0u;
			PutBits(block, position, MODE_PREFIX, MODE_PREFIX_BITS);
			PutBits(block, position, partition, PARTITION_BITS);
			for (uint channel = 0u; channel < 3u; ++channel)
			{
				const uint shift = channel * ENDPOINT_BITS;
				PutBits(block, position, packedCodes0 >> shift, ENDPOINT_BITS);
				PutBits(block, position, packedCodes1 >> shift, ENDPOINT_BITS);
				PutBits(block, position, packedCodes2 >> shift, ENDPOINT_BITS);
				PutBits(block, position, packedCodes3 >> shift, ENDPOINT_BITS);
				PutBits(block, position, packedCodes4 >> shift, ENDPOINT_BITS);
				PutBits(block, position, packedCodes5 >> shift, ENDPOINT_BITS);
			}

			if (PBIT_MODE == BC7_PBIT_PER_ENDPOINT)
			{
				PutBits(block, position, swapsSubset0 ? subset0.Pbit1 : subset0.Pbit0, 1u);
				PutBits(block, position, swapsSubset0 ? subset0.Pbit0 : subset0.Pbit1, 1u);
				PutBits(block, position, swapsSubset1 ? subset1.Pbit1 : subset1.Pbit0, 1u);
				PutBits(block, position, swapsSubset1 ? subset1.Pbit0 : subset1.Pbit1, 1u);
				PutBits(block, position, swapsSubset2 ? subset2.Pbit1 : subset2.Pbit0, 1u);
				PutBits(block, position, swapsSubset2 ? subset2.Pbit0 : subset2.Pbit1, 1u);
			}

			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
			{
				const float subset1Weight = (float)((subsetMasks >> texelIndex) & 1u);
				const float subset2Weight = (float)((subsetMasks >> (16u + texelIndex)) & 1u);

				uint index;
				FindBC7Index(gGroupTexels[texelBase + texelIndex] * CHANNEL_MASK,
					SelectBC7Subset(subset1Weight, subset2Weight, GetBC7PaletteBase(subset0), GetBC7PaletteBase(subset1), GetBC7PaletteBase(subset2)),
					SelectBC7Subset(subset1Weight, subset2Weight, GetBC7PaletteDelta(subset0), GetBC7PaletteDelta(subset1), GetBC7PaletteDelta(subset2)),
					INDEX_BITS, index);

				const bool swaps = (subset2Weight != 0.0f) ? swapsSubset2 : ((subset1Weight != 0.0f) ? swapsSubset1 : swapsSubset0);
				if (swaps)
					index = highestIndex - index;

				const bool isAnchor = texelIndex == 0u || texelIndex == subset1AnchorTexelIndex || texelIndex == subset2AnchorTexelIndex;
				PutBits(block, position, index, isAnchor ? INDEX_BITS - 1u : INDEX_BITS);
			}

			gOutput[blockId] = block;
			gBestErr[blockId] = winnerError;
		}
	};
};
