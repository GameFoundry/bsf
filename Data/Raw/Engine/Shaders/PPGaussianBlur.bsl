#include "$ENGINE$\PPBase.bslinc"

technique PPGaussianBlur
{
	mixin PPBase;

	code
	{
		[internal]
		cbuffer Input
		{
			float2 gSampleOffsets[MAX_NUM_SAMPLES];
			float gSampleWeights[MAX_NUM_SAMPLES];
			uint gNumSamples;
		}		

		SamplerState gInputSamp;
		Texture2D gInputTex;
		
		float4 fsmain(VStoFS input) : SV_Target0
		{
			// Note: Consider adding a version of this shader with unrolled loop for small number of samples
			float4 output = 0;
			
			uint sampleIdx = 0;
			for(sampleIdx = 0; sampleIdx < (gNumSamples - 1); sampleIdx += 2)
			{
				{
					float2 uv = input.uv0 + gSampleOffsets[sampleIdx];
					output += gInputTex.Sample(gInputSamp, uv) * gSampleWeights[sampleIdx + 0];
				}
				
				{
					float2 uv = input.uv0 + gSampleOffsets[sampleIdx];
					output += gInputTex.Sample(gInputSamp, uv) * gSampleWeights[sampleIdx + 1];
				}
			}
			
			[branch]
			if(sampleIdx < gNumSamples)
			{
				float2 uv = input.uv0 + gSampleOffsets[sampleIdx / 2].xy;
				output += gInputTex.Sample(gInputSamp, uv) * gSampleWeights[sampleIdx + 0];
			}
			
			return output;
		}	
	};
};