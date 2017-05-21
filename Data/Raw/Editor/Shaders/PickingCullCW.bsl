#include "$EDITOR$/PickingCull.bslinc"

technique PickingCullCW
{
	mixin PickingCull;
	
	raster
	{
		cull = cw;
	};
};