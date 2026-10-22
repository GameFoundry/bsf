#include "$ENGINE$\TextureCompression\TextureCompressBC7SubsetFit.bslinc"

shader TextureCompressBC7TwoSubsetFinalize
{
	featureset = HighEnd;

	mixin TextureCompressBC7SubsetFit;

	variations
	{
		BC7_MODE = { 1, 3, 7 };
		CANDIDATE_COUNT = { 2, 4, 8, 16, 32, 64 };
	};

	code
	{
		// The two-subset modes share the partitions and differ in their endpoint and index precision (block layouts
		// LSB->MSB, endpoint codes ordered subset 0 endpoint 0, subset 0 endpoint 1, subset 1 endpoint 0, subset 1
		// endpoint 1 within each channel; the anchor texels store their index with one bit less):
		//   mode 1: "01"       | partition(6) | RGB codes (6 bits) | 1 P-bit per subset   | 3-bit indices
		//   mode 3: "0001"     | partition(6) | RGB codes (7 bits) | 1 P-bit per endpoint | 2-bit indices
		//   mode 7: "00000001" | partition(6) | RGBA codes (5 bits) | 1 P-bit per endpoint | 2-bit indices
		// The RGB modes decode alpha as 255, so their error includes the block's distance from opaque.
		#if BC7_MODE == 1
			#define MODE_PREFIX 0x2u
			#define MODE_PREFIX_BITS 2u
			#define ENDPOINT_BITS 6u
			#define PBIT_MODE BC7_PBIT_PER_SUBSET
			#define INDEX_BITS 3u
			#define CHANNEL_COUNT 3u
		#elif BC7_MODE == 3
			#define MODE_PREFIX 0x8u
			#define MODE_PREFIX_BITS 4u
			#define ENDPOINT_BITS 7u
			#define PBIT_MODE BC7_PBIT_PER_ENDPOINT
			#define INDEX_BITS 2u
			#define CHANNEL_COUNT 3u
		#else
			#define MODE_PREFIX 0x80u
			#define MODE_PREFIX_BITS 8u
			#define ENDPOINT_BITS 5u
			#define PBIT_MODE BC7_PBIT_PER_ENDPOINT
			#define INDEX_BITS 2u
			#define CHANNEL_COUNT 4u
		#endif

		#define CHANNEL_MASK ((CHANNEL_COUNT == 4u) ? float4(1, 1, 1, 1) : float4(1, 1, 1, 0))

		Texture2D gInput;
		RWTexture2D<uint4> gOutput;
		RWTexture2D<float> gBestErr;
		StructuredBuffer<uint> gBC7TwoSubsetPartitions; // subset 1 texel mask | (subset 1 anchor texel << 16)
		RWStructuredBuffer<uint> gBC7TwoSubsetCandidates;

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

		// Returns the texel as the mode encodes it: alpha is dropped by the RGB modes
		float4 LoadBlockTexel(uint texelIndex)
		{
			return gGroupTexels[texelIndex] * CHANNEL_MASK;
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

			// Groups extending past the end of the row repeat its last block, and never write it
			for (uint loadIndex = threadIndex; loadIndex < BLOCKS_PER_GROUP * 16u; loadIndex += 64u)
			{
				const uint2 loadBlockId = uint2(min(firstBlockColumn + loadIndex / 16u, lastBlockColumn), groupId.y) + (uint2)gBlockOffset;
				const uint texelIndex = loadIndex % 16u;
				const uint2 coordinate = min(loadBlockId * 4u + uint2(texelIndex & 3u, texelIndex >> 2u), maxCoordinate);
				gGroupTexels[(loadIndex / 16u) * TEXEL_STRIDE + texelIndex] = round(saturate(gInput.Load(int3((int2)coordinate, 0))) * 255.0f);
			}

			GroupMemoryBarrierWithGroupSync();

			const bool isValidBlock = firstBlockColumn + blockInGroup <= lastBlockColumn;
			const uint2 localBlockId = uint2(min(firstBlockColumn + blockInGroup, lastBlockColumn), groupId.y);
			const uint2 blockId = localBlockId + (uint2)gBlockOffset;
			const uint candidateBase = (localBlockId.y * (uint)gScratchBlockCount.x + localBlockId.x) * (uint)gStageParameters.x + (uint)gStageParameters.y;
			const uint partition = gBC7TwoSubsetCandidates[candidateBase + candidateIndex];
			const uint texelBase = blockInGroup * TEXEL_STRIDE;

			// The estimate leaves the list empty (partition 64) for blocks the mode can't win
			const bool isEmpty = partition >= 64u;
			const uint partitionEntry = gBC7TwoSubsetPartitions[isEmpty ? 0u : partition];
			const uint subset1Mask = partitionEntry & 0xFFFFu;
			const uint subset1AnchorTexelIndex = partitionEntry >> 16u;

			// Initial endpoints: each subset's principal axis, spanning the extreme projections of the subset's texels.
			// Endpoint 0 is the high end of the axis.
			float4 subset0Endpoint0;
			float4 subset0Endpoint1;
			float4 subset1Endpoint0;
			float4 subset1Endpoint1;
			{
				BC7Moments blockMoments = CreateBC7Moments();
				BC7Moments subset1Moments = CreateBC7Moments();
				float4 blockSum = float4(0, 0, 0, 0);
				float4 subset1Sum = float4(0, 0, 0, 0);
				float subset1Count = 0.0f;
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float4 texel = LoadBlockTexel(texelBase + texelIndex);
					const float subset1Weight = (float)((subset1Mask >> texelIndex) & 1u);

					blockSum += texel;
					AccumulateBC7Moments(blockMoments, texel, 1.0f);

					subset1Count += subset1Weight;
					subset1Sum += subset1Weight * texel;
					AccumulateBC7Moments(subset1Moments, texel, subset1Weight);
				}

				const float subset0Count = 16.0f - subset1Count;
				const float4 subset0Sum = blockSum - subset1Sum;
				const float4 subset0Mean = subset0Sum / max(subset0Count, 1.0f);
				const float4 subset1Mean = subset1Sum / max(subset1Count, 1.0f);
				const float4 subset0Axis = FindBC7PrincipalAxis(CenterBC7Moments(SubtractBC7Moments(blockMoments, subset1Moments), subset0Sum, subset0Count)) * CHANNEL_MASK;
				const float4 subset1Axis = FindBC7PrincipalAxis(CenterBC7Moments(subset1Moments, subset1Sum, subset1Count)) * CHANNEL_MASK;

				// Minimum and maximum projection onto the axis
				float2 subset0Range = float2(1e30f, -1e30f);
				float2 subset1Range = float2(1e30f, -1e30f);
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float4 texel = LoadBlockTexel(texelBase + texelIndex);
					if (((subset1Mask >> texelIndex) & 1u) != 0u)
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

				subset0Endpoint0 = clamp(subset0Mean + subset0Axis * subset0Range.y, 0.0f, 255.0f);
				subset0Endpoint1 = clamp(subset0Mean + subset0Axis * subset0Range.x, 0.0f, 255.0f);
				subset1Endpoint0 = clamp(subset1Mean + subset1Axis * subset1Range.y, 0.0f, 255.0f);
				subset1Endpoint1 = clamp(subset1Mean + subset1Axis * subset1Range.x, 0.0f, 255.0f);
			}

			// Each pass quantizes the endpoints, picks the best index for every texel, and (on all but the last pass) solves
			// the endpoints for those indices by least squares
			BC7QuantizedSubset subset0;
			BC7QuantizedSubset subset1;
			float candidateError = 0.0f;
			for (uint refinePass = 0u; refinePass < BCN_REFINE_PASSES; ++refinePass)
			{
				subset0 = QuantizeBC7Subset(subset0Endpoint0, subset0Endpoint1, ENDPOINT_BITS, PBIT_MODE, CHANNEL_MASK);
				subset1 = QuantizeBC7Subset(subset1Endpoint0, subset1Endpoint1, ENDPOINT_BITS, PBIT_MODE, CHANNEL_MASK);

				const float4 subset0PaletteBase = GetBC7PaletteBase(subset0);
				const float4 subset0PaletteDelta = GetBC7PaletteDelta(subset0);
				const float4 subset1PaletteBase = GetBC7PaletteBase(subset1);
				const float4 subset1PaletteDelta = GetBC7PaletteDelta(subset1);

				BC7LeastSquares subset0LeastSquares = CreateBC7LeastSquares();
				BC7LeastSquares subset1LeastSquares = CreateBC7LeastSquares();
				candidateError = 0.0f;
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float4 texel = LoadBlockTexel(texelBase + texelIndex);
					const float subset1Weight = (float)((subset1Mask >> texelIndex) & 1u);
					const bool isInSubset1 = subset1Weight != 0.0f;

					uint index;
					candidateError += FindBC7Index(texel, isInSubset1 ? subset1PaletteBase : subset0PaletteBase, isInSubset1 ? subset1PaletteDelta : subset0PaletteDelta, INDEX_BITS, index);

					const float weightB = (float)GetBC7IndexWeight(index, INDEX_BITS) * (1.0f / 64.0f);
					AccumulateBC7LeastSquares(subset0LeastSquares, texel, weightB, 1.0f - subset1Weight);
					AccumulateBC7LeastSquares(subset1LeastSquares, texel, weightB, subset1Weight);
				}

				if (refinePass + 1u < BCN_REFINE_PASSES)
				{
					SolveBC7LeastSquares(subset0LeastSquares, subset0Endpoint0, subset0Endpoint1);
					SolveBC7LeastSquares(subset1LeastSquares, subset1Endpoint0, subset1Endpoint1);
				}
			}

			gCandidateErrors[threadIndex] = isEmpty ? 1e30f : candidateError;

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

			if (!isValidBlock || isEmpty || threadIndex != winnerThread)
				return;

			// The RGB modes decode alpha as 255
			if (CHANNEL_COUNT == 3u)
			{
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float alphaDelta = gGroupTexels[texelBase + texelIndex].w - 255.0f;
					winnerError += alphaDelta * alphaDelta;
				}
			}

			const float previousError = gBestErr[blockId];
			if (!(winnerError < previousError))
				return;

			// Anchor fix-up: the anchor texel of each subset stores its index without the top bit, so a subset whose anchor
			// index has the top bit set swaps its endpoints (and P-bits) and mirrors its indices
			const float4 subset0PaletteBase = GetBC7PaletteBase(subset0);
			const float4 subset0PaletteDelta = GetBC7PaletteDelta(subset0);
			const float4 subset1PaletteBase = GetBC7PaletteBase(subset1);
			const float4 subset1PaletteDelta = GetBC7PaletteDelta(subset1);
			const uint highestIndex = (1u << INDEX_BITS) - 1u;

			uint subset0AnchorIndex;
			uint subset1AnchorIndex;
			FindBC7Index(LoadBlockTexel(texelBase), subset0PaletteBase, subset0PaletteDelta, INDEX_BITS, subset0AnchorIndex);
			FindBC7Index(LoadBlockTexel(texelBase + subset1AnchorTexelIndex), subset1PaletteBase, subset1PaletteDelta, INDEX_BITS, subset1AnchorIndex);

			const bool swapsSubset0 = subset0AnchorIndex > highestIndex / 2u;
			const bool swapsSubset1 = subset1AnchorIndex > highestIndex / 2u;
			const uint packedCodes0 = swapsSubset0 ? subset0.Codes1 : subset0.Codes0;
			const uint packedCodes1 = swapsSubset0 ? subset0.Codes0 : subset0.Codes1;
			const uint packedCodes2 = swapsSubset1 ? subset1.Codes1 : subset1.Codes0;
			const uint packedCodes3 = swapsSubset1 ? subset1.Codes0 : subset1.Codes1;

			uint4 block = uint4(0, 0, 0, 0);
			uint position = 0u;
			PutBits(block, position, MODE_PREFIX, MODE_PREFIX_BITS);
			PutBits(block, position, partition, 6u);
			for (uint channel = 0u; channel < CHANNEL_COUNT; ++channel)
			{
				const uint shift = channel * ENDPOINT_BITS;
				PutBits(block, position, packedCodes0 >> shift, ENDPOINT_BITS);
				PutBits(block, position, packedCodes1 >> shift, ENDPOINT_BITS);
				PutBits(block, position, packedCodes2 >> shift, ENDPOINT_BITS);
				PutBits(block, position, packedCodes3 >> shift, ENDPOINT_BITS);
			}

			if (PBIT_MODE == BC7_PBIT_PER_SUBSET)
			{
				PutBits(block, position, subset0.Pbit0, 1u);
				PutBits(block, position, subset1.Pbit0, 1u);
			}
			else
			{
				PutBits(block, position, swapsSubset0 ? subset0.Pbit1 : subset0.Pbit0, 1u);
				PutBits(block, position, swapsSubset0 ? subset0.Pbit0 : subset0.Pbit1, 1u);
				PutBits(block, position, swapsSubset1 ? subset1.Pbit1 : subset1.Pbit0, 1u);
				PutBits(block, position, swapsSubset1 ? subset1.Pbit0 : subset1.Pbit1, 1u);
			}

			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
			{
				const bool isInSubset1 = ((subset1Mask >> texelIndex) & 1u) != 0u;

				uint index;
				FindBC7Index(LoadBlockTexel(texelBase + texelIndex), isInSubset1 ? subset1PaletteBase : subset0PaletteBase, isInSubset1 ? subset1PaletteDelta : subset0PaletteDelta, INDEX_BITS, index);
				if (isInSubset1 ? swapsSubset1 : swapsSubset0)
					index = highestIndex - index;

				PutBits(block, position, index, (texelIndex == 0u || texelIndex == subset1AnchorTexelIndex) ? INDEX_BITS - 1u : INDEX_BITS);
			}

			gOutput[blockId] = block;
			gBestErr[blockId] = winnerError;
		}
	};
};
