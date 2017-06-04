#include "$ENGINE$\ShadowDepthBase.bslinc"

mixin ShadowDepth
{ 
	code
	{
		float4 fsmain(ShadowVStoFS input, out float outDepth : SV_Depth) : SV_Target0
		{
			outDepth = saturate(-input.shadowPos * gInvDepthRange + gDepthBias);
			return 0;
		}
	};
};