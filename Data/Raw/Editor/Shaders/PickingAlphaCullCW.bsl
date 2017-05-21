#include "$EDITOR$/PickingAlphaCull.bslinc"

technique PickingAlphaCullCW
{
	mixin PickingAlphaCull;

	raster
	{
		cull = cw;
	};
};