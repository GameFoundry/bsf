#include "$EDITOR$/PickingAlphaCull.bslinc"

technique PickingAlphaCullNone
{
	mixin PickingAlphaCull;

	raster
	{
		cull = none;
	};
};