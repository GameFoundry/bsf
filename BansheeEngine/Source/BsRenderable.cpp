#include "BsRenderable.h"
#include "BsRenderableRTTI.h"
#include "BsSceneObject.h"
#include "BsBuiltinResources.h"
#include "BsMesh.h"
#include "BsMaterial.h"
#include "BsRenderQueue.h"
#include "BsBounds.h"

namespace BansheeEngine
{
	Renderable::Renderable(const HSceneObject& parent)
		:Component(parent)
	{
		setName("Renderable");

		mInternal = bs_shared_ptr<RenderableHandler>();
	}

	Bounds Renderable::getBounds() const
	{
		HMesh mesh = mInternal->getMesh();

		if (mesh == nullptr || !mesh.isLoaded())
		{
			Vector3 pos = SO()->getWorldPosition();

			AABox box(pos, pos);
			Sphere sphere(pos, 0.0f);

			return Bounds(box, sphere);
		}
		else
		{
			Bounds bounds = mesh->getBounds();
			bounds.transformAffine(SO()->getWorldTfrm());

			return bounds;
		}
	}

	RenderableProxyPtr Renderable::_createProxy() const 
	{ 
		return mInternal->_createProxy(SO()->getWorldTfrm()); 
	}

	RTTITypeBase* Renderable::getRTTIStatic()
	{
		return RenderableRTTI::instance();
	}

	RTTITypeBase* Renderable::getRTTI() const
	{
		return Renderable::getRTTIStatic();
	}
}