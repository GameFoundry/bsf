#include "$EDITOR$/SolidGizmo.bslinc"

options
{
	priority = 10;
}

technique SolidGizmo
{
	mixin SolidGizmoBase;

	depth
	{
		write = false;
		read = false;
	};
	
	stencil
	{
		enabled = true;
		front = { keep, keep, inc, pass };
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