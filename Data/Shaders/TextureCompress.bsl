#include "$ENGINE$\TextureCompression\TextureCompressBC4.bslinc"

shader TextureCompress
{
	featureset = HighEnd;

	mixin TextureCompressBC4;

	variations
	{
		// Target block-compressed format (BC1 and BC3 are TextureCompressBC1.bsl's FORMAT 0 and 1):
		//
		//   2 - BC4 (single red channel, 64-bit block)
		//   3 - BC5 (RG: two BC4 blocks, 128-bit)
		FORMAT = { 2, 3 };
	};

	code
	{
		// BC4 outputs a 64-bit block (uint2) and BC5 a 128-bit block (uint4)
		#if FORMAT == 2
			#define BLOCK_TYPE uint2
		#else
			#define BLOCK_TYPE uint4
		#endif

		// Source pixels as a 2D texture: RGBA8 read as normalized float4
		Texture2D gInput;

		// One packed block per texel, at the block-grid coordinate: a uint2 (64-bit formats) or uint4 (128-bit).
		RWTexture2D<BLOCK_TYPE> gOutput;

		[internal]
		cbuffer Parameters
		{
			int2 gTextureSize; // texture size in pixels (whole texture)
			int2 gBlockCount;  // number of 4x4 blocks along each axis (whole texture)
			int2 gBlockOffset; // block-grid offset of the dispatched chunk; added to the local block id (both X and Y)
		}

		[numthreads(8, 8, 1)]
		void csmain(uint3 dispatchId : SV_DispatchThreadID)
		{
			int2 texSize = gTextureSize;
			uint2 numBlocks = (uint2)gBlockCount;

			// Textures are compressed in chunks, each dispatched with a grid starting at (0,0), so shift it to the chunk's
			// real block coordinates. The shared input/output textures are addressed with this global block id.
			uint2 blockId = dispatchId.xy + (uint2)gBlockOffset;
			if (blockId.x >= numBlocks.x || blockId.y >= numBlocks.y)
				return;

			uint2 base = blockId * 4;
			uint2 maxCoord = (uint2)texSize - 1;

			float red[16];
			float green[16];

			[unroll]
			for (uint y = 0; y < 4; ++y)
			{
				[unroll]
				for (uint x = 0; x < 4; ++x)
				{
					uint2 coord = min(base + uint2(x, y), maxCoord);

					float4 texel = gInput.Load(int3((int2)coord, 0));

					uint idx = y * 4 + x;
					red[idx] = texel.r;
					green[idx] = texel.g;
				}
			}

			#if FORMAT == 2 // BC4
				gOutput[blockId] = CompressBC4(red);
			#else // BC5 = BC4(red) + BC4(green)
				uint2 redBlock = CompressBC4(red);
				uint2 greenBlock = CompressBC4(green);
				gOutput[blockId] = uint4(redBlock, greenBlock);
			#endif
		}
	};
};
