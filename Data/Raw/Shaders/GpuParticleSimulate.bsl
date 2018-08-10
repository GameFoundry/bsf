#include "$ENGINE$/GpuParticleTileVertex.bslinc"

shader GpuParticleSimulate
{
	mixin GpuParticleTileVertex;
	
	code
	{
		Texture2D gPosAndTimeTex;
		[alias(gPosAndTimeTex)]
		SamplerState gPosAndTimeSampler
		{
            Filter = MIN_MAG_MIP_POINT;
        };

		Texture2D gVelocityTex;
		[alias(gVelocityTex)]
		SamplerState gVelocitySampler
		{
            Filter = MIN_MAG_MIP_POINT;
        };
	
		void fsmain(VStoFS input, 
			out float4 outPosAndTime : SV_Target0, 
			out float4 outVelocity : SV_Target1)
		{
			float4 posAndTime = gPosAndTimeTex.Sample(gPosAndTimeSampler, input.uv0);
			float4 velocity = gVelocityTex.Sample(gVelocitySampler, input.uv0);
			
			// TODO - Simulate
			
			outPosAndTime = posAndTime;
			outVelocity = velocity;	
		}	
	};
};