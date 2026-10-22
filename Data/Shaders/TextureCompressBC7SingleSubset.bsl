#include "$ENGINE$\TextureCompression\TextureCompressBC7SubsetFit.bslinc"

shader TextureCompressBC7SingleSubset
{
	featureset = HighEnd;

	mixin TextureCompressBC7SubsetFit;

	variations
	{
		BC7_MODE = { 4, 5, 6 };
	};

	code
	{
		// The single-subset modes have no partitions. Modes 4 and 5 store color and alpha as separate lines with their own
		// indices, and may first swap alpha with one color channel (the rotation) to give that channel its own indices.
		// Mode 4 additionally chooses which of the two index sets gets 3 bits (the index selector). Every choice is one
		// candidate, encoded by its own thread. Block layouts (LSB->MSB, endpoint codes ordered endpoint 0, endpoint 1
		// within each channel; the anchor texel 0 stores its indices with one bit less):
		//   mode 4: "00001"   | rotation(2) | selector(1) | RGB codes (5 bits) | alpha codes (6 bits) | 2-bit indices | 3-bit indices
		//   mode 5: "000001"  | rotation(2) | RGB codes (7 bits) | alpha codes (8 bits) | color indices (2 bits) | alpha indices (2 bits)
		//   mode 6: "0000001" | RGBA codes (7 bits) | 1 P-bit per endpoint | 4-bit indices
		#if BC7_MODE == 4
			#define CANDIDATE_COUNT 8u
			#define MODE_PREFIX 0x10u
			#define MODE_PREFIX_BITS 5u
			#define COLOR_ENDPOINT_BITS 5u
			#define ALPHA_ENDPOINT_BITS 6u
		#elif BC7_MODE == 5
			#define CANDIDATE_COUNT 4u
			#define MODE_PREFIX 0x20u
			#define MODE_PREFIX_BITS 6u
			#define COLOR_ENDPOINT_BITS 7u
			#define ALPHA_ENDPOINT_BITS 8u
		#else
			#define CANDIDATE_COUNT 1u
			#define MODE_PREFIX 0x40u
			#define MODE_PREFIX_BITS 7u
			#define ENDPOINT_BITS 7u
			#define INDEX_BITS 4u
		#endif

		#define SEPARATE_ALPHA (BC7_MODE != 6)

		Texture2D gInput;
		RWTexture2D<uint4> gOutput;
		RWTexture2D<float> gBestErr;

		[internal]
		cbuffer Parameters
		{
			int2 gTextureSize;
			int2 gBlockCount;
			int2 gBlockOffset;
			int2 gScratchBlockCount;
		}

		#define BLOCKS_PER_GROUP (64u / CANDIDATE_COUNT)

		// Texels per block in groupshared memory. One more than a block has, so that threads reading the same texel of
		// different blocks hit different banks.
		#define TEXEL_STRIDE 17u

		groupshared float4 gGroupTexels[BLOCKS_PER_GROUP * TEXEL_STRIDE];
		groupshared float gCandidateErrors[64];

		// Swaps alpha with the color channel the rotation selects: 1 = red, 2 = green, 3 = blue
		float4 RotateBC7Texel(float4 texel, uint rotation)
		{
			if (rotation == 1u)
				return texel.agbr;
			else if (rotation == 2u)
				return texel.rabg;
			else if (rotation == 3u)
				return texel.rgab;

			return texel;
		}

		// Each group encodes BLOCKS_PER_GROUP consecutive blocks of one row, with one thread per (block, candidate). The
		// lowest-error candidate of each block is packed and replaces the block's current encoding if it has a lower error.
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
			const uint2 blockId = uint2(min(firstBlockColumn + blockInGroup, lastBlockColumn), groupId.y) + (uint2)gBlockOffset;
			const uint texelBase = blockInGroup * TEXEL_STRIDE;

		#if SEPARATE_ALPHA
			// Mode 4's selector gives the color indices 3 bits (and alpha 2) when set. Mode 5 uses 2 bits for both.
			const uint rotation = (BC7_MODE == 4) ? (candidateIndex >> 1u) : candidateIndex;
			const uint selector = (BC7_MODE == 4) ? (candidateIndex & 1u) : 0u;
			const uint colorIndexBits = (selector != 0u) ? 3u : 2u;
			const uint alphaIndexBits = (BC7_MODE == 4 && selector == 0u) ? 3u : 2u;
			const float4 colorMask = float4(1, 1, 1, 0);
			const float4 alphaMask = float4(1, 0, 0, 0);
		#else
			const uint rotation = 0u;
		#endif

			// Initial endpoints: the principal axis of the colors spanning their extreme projections, and the alpha range.
			// Endpoint 0 is the high end.
			float4 endpoint0;
			float4 endpoint1;
			{
				BC7Moments moments = CreateBC7Moments();
				float4 sum = float4(0, 0, 0, 0);
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
				#if SEPARATE_ALPHA
					const float4 texel = RotateBC7Texel(gGroupTexels[texelBase + texelIndex], rotation) * colorMask;
				#else
					const float4 texel = gGroupTexels[texelBase + texelIndex];
				#endif

					sum += texel;
					AccumulateBC7Moments(moments, texel, 1.0f);
				}

				const float4 mean = sum * (1.0f / 16.0f);
				const float4 axis = FindBC7PrincipalAxis(CenterBC7Moments(moments, sum, 16.0f));

				float2 range = float2(1e30f, -1e30f);
				float2 alphaRange = float2(255.0f, 0.0f);
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float4 texel = RotateBC7Texel(gGroupTexels[texelBase + texelIndex], rotation);
				#if SEPARATE_ALPHA
					alphaRange = float2(min(alphaRange.x, texel.w), max(alphaRange.y, texel.w));
					const float projection = dot(texel * colorMask - mean, axis);
				#else
					const float projection = dot(texel - mean, axis);
				#endif
					range = float2(min(range.x, projection), max(range.y, projection));
				}

				endpoint0 = clamp(mean + axis * range.y, 0.0f, 255.0f);
				endpoint1 = clamp(mean + axis * range.x, 0.0f, 255.0f);
			#if SEPARATE_ALPHA
				endpoint0.w = alphaRange.y;
				endpoint1.w = alphaRange.x;
			#endif
			}

			// Each pass quantizes the endpoints, picks the best indices for every texel, and (on all but the last pass) solves
			// the endpoints for those indices by least squares. Modes 4 and 5 quantize and solve color and alpha separately,
			// with alpha kept in the x channel.
		#if SEPARATE_ALPHA
			BC7QuantizedSubset color;
			BC7QuantizedSubset alpha;
		#else
			BC7QuantizedSubset color;
		#endif
			float candidateError = 0.0f;
			for (uint refinePass = 0u; refinePass < BCN_REFINE_PASSES; ++refinePass)
			{
			#if SEPARATE_ALPHA
				color = QuantizeBC7Subset(endpoint0, endpoint1, COLOR_ENDPOINT_BITS, BC7_PBIT_NONE, colorMask);
				alpha = QuantizeBC7Subset(endpoint0.wwww, endpoint1.wwww, ALPHA_ENDPOINT_BITS, BC7_PBIT_NONE, alphaMask);
				BC7LeastSquares alphaLeastSquares = CreateBC7LeastSquares();
			#else
				color = QuantizeBC7Subset(endpoint0, endpoint1, ENDPOINT_BITS, BC7_PBIT_PER_ENDPOINT, float4(1, 1, 1, 1));
			#endif
				BC7LeastSquares colorLeastSquares = CreateBC7LeastSquares();

				candidateError = 0.0f;
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float4 texel = RotateBC7Texel(gGroupTexels[texelBase + texelIndex], rotation);

				#if SEPARATE_ALPHA
					uint colorIndex;
					uint alphaIndex;
					candidateError += FindBC7Index(texel * colorMask, GetBC7PaletteBase(color), GetBC7PaletteDelta(color), colorIndexBits, colorIndex);
					candidateError += FindBC7Index(texel.wwww * alphaMask, GetBC7PaletteBase(alpha), GetBC7PaletteDelta(alpha), alphaIndexBits, alphaIndex);
					AccumulateBC7LeastSquares(colorLeastSquares, texel * colorMask, (float)GetBC7IndexWeight(colorIndex, colorIndexBits) * (1.0f / 64.0f), 1.0f);
					AccumulateBC7LeastSquares(alphaLeastSquares, texel.wwww * alphaMask, (float)GetBC7IndexWeight(alphaIndex, alphaIndexBits) * (1.0f / 64.0f), 1.0f);
				#else
					uint index;
					candidateError += FindBC7Index(texel, GetBC7PaletteBase(color), GetBC7PaletteDelta(color), INDEX_BITS, index);
					AccumulateBC7LeastSquares(colorLeastSquares, texel, (float)GetBC7IndexWeight(index, INDEX_BITS) * (1.0f / 64.0f), 1.0f);
				#endif
				}

				if (refinePass + 1u < BCN_REFINE_PASSES)
				{
				#if SEPARATE_ALPHA
					const float alphaEndpoint0 = endpoint0.w;
					const float alphaEndpoint1 = endpoint1.w;
					float4 solvedAlpha0 = float4(alphaEndpoint0, 0, 0, 0);
					float4 solvedAlpha1 = float4(alphaEndpoint1, 0, 0, 0);
					SolveBC7LeastSquares(colorLeastSquares, endpoint0, endpoint1);
					SolveBC7LeastSquares(alphaLeastSquares, solvedAlpha0, solvedAlpha1);
					endpoint0.w = solvedAlpha0.x;
					endpoint1.w = solvedAlpha1.x;
				#else
					SolveBC7LeastSquares(colorLeastSquares, endpoint0, endpoint1);
				#endif
				}
			}

			gCandidateErrors[threadIndex] = candidateError;

			GroupMemoryBarrierWithGroupSync();

			// The block's lowest-error candidate wins, with ties going to the lower candidate
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

			uint4 block = uint4(0, 0, 0, 0);
			uint position = 0u;
			PutBits(block, position, MODE_PREFIX, MODE_PREFIX_BITS);

		#if SEPARATE_ALPHA
			// Anchor fix-up: texel 0 stores each of its indices without the top bit, so a line whose texel 0 index has the
			// top bit set swaps its endpoints and mirrors its indices
			const float4 firstTexel = RotateBC7Texel(gGroupTexels[texelBase], rotation);
			uint firstColorIndex;
			uint firstAlphaIndex;
			FindBC7Index(firstTexel * colorMask, GetBC7PaletteBase(color), GetBC7PaletteDelta(color), colorIndexBits, firstColorIndex);
			FindBC7Index(firstTexel.wwww * alphaMask, GetBC7PaletteBase(alpha), GetBC7PaletteDelta(alpha), alphaIndexBits, firstAlphaIndex);

			const uint highestColorIndex = (1u << colorIndexBits) - 1u;
			const uint highestAlphaIndex = (1u << alphaIndexBits) - 1u;
			const bool swapsColor = firstColorIndex > highestColorIndex / 2u;
			const bool swapsAlpha = firstAlphaIndex > highestAlphaIndex / 2u;
			const uint colorCodes0 = swapsColor ? color.Codes1 : color.Codes0;
			const uint colorCodes1 = swapsColor ? color.Codes0 : color.Codes1;
			const uint alphaCodes0 = swapsAlpha ? alpha.Codes1 : alpha.Codes0;
			const uint alphaCodes1 = swapsAlpha ? alpha.Codes0 : alpha.Codes1;

			PutBits(block, position, rotation, 2u);
			if (BC7_MODE == 4)
				PutBits(block, position, selector, 1u);

			for (uint channel = 0u; channel < 3u; ++channel)
			{
				const uint shift = channel * COLOR_ENDPOINT_BITS;
				PutBits(block, position, colorCodes0 >> shift, COLOR_ENDPOINT_BITS);
				PutBits(block, position, colorCodes1 >> shift, COLOR_ENDPOINT_BITS);
			}

			PutBits(block, position, alphaCodes0, ALPHA_ENDPOINT_BITS);
			PutBits(block, position, alphaCodes1, ALPHA_ENDPOINT_BITS);

			// The 2-bit index set comes first: color, unless mode 4's selector gave color the 3-bit set
			for (uint indexSet = 0u; indexSet < 2u; ++indexSet)
			{
				const bool isColorSet = (indexSet == 0u) == (selector == 0u);
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float4 texel = RotateBC7Texel(gGroupTexels[texelBase + texelIndex], rotation);

					uint index;
					if (isColorSet)
					{
						FindBC7Index(texel * colorMask, GetBC7PaletteBase(color), GetBC7PaletteDelta(color), colorIndexBits, index);
						index = swapsColor ? highestColorIndex - index : index;
					}
					else
					{
						FindBC7Index(texel.wwww * alphaMask, GetBC7PaletteBase(alpha), GetBC7PaletteDelta(alpha), alphaIndexBits, index);
						index = swapsAlpha ? highestAlphaIndex - index : index;
					}

					const uint indexBits = isColorSet ? colorIndexBits : alphaIndexBits;
					PutBits(block, position, index, (texelIndex == 0u) ? indexBits - 1u : indexBits);
				}
			}
		#else
			uint firstIndex;
			FindBC7Index(gGroupTexels[texelBase], GetBC7PaletteBase(color), GetBC7PaletteDelta(color), INDEX_BITS, firstIndex);

			const uint highestIndex = (1u << INDEX_BITS) - 1u;
			const bool swaps = firstIndex > highestIndex / 2u;
			const uint codes0 = swaps ? color.Codes1 : color.Codes0;
			const uint codes1 = swaps ? color.Codes0 : color.Codes1;

			for (uint channel = 0u; channel < 4u; ++channel)
			{
				const uint shift = channel * ENDPOINT_BITS;
				PutBits(block, position, codes0 >> shift, ENDPOINT_BITS);
				PutBits(block, position, codes1 >> shift, ENDPOINT_BITS);
			}

			PutBits(block, position, swaps ? color.Pbit1 : color.Pbit0, 1u);
			PutBits(block, position, swaps ? color.Pbit0 : color.Pbit1, 1u);

			for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
			{
				uint index;
				FindBC7Index(gGroupTexels[texelBase + texelIndex], GetBC7PaletteBase(color), GetBC7PaletteDelta(color), INDEX_BITS, index);
				if (swaps)
					index = highestIndex - index;

				PutBits(block, position, index, (texelIndex == 0u) ? INDEX_BITS - 1u : INDEX_BITS);
			}
		#endif

			gOutput[blockId] = block;
			gBestErr[blockId] = winnerError;
		}
	};
};
