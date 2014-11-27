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
		:Component(parent), mLastUpdateHash(0)
	{
		setName("Renderable");

		mInternal = RenderableHandler::create();
	}

	Bounds Renderable::getBounds() const
	{
		updateTransform();

		return mInternal->getBounds();
	}

	void Renderable::updateTransform() const
	{
		UINT32 curHash = SO()->getTransformHash();
		if (curHash != mLastUpdateHash)
		{
			mInternal->setTransform(SO()->getWorldTfrm());

			mLastUpdateHash = curHash;
		}
	}

	void Renderable::update()
	{
		updateTransform();

		if (SO()->getActive() != mInternal->getIsActive())
		{
			mInternal->setIsActive(SO()->getActive());
		}
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