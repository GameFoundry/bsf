#pragma once

#include "BsPrerequisites.h"
#include "BsString.h"
#include "BsMatrix4.h"
#include "BsVector3.h"
#include "BsVector2.h"
#include "BsAABox.h"
#include "BsVertexData.h"
#include "BsPlane.h"
#include "BsQuaternion.h"
#include "BsRay.h"
#include "BsComponent.h"
#include "BsCameraProxy.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Specified projection type to use by the camera.
	 */
    enum ProjectionType
    {
        PT_ORTHOGRAPHIC,
        PT_PERSPECTIVE
    };

	/**
	 * @brief	Clip planes that form the camera frustum (visible area).
	 */
    enum FrustumPlane
    {
        FRUSTUM_PLANE_NEAR = 0,
        FRUSTUM_PLANE_FAR = 1,
        FRUSTUM_PLANE_LEFT = 2,
        FRUSTUM_PLANE_RIGHT = 3,
        FRUSTUM_PLANE_TOP = 4,
        FRUSTUM_PLANE_BOTTOM = 5
    };

	/**
	 * @brief	Camera determines how is world geometry projected onto a 2D surface. You may
	 *			position and orient it in space, and set other options like aspect ratio and field or view.
	 */
    class BS_EXPORT Camera : public Component
    {
    public:
		Camera(const HSceneObject& parent, RenderTargetPtr target = nullptr,
			float left = 0.0f, float top = 0.0f, float width = 1.0f, float height = 1.0f);

        virtual ~Camera();

		/**
		 * @brief	Returns the viewport used by the camera.
		 */	
		ViewportPtr getViewport() const { return mViewport; }

		/**
		 * @brief	Sets the camera horizontal field of view. This determines how wide the camera
		 *			viewing angle is along the horizontal axis. Vertical FOV is calculated from the
		 *			horizontal FOV and the aspect.
		 */
        virtual void setHorzFOV(const Radian& fovy);

		/**
		 * @brief	Retrieves the camera horizontal field of view.
		 */
        virtual const Radian& getHorzFOV() const;

		/**
		 * @brief	Sets the distance from the frustum to the near clipping plane. Anything
		 *			closer than the near clipping plane will not be rendered. Decreasing this value
		 *			decreases depth buffer precision.
		 */
        virtual void setNearClipDistance(float nearDist);

		/**
		 * @brief	Retrieves the distance from the frustum to the near clipping plane. Anything
		 *			closer than the near clipping plane will not be rendered. Decreasing this value
		 *			decreases depth buffer precision.
		 */
        virtual float getNearClipDistance() const;

		/**
		 * @brief	Sets the distance from the frustum to the far clipping plane. Anything
		 *			farther than the far clipping plane will not be rendered. Increasing this value
		 *			decreases depth buffer precision.
		 */
        virtual void setFarClipDistance(float farDist);

		/**
		 * @brief	Retrieves the distance from the frustum to the far clipping plane. Anything
		 *			farther than the far clipping plane will not be rendered. Increasing this value
		 *			decreases depth buffer precision.
		 */
        virtual float getFarClipDistance() const;

		/**
		 * @brief	Sets the current viewport aspect ratio (width / height).
		 */
        virtual void setAspectRatio(float ratio);

		/**
		 * @brief	Returns current viewport aspect ratio (width / height).
		 */
        virtual float getAspectRatio() const;

		/** @brief	Manually set the extents of the frustum that will be used when calculating the
		 *			projection matrix. This will prevents extents for being automatically calculated
		 *			from aspect and near plane so it is up to the caller to keep these values
		 *			accurate.
		 *
		 *	@param left		The position where the left clip plane intersect the near clip plane, in view space.
		 *  @param right	The position where the right clip plane intersect the near clip plane, in view space.
		 *  @param top		The position where the top clip plane intersect the near clip plane, in view space.
		 *  @param bottom	The position where the bottom clip plane intersect the near clip plane, in view space.
		*/
		virtual void setFrustumExtents(float left, float right, float top, float bottom);

		/** 
		 * @brief	Resets frustum extents so they are automatically derived from other values.
		 *			This is only relevant if you have previously set custom extents.
		 */
		virtual void resetFrustumExtents(); 

		/** 
		 * @brief	Returns the extents of the frustum in view space at the near plane.
		 */
		virtual void getFrustumExtents(float& outleft, float& outright, float& outtop, float& outbottom) const;

		/** 
		 * @brief	Returns the standard projection matrix that determines how are 3D points
		 *			projected to two dimensions. The layout of this matrix depends on currently
		 *			used render system.
		 *
		 * @note	You should use this matrix when sending the matrix to the render system to remain
		 *			everything works consistently when other render systems are used.
		 */
        virtual const Matrix4& getProjectionMatrixRS() const;

		/** 
		 * @brief	Returns the standard projection matrix that determines how are 3D points
		 *			projected to two dimensions. Returned matrix is standard following right-hand
		 *			rules and depth range of [-1, 1]. 
		 *
		 * @note	Different render systems will expect different projection matrix layouts, in which
		 *			case use getProjectionMatrixRS.
         */
        virtual const Matrix4& getProjectionMatrix() const;

		/** 
		 * @brief	Gets the camera view matrix. Used for positioning/orienting the camera.
         */
        virtual const Matrix4& getViewMatrix() const;

		/** 
		 * @brief	Sets whether the camera should use the custom view matrix.
		 *			When this is enabled camera will no longer calculate its view matrix
		 *			based on position/orientation and caller will be resonsible to keep 
		 *			the view matrix up to date.
         */
		virtual void setCustomViewMatrix(bool enable, const Matrix4& viewMatrix = Matrix4::IDENTITY);

		/** 
		 * @brief	Returns true if a custom view matrix is used.
         */
		virtual bool isCustomViewMatrixEnabled() const { return mCustomViewMatrix; }
		
		/** 
		 * @brief	Sets whether the camera should use the custom projection matrix.
		 *			When this is enabled camera will no longer calculate its projection matrix
		 *			based on field of view, aspect and other parameters and caller will be
		 *			resonsible to keep the projection matrix up to date.
         */
		virtual void setCustomProjectionMatrix(bool enable, const Matrix4& projectionMatrix = Matrix4::IDENTITY);

		/** 
		 * @brief	Returns true if a custom projection matrix is used.
         */
		virtual bool isCustomProjectionMatrixEnabled() const { return mCustomProjMatrix; }

		/** 
		 * @brief	Returns all frustum planes in an array.
		 *
		 * @note	Planes will be in order as specified by FrustumPlane enum.
         */
        virtual const Plane* getFrustumPlanes() const;

		/** 
		 * @brief	Returns one of the planes forming the camera frustum in world space.
		 *
		 * @param	plane	Index of the plane to retrieve from FrustumPlane enum.
         */
        virtual const Plane& getFrustumPlane(UINT16 plane) const;

        /** 
		 * @brief	Tests whether the given bounding box is visible in the frustum.
		 *
		 * @param	bound		Bounding box to check in world space.
		 * @param	culledBy	(optional)	Exact frustum plane which culled the bounding box.
         */
        virtual bool isVisible(const AABox& bound, FrustumPlane* culledBy = 0) const;

        /** 
		 * @brief	Tests whether the given sphere is visible in the frustum.
		 *
		 * @param	bound		Sphere to check in world space.
		 * @param	culledBy	(optional)	Exact frustum plane which culled the sphere.
         */
        virtual bool isVisible(const Sphere& bound, FrustumPlane* culledBy = 0) const;

        /** 
		 * @brief	Tests whether the given vertex is visible in the frustum.
		 *
		 * @param	vert		Vertex to check in world space.
		 * @param	culledBy	(optional)	Exact frustum plane which culled the vertex.
         */
        virtual bool isVisible(const Vector3& vert, FrustumPlane* culledBy = nullptr) const;

		/**
		 * @brief	Returns the bounding of the frustum.
		 */
        const AABox& getBoundingBox() const;

		/**
		 * @brief	Sets the type of projection used by the camera.
		 */
        virtual void setProjectionType(ProjectionType pt);

		/**
		 * @brief	Returns the type of projection used by the camera.
		 */
        virtual ProjectionType getProjectionType() const;

		/**
		 * @brief	Sets the orthographic window height, for use with orthographic rendering only. 
		 *
		 * @param	w	Width of the window in world units.
		 * @param	h	Height of the window in world units.
		 *
		 * @note	Calling this method will recalculate the aspect ratio, use setOrthoWindowHeight or 
		 *			setOrthoWindowWidth alone if you wish to preserve the aspect ratio but just fit one 
		 *			or other dimension to a particular size.
		 */
		virtual void setOrthoWindow(float w, float h);

		/**
		 * @brief	Sets the orthographic window height, for use with orthographic rendering only. 
		 *
		 * @param	h	Height of the window in world units.
		 *
		 * @note	The width of the window will be calculated from the aspect ratio. 
		 */
		virtual void setOrthoWindowHeight(float h);

		/**
		 * @brief	Sets the orthographic window width, for use with orthographic rendering only. 
		 *
		 * @param	w	Width of the window in world units.
		 *
		 * @note	The height of the window will be calculated from the aspect ratio. 
		 */
		virtual void setOrthoWindowWidth(float w);

		/**
		 * @brief	Gets the orthographic window width in world units, for use with orthographic rendering only. 
		 */
		virtual float getOrthoWindowHeight() const;

		/**
		 * @brief	Gets the orthographic window width in world units, for use with orthographic rendering only. 
		 *
		 * @note	This is calculated from the orthographic height and the aspect ratio.
		 */
		virtual float getOrthoWindowWidth() const;

		/**
		 * @brief	This option tells the renderer that this camera should ignore any renderable components.
		 */
		void setIgnoreSceneRenderables(bool value) { mIgnoreSceneRenderables = true; }

		/**
		 * @brief	This option tells the renderer that this camera should ignore any renderable components.
		 */
		bool getIgnoreSceneRenderables() const { return mIgnoreSceneRenderables; }

		/**
		 * @brief	Gets a priority that determines in which orders the cameras are rendered.
		 *			This only applies to cameras rendering to the same render target. 
		 */
		INT32 getPriority() const { return mPriority; }

		/**
		 * @brief	Sets a priority that determines in which orders the cameras are rendered.
		 * 			This only applies to cameras rendering to the same render target. 
		 *
		 * @param	priority	The priority. Higher value means the camera will be rendered sooner.
		 */
		void setPriority(INT32 priority) { mPriority = priority; }

		/**
		 * @brief	Retrieves layer bitfield that is used when determining which object should the camera render.
		 */
		UINT64 getLayers() const { return mLayers; }

		/**
		 * @brief	Sets layer bitfield that is used when determining which object should the camera render.
		 */
		void setLayers(UINT64 layers) { mLayers = layers; }

		// TODO UNDOCUMENTED
		CameraProxy _createProxy() const;

        static const float INFINITE_FAR_PLANE_ADJUST; /**< Small constant used to reduce far plane projection to avoid inaccuracies. */

	protected:
		/**
		 * @brief	Calculate projection parameters that are used when constructing the projection matrix.
		 */
		virtual void calcProjectionParameters(float& left, float& right, float& bottom, float& top) const;

		/**
		 * @brief	Recalculate frustum if dirty.
		 */
		virtual void updateFrustum() const;

		/**
		 * @brief	Recalculate frustum planes if dirty.
		 */
		virtual void updateFrustumPlanes() const;

		/**
		 * @brief	Update view matrix from parent position/orientation.
		 *
		 * @note	Does nothing when custom view matrix is set.
		 */
		virtual void updateView() const;

		/**
		 * @brief	Checks if the frustum requires updating.
		 */
		virtual bool isFrustumOutOfDate() const;

		/**
		 * @brief	Notify camera that the frustum requires to be updated.
		 */
		virtual void invalidateFrustum() const;

    protected:
		ViewportPtr mViewport; /**< Viewport that describes 2D rendering surface. */
		UINT64 mLayers; /**< Bitfield that can be used for filtering what objects the camera sees. */

		ProjectionType mProjType; /**< Type of camera projection. */
		Radian mHorzFOV; /**< Horizontal field of view represents how wide is the camera angle. */
		float mFarDist; /**< Clip any objects further than this. Larger value decreases depth precision at smaller depths. */
		float mNearDist; /**< Clip any objects close than this. Smaller value decreases depth precision at larger depths. */
		float mAspect; /**< Width/height viewport ratio. */
		float mOrthoHeight; /**< Height in world units used for orthographic cameras. */
		INT32 mPriority; /**< Determines in what order will the camera be rendered. Higher priority means the camera will be rendered sooner. */

		bool mCustomViewMatrix; /**< Is custom view matrix set. */
		bool mCustomProjMatrix; /**< Is custom projection matrix set. */

		bool mFrustumExtentsManuallySet; /**< Are frustum extents manually set. */
		bool mIgnoreSceneRenderables; /**< Should the camera ignore renderable components. */

		mutable Matrix4 mProjMatrixRS; /**< Cached render-system specific projection matrix. */
		mutable Matrix4 mProjMatrix; /**< Cached projection matrix that determines how are 3D points projected to a 2D viewport. */
		mutable Matrix4 mViewMatrix; /**< Cached view matrix that determines camera position/orientation. */

		mutable Plane mFrustumPlanes[6]; /**< Main clipping planes describing cameras visible area. */
		mutable bool mRecalcFrustum; /**< Should frustum be recalculated. */
		mutable bool mRecalcFrustumPlanes; /**< Should frustum planes be recalculated. */
		mutable float mLeft, mRight, mTop, mBottom; /**< Frustum extents. */
		mutable AABox mBoundingBox; /**< Frustum bounding box. */

		/************************************************************************/
		/* 						COMPONENT OVERRIDES                      		*/
		/************************************************************************/
	protected:
		friend class SceneObject;

	public:
		/**
		 * @copydoc	Component::update
		 */
		virtual void update() {}

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