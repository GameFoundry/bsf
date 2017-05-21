#include "$EDITOR$/PickingCull.bslinc"

technique PickingCullNone
{
	mixin PickingCull;

	raster
	{
		cull = none;
	};
};