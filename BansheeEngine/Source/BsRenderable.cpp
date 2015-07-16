#include "BsRenderable.h"
#include "BsRenderableRTTI.h"
#include "BsSceneObject.h"
#include "BsBuiltinResources.h"
#include "BsMesh.h"
#include "BsMaterial.h"
#include "BsRenderQueue.h"
#include "BsBounds.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	Renderable::Renderable(const HSceneObject& parent)
		:Component(parent)
	{
		setName("Renderable");
		mInternal = RenderableHandler::create();
	}

	void Renderable::onInitialized()
	{
		gSceneManager()._registerRenderable(mInternal, sceneObject());
	}

	Bounds Renderable::getBounds() const
	{
		updateTransform();

		return mInternal->getBounds();
	}

	void Renderable::updateTransform() const
	{
		UINT32 curHash = SO()->getTransformHash();
		if (curHash != mInternal->_getLastModifiedHash())
		{
			mInternal->setTransform(SO()->getWorldTfrm());
			mInternal->_setLastModifiedHash(curHash);
		}
	}

	void Renderable::update()
	{

	}

	void Renderable::onDestroyed()
	{
		gSceneManager()._unregisterRenderable(mInternal);
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