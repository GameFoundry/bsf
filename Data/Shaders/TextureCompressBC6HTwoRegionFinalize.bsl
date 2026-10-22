#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode1.bslinc"
#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode2.bslinc"
#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode3.bslinc"
#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode4.bslinc"
#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode5.bslinc"
#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode6.bslinc"
#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode7.bslinc"
#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode8.bslinc"
#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode9.bslinc"
#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode10.bslinc"

shader TextureCompressBC6HTwoRegionFinalize
{
	featureset = HighEnd;

	mixin TextureCompressBC6Mode1;
	mixin TextureCompressBC6Mode2;
	mixin TextureCompressBC6Mode3;
	mixin TextureCompressBC6Mode4;
	mixin TextureCompressBC6Mode5;
	mixin TextureCompressBC6Mode6;
	mixin TextureCompressBC6Mode7;
	mixin TextureCompressBC6Mode8;
	mixin TextureCompressBC6Mode9;
	mixin TextureCompressBC6Mode10;

	variations
	{
		BC6_MODE = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		CANDIDATE_COUNT = { 2, 4, 8, 16, 32 };
	};

	code
	{
		// The two-region modes share the partitions and 3-bit indices, and differ in the precision of their endpoints. All
		// but mode 10 store the first endpoint of subset 0 (the base) with BASE_BITS and the other three as DELTA_BITS
		// signed deltas from it.
		#if BC6_MODE == 1
			#define BASE_BITS 10
			#define DELTA_BITS int3(5, 5, 5)
		#elif BC6_MODE == 2
			#define BASE_BITS 7
			#define DELTA_BITS int3(6, 6, 6)
		#elif BC6_MODE == 3
			#define BASE_BITS 11
			#define DELTA_BITS int3(5, 4, 4)
		#elif BC6_MODE == 4
			#define BASE_BITS 11
			#define DELTA_BITS int3(4, 5, 4)
		#elif BC6_MODE == 5
			#define BASE_BITS 11
			#define DELTA_BITS int3(4, 4, 5)
		#elif BC6_MODE == 6
			#define BASE_BITS 9
			#define DELTA_BITS int3(5, 5, 5)
		#elif BC6_MODE == 7
			#define BASE_BITS 8
			#define DELTA_BITS int3(6, 5, 5)
		#elif BC6_MODE == 8
			#define BASE_BITS 8
			#define DELTA_BITS int3(5, 6, 5)
		#elif BC6_MODE == 9
			#define BASE_BITS 8
			#define DELTA_BITS int3(5, 5, 6)
		#else
			#define BASE_BITS 6
			#define DELTA_BITS int3(6, 6, 6)
		#endif

		#define TRANSFORMED (BC6_MODE != 10)
		#define INDEX_BITS 3u

		Texture2D gInput;
		RWTexture2D<uint4> gOutput;
		RWTexture2D<float> gBestErr;
		StructuredBuffer<uint> gBC7TwoSubsetPartitions; // subset 1 texel mask | (subset 1 anchor texel << 16); BC6H uses the first 32
		RWStructuredBuffer<uint> gBC6HCandidates;

		[internal]
		cbuffer Parameters
		{
			int2 gTextureSize;
			int2 gBlockCount;
			int2 gBlockOffset;
			int2 gScratchBlockCount;

			// x: number of candidates the estimate wrote per block
			int4 gStageParameters;
		}

		#define BLOCKS_PER_GROUP (64 / CANDIDATE_COUNT)

		// Texels per block in groupshared memory. One more than a block has, so that threads reading the same texel of
		// different blocks hit different banks.
		#define TEXEL_STRIDE 17

		groupshared float3 gGroupTexels[BLOCKS_PER_GROUP * TEXEL_STRIDE];
		groupshared float gCandidateErrors[64];

		// Texels are stored without alpha, and widened with a constant zero alpha so that the fits don't spend registers on it
		float4 LoadGroupTexel(uint index)
		{
			return float4(gGroupTexels[index], 0.0f);
		}

		// Each group encodes BLOCKS_PER_GROUP consecutive blocks of one row, with one thread per (block, candidate
		// partition). Texels are half-float bits. Every thread fits, quantizes and refines its partition, and measures the
		// error of the block as the decoder reconstructs it. The lowest-error candidate of each block is packed and replaces
		// the block's current encoding if it has a lower error.
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
				gGroupTexels[(loadIndex / 16u) * TEXEL_STRIDE + texelIndex] = GetBC6HalfBits(gInput.Load(int3((int2)coordinate, 0))).xyz;
			}

			GroupMemoryBarrierWithGroupSync();

			const bool isValidBlock = firstBlockColumn + blockInGroup <= lastBlockColumn;
			const uint2 localBlockId = uint2(min(firstBlockColumn + blockInGroup, lastBlockColumn), groupId.y);
			const uint2 blockId = localBlockId + (uint2)gBlockOffset;
			const uint partition = gBC6HCandidates[(localBlockId.y * (uint)gScratchBlockCount.x + localBlockId.x) * (uint)gStageParameters.x + candidateIndex];
			const uint partitionEntry = gBC7TwoSubsetPartitions[partition];
			const uint subset1Mask = partitionEntry & 0xFFFFu;
			const uint subset1AnchorTexelIndex = partitionEntry >> 16u;
			const uint texelBase = blockInGroup * TEXEL_STRIDE;

			// Initial endpoints: each subset's principal axis, spanning the extreme projections of the subset's texels.
			// Endpoint 0 is the high end of the axis. Moments are accumulated relative to the block mean to keep their
			// precision for values up to 0x7BFF. The bounds of each subset limit the least-squares solves below.
			float4 subset0Endpoint0;
			float4 subset0Endpoint1;
			float4 subset1Endpoint0;
			float4 subset1Endpoint1;
			float4 subset0Minimum = float4(1e30f, 1e30f, 1e30f, 0.0f);
			float4 subset0Maximum = float4(-1e30f, -1e30f, -1e30f, 0.0f);
			float4 subset1Minimum = float4(1e30f, 1e30f, 1e30f, 0.0f);
			float4 subset1Maximum = float4(-1e30f, -1e30f, -1e30f, 0.0f);
			{
				float4 blockMean = float4(0, 0, 0, 0);
				for (uint meanTexelIndex = 0u; meanTexelIndex < 16u; ++meanTexelIndex)
					blockMean += LoadGroupTexel(texelBase + meanTexelIndex);

				blockMean *= 1.0f / 16.0f;

				BC7Moments blockMoments = CreateBC7Moments();
				BC7Moments subset1Moments = CreateBC7Moments();
				float4 blockSum = float4(0, 0, 0, 0);
				float4 subset1Sum = float4(0, 0, 0, 0);
				float subset1Count = 0.0f;
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float4 texel = LoadGroupTexel(texelBase + texelIndex);
					const float4 centeredTexel = texel - blockMean;
					const float subset1Weight = (float)((subset1Mask >> texelIndex) & 1u);

					blockSum += centeredTexel;
					AccumulateBC7Moments(blockMoments, centeredTexel, 1.0f);

					subset1Count += subset1Weight;
					subset1Sum += subset1Weight * centeredTexel;
					AccumulateBC7Moments(subset1Moments, centeredTexel, subset1Weight);

					if (subset1Weight != 0.0f)
					{
						subset1Minimum = min(subset1Minimum, texel);
						subset1Maximum = max(subset1Maximum, texel);
					}
					else
					{
						subset0Minimum = min(subset0Minimum, texel);
						subset0Maximum = max(subset0Maximum, texel);
					}
				}

				const float subset0Count = 16.0f - subset1Count;
				const float4 subset0Sum = blockSum - subset1Sum;
				const float4 subset0Mean = subset0Sum / max(subset0Count, 1.0f) + blockMean;
				const float4 subset1Mean = subset1Sum / max(subset1Count, 1.0f) + blockMean;
				const float4 subset0Axis = FindBC7PrincipalAxis(CenterBC7Moments(SubtractBC7Moments(blockMoments, subset1Moments), subset0Sum, subset0Count));
				const float4 subset1Axis = FindBC7PrincipalAxis(CenterBC7Moments(subset1Moments, subset1Sum, subset1Count));

				// Minimum and maximum projection onto the axis
				float2 subset0Range = float2(1e30f, -1e30f);
				float2 subset1Range = float2(1e30f, -1e30f);
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float4 texel = LoadGroupTexel(texelBase + texelIndex);
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

				subset0Endpoint0 = subset0Mean + subset0Axis * subset0Range.y;
				subset0Endpoint1 = subset0Mean + subset0Axis * subset0Range.x;
				subset1Endpoint0 = subset1Mean + subset1Axis * subset1Range.y;
				subset1Endpoint1 = subset1Mean + subset1Axis * subset1Range.x;
			}

			// Each pass quantizes the endpoints to absolute BASE_BITS codes and picks the best index for every texel from
			// the palette of those codes. On all but the last pass, it then solves the endpoints for those indices by least
			// squares. Every pass also measures the error as the decoder sees it: with the anchor fix-ups applied and the
			// endpoints stored as clamped deltas, the decoded endpoints may differ from the absolute codes.
			int3 storedCodes[4];
			float candidateError = 0.0f;
			for (uint refinePass = 0u; refinePass < BCN_REFINE_PASSES; ++refinePass)
			{
				int3 subset0Code0 = QuantizeVecBC6(subset0Endpoint0, BASE_BITS);
				int3 subset0Code1 = QuantizeVecBC6(subset0Endpoint1, BASE_BITS);
				int3 subset1Code0 = QuantizeVecBC6(subset1Endpoint0, BASE_BITS);
				int3 subset1Code1 = QuantizeVecBC6(subset1Endpoint1, BASE_BITS);
				const int3 subset0Unquantized0 = UnquantizeBC6Vector(subset0Code0, BASE_BITS);
				const int3 subset0Unquantized1 = UnquantizeBC6Vector(subset0Code1, BASE_BITS);
				const int3 subset1Unquantized0 = UnquantizeBC6Vector(subset1Code0, BASE_BITS);
				const int3 subset1Unquantized1 = UnquantizeBC6Vector(subset1Code1, BASE_BITS);

				// Anchor fix-up: the anchor texel of each subset stores its index without the top bit, so a subset whose
				// anchor index has the top bit set swaps its endpoints and mirrors its indices. The 3-bit ramp is symmetric,
				// so this changes nothing but which endpoint is the base.
				uint subset0AnchorIndex;
				uint subset1AnchorIndex;
				FindBC6Index((int3)gGroupTexels[texelBase].xyz, subset0Unquantized0, subset0Unquantized1, INDEX_BITS, subset0AnchorIndex);
				FindBC6Index((int3)gGroupTexels[texelBase + subset1AnchorTexelIndex].xyz, subset1Unquantized0, subset1Unquantized1, INDEX_BITS, subset1AnchorIndex);

				const bool swapsSubset0 = subset0AnchorIndex >= 4u;
				const bool swapsSubset1 = subset1AnchorIndex >= 4u;
				const int3 base = swapsSubset0 ? subset0Code1 : subset0Code0;
				storedCodes[0] = base;
				const int3 decodedSubset0Code1 = TransformBC6Endpoint(swapsSubset0 ? subset0Code0 : subset0Code1, base, BASE_BITS, DELTA_BITS, TRANSFORMED, storedCodes[1]);
				const int3 decodedSubset1Code0 = TransformBC6Endpoint(swapsSubset1 ? subset1Code1 : subset1Code0, base, BASE_BITS, DELTA_BITS, TRANSFORMED, storedCodes[2]);
				const int3 decodedSubset1Code1 = TransformBC6Endpoint(swapsSubset1 ? subset1Code0 : subset1Code1, base, BASE_BITS, DELTA_BITS, TRANSFORMED, storedCodes[3]);
				const int3 decodedSubset0Unquantized0 = UnquantizeBC6Vector(base, BASE_BITS);
				const int3 decodedSubset0Unquantized1 = UnquantizeBC6Vector(decodedSubset0Code1, BASE_BITS);
				const int3 decodedSubset1Unquantized0 = UnquantizeBC6Vector(decodedSubset1Code0, BASE_BITS);
				const int3 decodedSubset1Unquantized1 = UnquantizeBC6Vector(decodedSubset1Code1, BASE_BITS);

				BC7LeastSquares subset0LeastSquares = CreateBC7LeastSquares();
				BC7LeastSquares subset1LeastSquares = CreateBC7LeastSquares();
				candidateError = 0.0f;
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float4 texel = LoadGroupTexel(texelBase + texelIndex);
					const int3 texelBits = (int3)texel.xyz;
					const float subset1Weight = (float)((subset1Mask >> texelIndex) & 1u);
					const bool isInSubset1 = subset1Weight != 0.0f;

					uint index;
					FindBC6Index(texelBits, isInSubset1 ? subset1Unquantized0 : subset0Unquantized0, isInSubset1 ? subset1Unquantized1 : subset0Unquantized1, INDEX_BITS, index);

					const uint storedIndex = (isInSubset1 ? swapsSubset1 : swapsSubset0) ? 7u - index : index;
					candidateError += GetBC6PaletteDistance(texelBits, isInSubset1 ? decodedSubset1Unquantized0 : decodedSubset0Unquantized0,
						isInSubset1 ? decodedSubset1Unquantized1 : decodedSubset0Unquantized1, storedIndex, INDEX_BITS);

					const float weightB = (float)GetBC7IndexWeight(index, INDEX_BITS) * (1.0f / 64.0f);
					AccumulateBC7LeastSquares(subset0LeastSquares, texel, weightB, 1.0f - subset1Weight);
					AccumulateBC7LeastSquares(subset1LeastSquares, texel, weightB, subset1Weight);
				}

				if (refinePass + 1u < BCN_REFINE_PASSES)
				{
					SolveBC6LeastSquares(subset0LeastSquares, subset0Minimum, subset0Maximum, subset0Endpoint0, subset0Endpoint1);
					SolveBC6LeastSquares(subset1LeastSquares, subset1Minimum, subset1Maximum, subset1Endpoint0, subset1Endpoint1);
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

			const float previousError = gBestErr[blockId];
			if (!(winnerError < previousError))
				return;

			#if BC6_MODE == 1
				uint4 block = PackBC6Mode1Endpoints(storedCodes, partition);
			#elif BC6_MODE == 2
				uint4 block = PackBC6Mode2Endpoints(storedCodes, partition);
			#elif BC6_MODE == 3
				uint4 block = PackBC6Mode3Endpoints(storedCodes, partition);
			#elif BC6_MODE == 4
				uint4 block = PackBC6Mode4Endpoints(storedCodes, partition);
			#elif BC6_MODE == 5
				uint4 block = PackBC6Mode5Endpoints(storedCodes, partition);
			#elif BC6_MODE == 6
				uint4 block = PackBC6Mode6Endpoints(storedCodes, partition);
			#elif BC6_MODE == 7
				uint4 block = PackBC6Mode7Endpoints(storedCodes, partition);
			#elif BC6_MODE == 8
				uint4 block = PackBC6Mode8Endpoints(storedCodes, partition);
			#elif BC6_MODE == 9
				uint4 block = PackBC6Mode9Endpoints(storedCodes, partition);
			#else
				uint4 block = PackBC6Mode10Endpoints(storedCodes, partition);
			#endif

			// Indices, as chosen on the last pass, from bit 82: the anchors of both subsets store 2 bits, the others 3
			const int3 subset0Unquantized0 = UnquantizeBC6Vector(QuantizeVecBC6(subset0Endpoint0, BASE_BITS), BASE_BITS);
			const int3 subset0Unquantized1 = UnquantizeBC6Vector(QuantizeVecBC6(subset0Endpoint1, BASE_BITS), BASE_BITS);
			const int3 subset1Unquantized0 = UnquantizeBC6Vector(QuantizeVecBC6(subset1Endpoint0, BASE_BITS), BASE_BITS);
			const int3 subset1Unquantized1 = UnquantizeBC6Vector(QuantizeVecBC6(subset1Endpoint1, BASE_BITS), BASE_BITS);

			uint subset0AnchorIndex;
			uint subset1AnchorIndex;
			FindBC6Index((int3)gGroupTexels[texelBase].xyz, subset0Unquantized0, subset0Unquantized1, INDEX_BITS, subset0AnchorIndex);
			FindBC6Index((int3)gGroupTexels[texelBase + subset1AnchorTexelIndex].xyz, subset1Unquantized0, subset1Unquantized1, INDEX_BITS, subset1AnchorIndex);

			uint position = 82u;
			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
			{
				const bool isInSubset1 = ((subset1Mask >> texelIndex) & 1u) != 0u;

				uint index;
				FindBC6Index((int3)gGroupTexels[texelBase + texelIndex].xyz, isInSubset1 ? subset1Unquantized0 : subset0Unquantized0, isInSubset1 ? subset1Unquantized1 : subset0Unquantized1, INDEX_BITS, index);
				if ((isInSubset1 ? subset1AnchorIndex : subset0AnchorIndex) >= 4u)
					index = 7u - index;

				PutBits(block, position, index, (texelIndex == 0u || texelIndex == subset1AnchorTexelIndex) ? INDEX_BITS - 1u : INDEX_BITS);
			}

			gOutput[blockId] = block;
			gBestErr[blockId] = winnerError;
		}
	};
};
