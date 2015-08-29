#pragma once

#include "BsEditorPrerequisites.h"
#include "BsHandleSlider.h"
#include "BsRect3.h"
#include "BsVector2.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handle slider that returns a delta value as you drag the pointer
	 *			along a plane. For intersection purposes the line is internally
	 *			by a quadrilateral (a bounded plane).
	 */
	class BS_ED_EXPORT HandleSliderPlane : public HandleSlider
	{
	public:
		/**
		 * @brief	Constructs a new plane slider. The plane is constructed from two
		 *			direction vectors.
		 *
		 * @param	dir1		First direction of the plane. The x component of returned delta value will be in 
		 *						this direction.
		 * @param	dir2		Second direction of the plane. The y component of returned delta value will be in 
		 *						this direction.
		 * @param	length		Determines size of the plane. 
		 * @param	fixedScale	If true the handle slider will always try to maintain the same visible
		 *						area in the viewport regardless of distance from camera.
		 */
		HandleSliderPlane(const Vector3& dir1, const Vector3& dir2, float length, bool fixedScale);
		~HandleSliderPlane();

		/**
		 * @copydoc	HandleSlider::intersects
		 */
		bool intersects(const Ray& ray, float& t) const override;

		/**
		 * @copydoc	HandleSlider::handleInput
		 */
		void handleInput(const CameraPtr& camera, const Vector2I& inputDelta) override;

		/**
		 * @brief	Returns a delta value that is the result of dragging/sliding the pointer 
		 *			along the plane. Returned movement is in terms of the two directions originally provided
		 *			when constructing the slider. This changes every frame and will be zero unless the slider 
		 *			is active.
		 */
		Vector2 getDelta() const { return mDelta; }
	protected:
		/**
		 * @copydoc	HandleSlider::activate
		 */
		void activate(const CameraPtr& camera, const Vector2I& pointerPos) override;

		/**
		 * @copydoc	HandleSlider::reset
		 */
		void reset() override { mDelta = Vector2::ZERO; }

		/**
		 * @brief	Returns the position on plane based on pointer position.
		 *
		 * @param	camera		Camera we're interacting through.
		 * @param	pointerPos	Position of the pointer in pixels relative to the provided camera's viewport.
		 */
		Vector3 getPositionOnPlane(const CameraPtr& camera, const Vector2I& pointerPos) const;

		Vector3 mDirection1;
		Vector3 mDirection2;
		float mLength;

		Rect3 mCollider;

		Vector2 mDelta;
		Vector3 mStartPlanePosition;
		Vector3 mStartClickPosition;
	};
}