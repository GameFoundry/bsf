#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode11.bslinc"
#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode12.bslinc"
#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode13.bslinc"
#include "$ENGINE$\TextureCompression\TextureCompressBC6Mode14.bslinc"

shader TextureCompressBC6HOneRegion
{
	featureset = HighEnd;

	mixin TextureCompressBC6Mode11;
	mixin TextureCompressBC6Mode12;
	mixin TextureCompressBC6Mode13;
	mixin TextureCompressBC6Mode14;

	variations
	{
		BC6_MODE = { 11, 12, 13, 14 };
	};

	code
	{
		// The one-region modes differ in the precision of their endpoints. Mode 11 stores both endpoints as BASE_BITS
		// codes; the others store the second one as a DELTA_BITS signed delta from the first.
		#if BC6_MODE == 11
			#define BASE_BITS 10
			#define DELTA_BITS int3(10, 10, 10)
		#elif BC6_MODE == 12
			#define BASE_BITS 11
			#define DELTA_BITS int3(9, 9, 9)
		#elif BC6_MODE == 13
			#define BASE_BITS 12
			#define DELTA_BITS int3(8, 8, 8)
		#else
			#define BASE_BITS 16
			#define DELTA_BITS int3(4, 4, 4)
		#endif

		#define TRANSFORMED (BC6_MODE != 11)
		#define INDEX_BITS 4u

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
			int4 gStageParameters;
		}

		#define BLOCKS_PER_GROUP 64u

		// Texels per block in groupshared memory, as half-float bits packed red | green << 16 and blue. One more than a
		// block has, so that threads reading the same texel of different blocks hit different banks.
		#define TEXEL_STRIDE 17

		groupshared uint2 gGroupTexels[BLOCKS_PER_GROUP * TEXEL_STRIDE];

		float4 LoadGroupTexel(uint index)
		{
			const uint2 packedTexel = gGroupTexels[index];
			return float4((float)(packedTexel.x & 0xFFFFu), (float)(packedTexel.x >> 16u), (float)packedTexel.y, 0.0f);
		}

		// Each group encodes 64 consecutive blocks of one row, with one thread per block. Texels are half-float bits. The
		// endpoints are fitted, quantized and refined against the palette the decoder reconstructs, with the second endpoint
		// stored as a clamped delta. The block replaces the block's current encoding if it has a lower error.
		[numthreads(64, 1, 1)]
		void csmain(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID)
		{
			const uint threadIndex = groupThreadId.x;
			const uint firstBlockColumn = groupId.x * BLOCKS_PER_GROUP;
			const uint lastBlockColumn = (uint)gScratchBlockCount.x - 1u;
			const uint2 maxCoordinate = (uint2)gTextureSize - 1u;

			// Groups extending past the end of the row repeat its last block, and never write it
			for (uint loadIndex = threadIndex; loadIndex < BLOCKS_PER_GROUP * 16u; loadIndex += 64u)
			{
				const uint2 loadBlockId = uint2(min(firstBlockColumn + loadIndex / 16u, lastBlockColumn), groupId.y) + (uint2)gBlockOffset;
				const uint texelIndex = loadIndex % 16u;
				const uint2 coordinate = min(loadBlockId * 4u + uint2(texelIndex & 3u, texelIndex >> 2u), maxCoordinate);
				const uint4 texelBits = (uint4)GetBC6HalfBits(gInput.Load(int3((int2)coordinate, 0)));
				gGroupTexels[(loadIndex / 16u) * TEXEL_STRIDE + texelIndex] = uint2(texelBits.x | (texelBits.y << 16u), texelBits.z);
			}

			GroupMemoryBarrierWithGroupSync();

			if (firstBlockColumn + threadIndex > lastBlockColumn)
				return;

			const uint2 blockId = uint2(firstBlockColumn + threadIndex, groupId.y) + (uint2)gBlockOffset;
			const uint texelBase = threadIndex * TEXEL_STRIDE;

			// Initial endpoints: the block's principal axis, spanning the extreme projections of its texels. Endpoint 0 is
			// the high end of the axis. Moments are accumulated relative to the block mean to keep their precision for values
			// up to 0x7BFF. The block's bounds limit the least-squares solves below.
			float4 endpoint0;
			float4 endpoint1;
			float4 minimum = float4(1e30f, 1e30f, 1e30f, 0.0f);
			float4 maximum = float4(-1e30f, -1e30f, -1e30f, 0.0f);
			{
				float4 blockMean = float4(0, 0, 0, 0);
				for (uint meanTexelIndex = 0u; meanTexelIndex < 16u; ++meanTexelIndex)
				{
					const float4 texel = LoadGroupTexel(texelBase + meanTexelIndex);
					blockMean += texel;
					minimum = min(minimum, texel);
					maximum = max(maximum, texel);
				}

				blockMean *= 1.0f / 16.0f;

				BC7Moments blockMoments = CreateBC7Moments();
				float4 blockSum = float4(0, 0, 0, 0);
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float4 centeredTexel = LoadGroupTexel(texelBase + texelIndex) - blockMean;
					blockSum += centeredTexel;
					AccumulateBC7Moments(blockMoments, centeredTexel, 1.0f);
				}

				const float4 axis = FindBC7PrincipalAxis(CenterBC7Moments(blockMoments, blockSum, 16.0f));
				float2 range = float2(1e30f, -1e30f);
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float projection = dot(LoadGroupTexel(texelBase + texelIndex) - blockMean, axis);
					range = float2(min(range.x, projection), max(range.y, projection));
				}

				endpoint0 = blockMean + axis * range.y;
				endpoint1 = blockMean + axis * range.x;
			}

			// Each pass quantizes the endpoints and picks the best index for every texel from the palette the decoder
			// reconstructs, with the second endpoint stored as a clamped delta from the first. Texel 0 stores its index
			// without the top bit, so if it picks the upper half of the ramp the endpoints swap, and the palette is rebuilt
			// from the swapped codes (clamping may make it differ from the mirrored one). On all but the last pass, the
			// endpoints are then solved for the indices by least squares. The last pass packs the indices.
			uint4 block = uint4(0, 0, 0, 0);
			int3 storedCode0 = int3(0, 0, 0);
			int3 storedCode1 = int3(0, 0, 0);
			float blockError = 0.0f;
			for (uint refinePass = 0u; refinePass < BCN_REFINE_PASSES; ++refinePass)
			{
				const bool isLastPass = refinePass + 1u == BCN_REFINE_PASSES;
				int3 code0 = QuantizeVecBC6(endpoint0, BASE_BITS);
				int3 code1 = QuantizeVecBC6(endpoint1, BASE_BITS);
				int3 unquantized0 = UnquantizeBC6Vector(code0, BASE_BITS);
				int3 unquantized1 = UnquantizeBC6Vector(TransformBC6Endpoint(code1, code0, BASE_BITS, DELTA_BITS, TRANSFORMED, storedCode1), BASE_BITS);

				const int3 anchorTexel = (int3)LoadGroupTexel(texelBase).xyz;
				uint anchorIndex;
				FindBC6Index(anchorTexel, unquantized0, unquantized1, INDEX_BITS, anchorIndex);

				const bool swapsEndpoints = anchorIndex >= 8u;
				if (swapsEndpoints)
				{
					const int3 swappedCode = code0;
					code0 = code1;
					code1 = swappedCode;
					unquantized0 = UnquantizeBC6Vector(code0, BASE_BITS);
					unquantized1 = UnquantizeBC6Vector(TransformBC6Endpoint(code1, code0, BASE_BITS, DELTA_BITS, TRANSFORMED, storedCode1), BASE_BITS);
				}

				storedCode0 = code0;

				BC7LeastSquares leastSquares = CreateBC7LeastSquares();
				uint position = 65u;
				blockError = 0.0f;
				for (uint texelIndex = 0u; texelIndex < 16u; ++texelIndex)
				{
					const float4 texel = LoadGroupTexel(texelBase + texelIndex);
					const int3 texelBits = (int3)texel.xyz;

					uint index;
					float distance = FindBC6Index(texelBits, unquantized0, unquantized1, INDEX_BITS, index);

					// The rebuilt palette can still put texel 0 in the upper half. It then takes the closest entry of the
					// lower half instead.
					if (texelIndex == 0u && index >= 8u)
					{
						index = 7u;
						distance = GetBC6PaletteDistance(texelBits, unquantized0, unquantized1, index, INDEX_BITS);
					}

					blockError += distance;

					// Least squares solves for endpoint0 and endpoint1, which are the second and first palette endpoint
					// after a swap
					const float weight = (float)GetBC7IndexWeight(index, INDEX_BITS) * (1.0f / 64.0f);
					AccumulateBC7LeastSquares(leastSquares, texel, swapsEndpoints ? 1.0f - weight : weight, 1.0f);

					if (isLastPass)
						PutBits(block, position, index, texelIndex == 0u ? INDEX_BITS - 1u : INDEX_BITS);
				}

				if (!isLastPass)
					SolveBC6LeastSquares(leastSquares, minimum, maximum, endpoint0, endpoint1);
			}

			const float previousError = gBestErr[blockId];
			if (!(blockError < previousError))
				return;

			#if BC6_MODE == 11
				block |= PackBC6Mode11Endpoints(storedCode0, storedCode1);
			#elif BC6_MODE == 12
				block |= PackBC6Mode12Endpoints(storedCode0, storedCode1);
			#elif BC6_MODE == 13
				block |= PackBC6Mode13Endpoints(storedCode0, storedCode1);
			#else
				block |= PackBC6Mode14Endpoints(storedCode0, storedCode1);
			#endif

			gOutput[blockId] = block;
			gBestErr[blockId] = blockError;
		}
	};
};
