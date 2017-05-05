#include "$ENGINE$\SpriteImage.bslinc"

Technique : inherits("SpriteImage") =
{
	Pass =
	{
		Target = 
		{
			Blend = true;
			Color = { SRCA, SRCIA, ADD };
			WriteMask = RGB;
		};
	};
};