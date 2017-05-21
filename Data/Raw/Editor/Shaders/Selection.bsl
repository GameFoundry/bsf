#include "$EDITOR$\SelectionBase.bslinc"

#define USE_SKELETON
#include "$EDITOR$\SelectionBase.bslinc"
#undef USE_SKELETON

#define USE_BLEND_SHAPES
#include "$EDITOR$\SelectionBase.bslinc"

#define USE_SKELETON
#include "$EDITOR$\SelectionBase.bslinc"

technique Selection1
{ 
	mixin Selection;
};

technique Selection2
{
	mixin SelectionSkinned;

	Tags = { "Skinned" };
};

technique Selection3
{
	mixin SelectionMorph;

	Tags = { "Morph" };
};

technique Selection4
{
	mixin SelectionSkinnedMorph;

	Tags = { "SkinnedMorph" };
};