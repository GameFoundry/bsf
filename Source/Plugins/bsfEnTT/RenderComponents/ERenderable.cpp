#include "./ERenderable.h"

namespace bs::ecs {

Bounds ERenderable::getBounds() const
{
	if(mUseOverrideBounds)
	{
		Sphere sphere(mOverrideBounds.getCenter(), mOverrideBounds.getRadius());

		Bounds bounds(mOverrideBounds, sphere);
		bounds.transformAffine(mTfrmMatrix);

		return bounds;
	}

	HMesh mesh = getMesh();

	if (!mMesh.isLoaded())
	{

		Bounds bounds;
		bounds.transformAffine(mTfrmMatrix);
		return bounds;
	}
	else
	{
		Bounds bounds = mMesh->getProperties().getBounds();
		bounds.transformAffine(mTfrmMatrix);

		return bounds;
	}
}

} // namespace bs::ecs