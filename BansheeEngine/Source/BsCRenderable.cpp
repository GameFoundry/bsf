#include "BsCRenderable.h"
#include "BsCRenderableRTTI.h"
#include "BsSceneObject.h"
#include "BsBuiltinResources.h"
#include "BsMesh.h"
#include "BsMaterial.h"
#include "BsRenderQueue.h"
#include "BsBounds.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	CRenderable::CRenderable(const HSceneObject& parent)
		:Component(parent)
	{
		setName("Renderable");
	}

	void CRenderable::onInitialized()
	{
		// If mInternal already exists this means this object was deserialized,
		// so all we need to do is initialize it.
		if (mInternal != nullptr)
			mInternal->initialize();
		else
			mInternal = Renderable::create();

		gSceneManager()._registerRenderable(mInternal, sceneObject());
	}

	Bounds CRenderable::getBounds() const
	{
		updateTransform();

		return mInternal->getBounds();
	}

	void CRenderable::updateTransform() const
	{
		UINT32 curHash = SO()->getTransformHash();
		if (curHash != mInternal->_getLastModifiedHash())
		{
			mInternal->setTransform(SO()->getWorldTfrm());
			mInternal->_setLastModifiedHash(curHash);
		}
	}

	void CRenderable::update()
	{

	}

	void CRenderable::onDestroyed()
	{
		gSceneManager()._unregisterRenderable(mInternal);
	}

	RTTITypeBase* CRenderable::getRTTIStatic()
	{
		return CRenderableRTTI::instance();
	}

	RTTITypeBase* CRenderable::getRTTI() const
	{
		return CRenderable::getRTTIStatic();
	}
}