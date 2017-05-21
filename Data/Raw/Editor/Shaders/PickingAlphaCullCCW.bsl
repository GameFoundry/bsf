#include "$EDITOR$/PickingAlphaCull.bslinc"

technique PickingAlphaCullCCW
{
	mixin PickingAlphaCull;

	raster
	{
		cull = ccw;
	};
};