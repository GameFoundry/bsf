#pragma once

#include "BsPrerequisites.h"
#include "BsRenderableHandler.h"
#include "BsComponent.h"

namespace BansheeEngine
{
	/**
	 * @copydoc	RenderableHandler
	 */
	class BS_EXPORT Renderable : public Component
	{
	public:
		/**
		 * @copydoc	RenderableHandler::setMesh
		 */
		void setMesh(HMesh mesh) { mInternal->setMesh(mesh); }

		/**
		 * @copydoc	RenderableHandler::setMaterial
		 */
		void setMaterial(UINT32 idx, HMaterial material) { mInternal->setMaterial(idx, material); }

		/**
		 * @copydoc	RenderableHandler::setMaterial
		 */
		void setMaterial(HMaterial material) { mInternal->setMaterial(material); }

		/**
		 * @copydoc	RenderableHandler::setLayer
		 */
		void setLayer(UINT64 layer) { mInternal->setLayer(layer); }

		/**
		 * @copydoc	RenderableHandler::getLayer
		 */
		UINT64 getLayer() const { return mInternal->getLayer(); }

		/**
		 * @copydoc	RenderableHandler::getMesh
		 */
		HMesh getMesh() const { return mInternal->getMesh(); }

		/**
		 * @copydoc	RenderableHandler::getMaterial
		 */
		HMaterial getMaterial(UINT32 idx) const { return mInternal->getMaterial(idx); }

		/**
		 * @brief	Gets world bounds of the mesh rendered by this object.
		 */
		Bounds getBounds() const;

		/**
		 * @brief	Returns the internal renderable handler that is used for
		 *			majority of operations by this component.
		 */
		RenderableHandlerPtr _getHandler() const { return mInternal; }

		/************************************************************************/
		/* 								CORE PROXY                      		*/
		/************************************************************************/

		/**
		 * @copydoc	RenderableHandler::_isCoreDirty
		 */
		bool _isCoreDirty() const { return mInternal->_isCoreDirty(); }

		/**
		 * @copydoc	RenderableHandler::_markCoreClean
		 */
		void _markCoreClean() { mInternal->_markCoreClean(); }

		/**
		 * @copydoc	RenderableHandler::_markCoreDirty
		 */
		void _markCoreDirty() const { mInternal->_markCoreDirty(); }

		/**
		 * @copydoc	RenderableHandler::_createProxy
		 */
		RenderableProxyPtr _createProxy() const;

		/**
		 * @copydoc	RenderableHandler::_getActiveProxy
		 */
		RenderableProxyPtr _getActiveProxy() const { return mInternal->_getActiveProxy();; }

		/**
		 * @copydoc	RenderableHandler::_setActiveProxy
		 */
		void _setActiveProxy(const RenderableProxyPtr& proxy) { mInternal->_setActiveProxy(proxy); }

	private:
		RenderableHandlerPtr mInternal;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class SceneObject;

		Renderable(const HSceneObject& parent);

	public:
		/**
		 * @copydoc	Component::update
		 */
		virtual void update() {}

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class RenderableRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	protected:
		Renderable() {} // Serialization only
	};
}