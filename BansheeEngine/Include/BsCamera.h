#pragma once

#include "BsPrerequisites.h"
#include "BsCameraHandler.h"
#include "BsComponent.h"
#include "BsViewport.h"

namespace BansheeEngine 
{
	/**
	 * @copydoc	CameraHandler
	 */
    class BS_EXPORT Camera : public Component
    {
    public:
		Camera(const HSceneObject& parent, RenderTargetPtr target = nullptr,
			float left = 0.0f, float top = 0.0f, float width = 1.0f, float height = 1.0f);

        virtual ~Camera();

		/**
		 * @copydoc	CameraHandler::getViewport
		 */
		ViewportPtr getViewport() const { return mInternal->getViewport(); }

		/**
		 * @copydoc	CameraHandler::setHorzFOV
		 */
		virtual void setHorzFOV(const Radian& fovy) { mInternal->setHorzFOV(fovy); }

		/**
		 * @copydoc	CameraHandler::getHorzFOV
		 */
		virtual const Radian& getHorzFOV() const { return mInternal->getHorzFOV(); }

		/**
		 * @copydoc	CameraHandler::setNearClipDistance
		 */
		virtual void setNearClipDistance(float nearDist) { mInternal->setNearClipDistance(nearDist); }

		/**
		 * @copydoc	CameraHandler::getNearClipDistance
		 */
		virtual float getNearClipDistance() const { return mInternal->getNearClipDistance(); }

		/**
		 * @copydoc	CameraHandler::setFarClipDistance
		 */
		virtual void setFarClipDistance(float farDist) { mInternal->setFarClipDistance(farDist); }

		/**
		 * @copydoc	CameraHandler::getFarClipDistance
		 */
		virtual float getFarClipDistance() const { return mInternal->getFarClipDistance(); }

		/**
		 * @copydoc	CameraHandler::setAspectRatio
		 */
		virtual void setAspectRatio(float ratio) { mInternal->setAspectRatio(ratio); }

		/**
		 * @copydoc	CameraHandler::getAspectRatio
		 */
		virtual float getAspectRatio() const { return mInternal->getAspectRatio(); }

		/**
		 * @copydoc	CameraHandler::setFrustumExtents
		 */
		virtual void setFrustumExtents(float left, float right, float top, float bottom)
		{
			mInternal->setFrustumExtents(left, right, top, bottom);
		}

		/**
		 * @copydoc	CameraHandler::resetFrustumExtents
		 */
		virtual void resetFrustumExtents() { mInternal->resetFrustumExtents(); }

		/**
		 * @copydoc	CameraHandler::getFrustumExtents
		 */
		virtual void getFrustumExtents(float& outleft, float& outright, float& outtop, float& outbottom) const
		{
			mInternal->getFrustumExtents(outleft, outright, outtop, outbottom);
		}

		/**
		 * @copydoc	CameraHandler::getProjectionMatrixRS
		 */
		virtual const Matrix4& getProjectionMatrixRS() const { return mInternal->getProjectionMatrixRS(); }

		/**
		 * @copydoc	CameraHandler::getProjectionMatrix
		 */
		virtual const Matrix4& getProjectionMatrix() const { return mInternal->getProjectionMatrix(); }

		/**
		 * @copydoc	CameraHandler::getViewMatrix
		 */
		virtual const Matrix4& getViewMatrix() const { updateView(); return mInternal->getViewMatrix(); }

		/**
		 * @copydoc	CameraHandler::setCustomViewMatrix
		 */
		virtual void setCustomViewMatrix(bool enable, const Matrix4& viewMatrix = Matrix4::IDENTITY)
		{
			mInternal->setCustomViewMatrix(enable, viewMatrix);
		}

		/**
		 * @copydoc	CameraHandler::isCustomViewMatrixEnabled
		 */
		virtual bool isCustomViewMatrixEnabled() const { return mInternal->isCustomViewMatrixEnabled(); }
		
		/**
		 * @copydoc	CameraHandler::setCustomProjectionMatrix
		 */
		virtual void setCustomProjectionMatrix(bool enable, const Matrix4& projectionMatrix = Matrix4::IDENTITY)
		{
			mInternal->setCustomProjectionMatrix(enable, projectionMatrix);
		}

		/**
		 * @copydoc	CameraHandler::isCustomProjectionMatrixEnabled
		 */
		virtual bool isCustomProjectionMatrixEnabled() const { return mInternal->isCustomProjectionMatrixEnabled(); }

		/**
		 * @copydoc	CameraHandler::getFrustum
		 */
		virtual const ConvexVolume& getFrustum() const { return mInternal->getFrustum(); }

		/**
		 * @copydoc	CameraHandler::getWorldFrustum
		 */
        virtual ConvexVolume getWorldFrustum() const;

		/**
		 * @copydoc	CameraHandler::getBoundingBox
		 */
		const AABox& getBoundingBox() const { return mInternal->getBoundingBox(); }

		/**
		 * @copydoc	CameraHandler::setProjectionType
		 */
		virtual void setProjectionType(ProjectionType pt) { mInternal->setProjectionType(pt); }

		/**
		 * @copydoc	CameraHandler::getProjectionType
		 */
		virtual ProjectionType getProjectionType() const { return mInternal->getProjectionType(); }

		/**
		 * @copydoc	CameraHandler::setOrthoWindow
		 */
		virtual void setOrthoWindow(float w, float h) { mInternal->setOrthoWindow(w, h); }

		/**
		 * @copydoc	CameraHandler::setOrthoWindowHeight
		 */
		virtual void setOrthoWindowHeight(float h) { mInternal->setOrthoWindowHeight(h); }

		/**
		 * @copydoc	CameraHandler::setOrthoWindowWidth
		 */
		virtual void setOrthoWindowWidth(float w) { mInternal->setOrthoWindowWidth(w); }

		/**
		 * @copydoc	CameraHandler::getOrthoWindowHeight
		 */
		virtual float getOrthoWindowHeight() const { return mInternal->getOrthoWindowHeight(); }

		/**
		 * @copydoc	CameraHandler::getOrthoWindowWidth
		 */
		virtual float getOrthoWindowWidth() const { return mInternal->getOrthoWindowWidth(); }

		/**
		 * @copydoc	CameraHandler::setIgnoreSceneRenderables
		 */
		void setIgnoreSceneRenderables(bool value) { mInternal->setIgnoreSceneRenderables(value); }

		/**
		 * @copydoc	CameraHandler::getIgnoreSceneRenderables
		 */
		bool getIgnoreSceneRenderables() const { return mInternal->getIgnoreSceneRenderables(); }

		/**
		 * @copydoc	CameraHandler::getPriority
		 */
		INT32 getPriority() const { return mInternal->getPriority(); }

		/**
		 * @copydoc	CameraHandler::setPriority
		 */
		void setPriority(INT32 priority) { mInternal->setPriority(priority); }

		/**
		 * @copydoc	CameraHandler::getLayers
		 */
		UINT64 getLayers() const { return mInternal->getLayers(); }

		/**
		 * @copydoc	CameraHandler::setLayers
		 */
		void setLayers(UINT64 layers) { mInternal->setLayers(layers); }

		/**
		 * @copydoc	CameraHandler::worldToScreenPoint
		 */
		Vector2I worldToScreenPoint(const Vector3& worldPoint) const { updateView(); return mInternal->worldToScreenPoint(worldPoint); }

		/**
		 * @copydoc	CameraHandler::worldToClipPoint
		 */
		Vector2 worldToClipPoint(const Vector3& worldPoint) const { updateView(); return mInternal->worldToClipPoint(worldPoint); }

		/**
		 * @copydoc	CameraHandler::worldToViewPoint
		 */
		Vector3 worldToViewPoint(const Vector3& worldPoint) const { updateView(); return mInternal->worldToViewPoint(worldPoint); }

		/**
		 * @copydoc	CameraHandler::screenToWorldPoint
		 */
		Vector3 screenToWorldPoint(const Vector2I& screenPoint) const { updateView(); return mInternal->screenToWorldPoint(screenPoint); }

		/**
		 * @copydoc	CameraHandler::screenToViewPoint
		 */
		Vector3 screenToViewPoint(const Vector2I& screenPoint) const { return mInternal->screenToViewPoint(screenPoint); }

		/**
		 * @copydoc	CameraHandler::screenToClipPoint
		 */
		Vector2 screenToClipPoint(const Vector2I& screenPoint) const { return mInternal->screenToClipPoint(screenPoint); }

		/**
		 * @copydoc	CameraHandler::viewToWorldPoint
		 */
		Vector3 viewToWorldPoint(const Vector3& viewPoint) const { updateView(); return mInternal->viewToWorldPoint(viewPoint); }

		/**
		 * @copydoc	CameraHandler::viewToScreenPoint
		 */
		Vector2I viewToScreenPoint(const Vector3& viewPoint) const { return mInternal->viewToScreenPoint(viewPoint); }

		/**
		 * @copydoc	CameraHandler::viewToClipPoint
		 */
		Vector2 viewToClipPoint(const Vector3& viewPoint) const { return mInternal->viewToClipPoint(viewPoint); }

		/**
		 * @copydoc	CameraHandler::clipToWorldPoint
		 */
		Vector3 clipToWorldPoint(const Vector2& clipPoint) const { updateView(); return mInternal->clipToWorldPoint(clipPoint); }

		/**
		 * @copydoc	CameraHandler::clipToViewPoint
		 */
		Vector3 clipToViewPoint(const Vector2& clipPoint) const { return mInternal->clipToViewPoint(clipPoint); }

		/**
		 * @copydoc	CameraHandler::clipToScreenPoint
		 */
		Vector2I clipToScreenPoint(const Vector2& clipPoint) const { return mInternal->clipToScreenPoint(clipPoint); }

		/**
		 * @copydoc	CameraHandler::screenPointToRay
		 */
		Ray screenPointToRay(const Vector2I& screenPoint) const { updateView(); return mInternal->screenPointToRay(screenPoint); }

		/**
		 * @copydoc	CameraHandler::projectPoint
		 */
		Vector3 projectPoint(const Vector3& point) const { return mInternal->projectPoint(point); }

		/**
		 * @copydoc	CameraHandler::unprojectPoint
		 */
		Vector3 unprojectPoint(const Vector3& point) const { return mInternal->unprojectPoint(point); }

		/**
		 * @brief	Returns the internal camera handler that is used for
		 *			majority of operations by this component.
		 */
		CameraHandlerPtr _getHandler() const { updateView(); return mInternal; }

    protected:
		/**
		 * @brief	Checks if the world transform of the camera changed, and if needed updates
		 *			the view matrix.
		 */
		void updateView() const;

		mutable CameraHandlerPtr mInternal;
		mutable UINT32 mLastUpdateHash;

		/************************************************************************/
		/* 						COMPONENT OVERRIDES                      		*/
		/************************************************************************/
	protected:
		friend class SceneObject;

		/**
		 * @copydoc	Component::onDestroyed
		 */
		void onDestroyed();

	public:
		/**
		 * @copydoc	Component::update
		 */
		virtual void update();

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class CameraRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	protected:
		Camera() {} // Serialization only
     };
}