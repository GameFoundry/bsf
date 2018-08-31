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
		
		Texture3D gVectorFieldTex;
		[alias(gVectorFieldTex)]
		SamplerState gVectorFieldSampler
		{
			AddressU = CLAMP;
			AddressV = CLAMP;
			AddressW = CLAMP;
		};
		
		cbuffer VectorFieldParams
		{
			float3 gFieldBounds;
			float gFieldIntensity;
			float3 gFieldTiling;
			float gFieldTightness;
			float4x4 gWorldToField;
			float3x3 gFieldToWorld;
		};
		
		cbuffer Params
		{
			uint gNumVectorFields;
			uint gNumIterations;
			float gDT;
		};
		
		float3 evaluateVectorField(float3 pos, float scale, out float3 velocity, out float tightness)
		{
			if(gNumVectorFields == 0)
			{
				velocity = 0.0f;
				tightness = 0.0f;
				return 0.0f;
			}
		
			float3 uv = mul(gWorldToField, float4(pos, 1.0f)).xyz;
			uv -= floor(uv * gFieldTiling);
			
			float3 smp = gVectorFieldTex.Sample(gVectorFieldSampler, uv);
			smp = mul(gFieldToWorld, smp);
			
			velocity = gFieldIntensity * gFieldTightness * smp;
			tightness = gFieldTightness;
			return gFieldIntensity * smp;
		}
	
		void fsmain(VStoFS input, 
			out float4 outPosAndTime : SV_Target0, 
			out float4 outVelocityAndTimeScale : SV_Target1)
		{
			float4 posAndTime = gPosAndTimeTex.Sample(gPosAndTimeSampler, input.uv0);
			float4 velocityAndTimeScale = gVelocityTex.Sample(gVelocitySampler, input.uv0);
			
			float3 position = posAndTime.xyz;
			float time = posAndTime.w;
			float3 velocity = velocityAndTimeScale.xyz;
			float timeScale = velocityAndTimeScale.w;
			
			for(uint i = 0; i < gNumIterations; i++)
			{
				time += gDT * timeScale;
			
				// Evaluate vector field
				float3 fieldVelocity;
				float fieldTightness;
				float3 totalForce = evaluateVectorField(posAndTime.xyz, 1.0f, fieldVelocity, fieldTightness);
				
				velocity = lerp(velocity, fieldVelocity, fieldTightness);
				
				// Integrate
				float3 acceleration = totalForce * gDT;
				position += (velocity + acceleration * 0.5f) * gDT;
				velocity += acceleration;
			}

			outPosAndTime.xyz = position;
			outPosAndTime.w = time;
			outVelocityAndTimeScale.xyz = velocity;
			outVelocityAndTimeScale.w = timeScale;			
		}	
	};
};