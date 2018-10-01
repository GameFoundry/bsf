shader ClearLoadStore
{
	featureset = HighEnd;

	variations
	{
		TEX_ARRAY = { false, true };
	};
	
	code
	{
		#if TEX_ARRAY
			RWTexture2DArray<float> gOutput;
		#else
			RWTexture2D<float> gOutput;
		#endif
	
		[internal]
		cbuffer Params : register(b0)
		{
			uint2 gSize;
		}

		[numthreads(NUM_THREADS, 1, 1)]
		void csmain(uint3 groupThreadId : SV_GroupThreadID, uint3 groupId : SV_GroupID)
		{
			uint2 tilePos;
			tilePos.x = groupId.x * NUM_THREADS + groupThreadId.x;
			tilePos.y = groupId.y * NUM_THREADS + groupThreadId.y;
			
			if(tilePos.x >= gSize.x || tilePos.y >= gSize.y)
				return;
				
			for (uint y = 0; y < TILE_SIZE; ++y)
			{
				for (uint x = 0; x < TILE_SIZE; ++x)
				{
					uint2 pixelPos = tilePos + uint2(x, y);
					if(pixelPos.x >= gSize.x || pixelPos.y >= gSize.y)
						continue;

					#if TEX_ARRAY
						gOutput[uint3(pixelPos, 0)] = 0.0f;
					#else
						gOutput[pixelPos] = 0.0f;
					#endif
				}
			}
		}
	};
};