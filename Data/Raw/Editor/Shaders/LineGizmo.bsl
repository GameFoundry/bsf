#include "$EDITOR$/LineGizmo.bslinc"

technique LineGizmo
{
	mixin LineGizmoBase;

	raster
	{
		multisample = false; // This controls line rendering algorithm
		lineaa = true;
	};
	
	blend
	{
		target
		{
			enabled = true;
			color = { srcA, srcIA, add };
		};
	};
};
