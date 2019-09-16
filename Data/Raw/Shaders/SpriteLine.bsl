#define TRANSPARENCY 1
#include "$ENGINE$\SpriteCommon.bslinc"

shader SpriteLine
{
	mixin SpriteCommon;
	
	variations
	{
		ALPHA = { false, true };
	};

	raster
	{
		multisample = false; // This controls line rendering algorithm
		lineaa = false;
	};
	
	code
	{
		float4 fsmain() : SV_Target
		{
			return gTint;
		}
	};
};
