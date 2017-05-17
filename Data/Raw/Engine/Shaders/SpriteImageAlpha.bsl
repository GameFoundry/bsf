#include "$ENGINE$\SpriteImage.bslinc"

technique SpriteImageAlpha
{
	mixin SpriteImage;

	blend
	{
		target	
		{
			enabled = true;
			color = { srcA, srcIA, add };
			writemask = RGB;
		};
	};
};