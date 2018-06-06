#include "$ENGINE$\ParticleVertex.bslinc"

options
{
	transparent = true;
};

shader Surface
{
	mixin ParticleVertex;

	blend
	{
		target	
		{
			enabled = true;
			color = { srcA, srcIA, add };
		};
	};	
	
	depth
	{
		write = false;
	};

	code
	{
		float4 fsmain(in VStoFS input) : SV_Target0
		{
			return float4(1.0f, 1.0f, 0.0f, 1.0f);
		}	
	};
};