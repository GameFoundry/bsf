#define UV 1
#define TRANSPARENCY 1
#include "$ENGINE$\SpriteCommon.bslinc"

shader SpriteText
{
	mixin SpriteCommon;

	variations
	{
		ALPHA = { false, true };
	};
	
	code
	{
		[alias(gMainTexture)]
		SamplerState gMainTexSamp;
		Texture2D gMainTexture;

		float4 fsmain(in float4 inPos : SV_Position, float2 uv : TEXCOORD0) : SV_Target
		{
			#if ALPHA
			return gMainTexture.Sample(gMainTexSamp, uv).r * gTint.a;
			#else
			return float4(gTint.rgb, gMainTexture.Sample(gMainTexSamp, uv).r * gTint.a);
			#endif
		}
	};
};
