#include "$EDITOR$/PickingCull.bslinc"

technique PickingCullCCW
{
	mixin PickingCull;

	raster
	{
		cull = ccw;
	};
};