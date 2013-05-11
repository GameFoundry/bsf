/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __Camera_H__
#define __Camera_H__

// Default options
#include "BsPrerequisites.h"

#include "CmString.h"

// Matrices & Vectors
#include "CmMatrix4.h"
#include "CmVector3.h"
#include "CmVector2.h"
#include "CmAxisAlignedBox.h"
#include "CmVertexData.h"
#include "CmIndexData.h"
#include "CmPlane.h"
#include "CmQuaternion.h"
#include "CmCommonEnums.h"
#include "CmRay.h"
#include "CmComponent.h"

namespace BansheeEngine {

	/** Specifies perspective (realistic) or orthographic (architectural) projection.
    */
    enum ProjectionType
    {
        PT_ORTHOGRAPHIC,
        PT_PERSPECTIVE
    };

    /** Worldspace clipping planes.
    */
    enum FrustumPlane
    {
        FRUSTUM_PLANE_NEAR   = 0,
        FRUSTUM_PLANE_FAR    = 1,
        FRUSTUM_PLANE_LEFT   = 2,
        FRUSTUM_PLANE_RIGHT  = 3,
        FRUSTUM_PLANE_TOP    = 4,
        FRUSTUM_PLANE_BOTTOM = 5
    };

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup Scene
	*  @{
	*/

    /** A viewpoint from which the scene will be rendered.
        @remarks
            OGRE renders scenes from a camera viewpoint into a buffer of
            some sort, normally a window or a texture (a subclass of
            RenderTarget). OGRE cameras support both perspective projection (the default,
            meaning objects get smaller the further away they are) and
            orthographic projection (blueprint-style, no decrease in size
            with distance). Each camera carries with it a style of rendering,
            e.g. full textured, flat shaded, wireframe), field of view,
            rendering distances etc, allowing you to use OGRE to create
            complex multi-window views if required. In addition, more than
            one camera can point at a single render target if required,
            each rendering to a subset of the target, allowing split screen
            and picture-in-picture views.
        @par
            Cameras maintain their own aspect ratios, field of view, and frustum,
            and project co-ordinates into a space measured from -1 to 1 in x and y,
            and 0 to 1 in z. At render time, the camera will be rendering to a
            Viewport which will translate these parametric co-ordinates into real screen
            co-ordinates. Obviously it is advisable that the viewport has the same
            aspect ratio as the camera to avoid distortion (unless you want it!).
        @par
            Note that a Camera can be attached to a SceneNode, using the method
            SceneNode::attachObject. If this is done the Camera will combine it's own
            position/orientation settings with it's parent SceneNode. 
            This is useful for implementing more complex Camera / object
            relationships i.e. having a camera attached to a world object.
    */
    class BS_EXPORT Camera : public CM::Component
    {
	protected:
        /// Orthographic or perspective?
        ProjectionType mProjType;

        /// y-direction field-of-view (default 45)
        CM::Radian mFOVy;
        /// Far clip distance - default 10000
        float mFarDist;
        /// Near clip distance - default 100
        float mNearDist;
        /// x/y viewport ratio - default 1.3333
        float mAspect;
		/// Ortho height size (world units)
		float mOrthoHeight;
        /// Off-axis frustum center offset - default (0.0, 0.0)
        CM::Vector2 mFrustumOffset;
        /// Focal length of frustum (for stereo rendering, defaults to 1.0)
        float mFocalLength;

        /// The 6 main clipping planes
        mutable CM::Plane mFrustumPlanes[6];

        /// Stored versions of parent orientation / position
        mutable CM::Quaternion mLastParentOrientation;
        mutable CM::Vector3 mLastParentPosition;

        /// Pre-calced projection matrix for the specific render system
        mutable CM::Matrix4 mProjMatrixRS;
        /// Pre-calced standard projection matrix but with render system depth range
        mutable CM::Matrix4 mProjMatrixRSDepth;
        /// Pre-calced standard projection matrix
        mutable CM::Matrix4 mProjMatrix;
        /// Pre-calced view matrix
        mutable CM::Matrix4 mViewMatrix;
        /// Something's changed in the frustum shape?
        mutable bool mRecalcFrustum;
        /// Something re the frustum planes has changed
        mutable bool mRecalcFrustumPlanes;
        /// Something re the world space corners has changed
        mutable bool mRecalcWorldSpaceCorners;
        /// Something re the vertex data has changed
        mutable bool mRecalcVertexData;
		/// Are we using a custom view matrix?
		bool mCustomViewMatrix;
		/// Are we using a custom projection matrix?
		bool mCustomProjMatrix;
		/// Have the frustum extents been manually set?
		bool mFrustumExtentsManuallySet;
		bool mIgnoreSceneRenderables;
		/// Frustum extents
		mutable float mLeft, mRight, mTop, mBottom;
		
        // Internal functions for calcs
        virtual void calcProjectionParameters(float& left, float& right, float& bottom, float& top) const;
		/// Update frustum if out of date
        virtual void updateFrustum(void) const;
		/// Implementation of updateFrustum (called if out of date)
		virtual void updateFrustumImpl(void) const;
        virtual void updateFrustumPlanes(void) const;
		/// Implementation of updateFrustumPlanes (called if out of date)
		virtual void updateFrustumPlanesImpl(void) const;
        virtual void updateWorldSpaceCorners(void) const;
		/// Implementation of updateWorldSpaceCorners (called if out of date)
		virtual void updateWorldSpaceCornersImpl(void) const;
		virtual void updateView(void) const;
        virtual bool isFrustumOutOfDate(void) const;
        /// Signal to update frustum information.
        virtual void invalidateFrustum(void) const;

        mutable CM::AxisAlignedBox mBoundingBox;

        mutable CM::Vector3 mWorldSpaceCorners[8];

    public:
        /** Sets the Y-dimension Field Of View (FOV) of the frustum.
            @remarks
                Field Of View (FOV) is the angle made between the frustum's position, and the edges
                of the 'screen' onto which the scene is projected. High values (90+ degrees) result in a wide-angle,
                fish-eye kind of view, low values (30- degrees) in a stretched, telescopic kind of view. Typical values
                are between 45 and 60 degrees.
            @par
                This value represents the VERTICAL field-of-view. The horizontal field of view is calculated from
                this depending on the dimensions of the viewport (they will only be the same if the viewport is square).
            @note
                Setting the FOV overrides the value supplied for frustum::setNearClipPlane.
         */
        virtual void setFOVy(const CM::Radian& fovy);

        /** Retrieves the frustums Y-dimension Field Of View (FOV).
        */
        virtual const CM::Radian& getFOVy(void) const;

        /** Sets the position of the near clipping plane.
            @remarks
                The position of the near clipping plane is the distance from the frustums position to the screen
                on which the world is projected. The near plane distance, combined with the field-of-view and the
                aspect ratio, determines the size of the viewport through which the world is viewed (in world
                co-ordinates). Note that this world viewport is different to a screen viewport, which has it's
                dimensions expressed in pixels. The frustums viewport should have the same aspect ratio as the
                screen viewport it renders into to avoid distortion.
            @param
                near The distance to the near clipping plane from the frustum in world coordinates.
         */
        virtual void setNearClipDistance(float nearDist);

        /** Sets the position of the near clipping plane.
        */
        virtual float getNearClipDistance(void) const;

        /** Sets the distance to the far clipping plane.
            @remarks
                The view frustum is a pyramid created from the frustum position and the edges of the viewport.
                This method sets the distance for the far end of that pyramid. 
                Different applications need different values: e.g. a flight sim
                needs a much further far clipping plane than a first-person 
                shooter. An important point here is that the larger the ratio 
                between near and far clipping planes, the lower the accuracy of
                the Z-buffer used to depth-cue pixels. This is because the
                Z-range is limited to the size of the Z buffer (16 or 32-bit) 
                and the max values must be spread over the gap between near and
                far clip planes. As it happens, you can affect the accuracy far 
                more by altering the near distance rather than the far distance, 
                but keep this in mind.
            @param
                far The distance to the far clipping plane from the frustum in 
                world coordinates.If you specify 0, this means an infinite view
                distance which is useful especially when projecting shadows; but
                be careful not to use a near distance too close.
        */
        virtual void setFarClipDistance(float farDist);

        /** Retrieves the distance from the frustum to the far clipping plane.
        */
        virtual float getFarClipDistance(void) const;

        /** Sets the aspect ratio for the frustum viewport.
            @remarks
                The ratio between the x and y dimensions of the rectangular area visible through the frustum
                is known as aspect ratio: aspect = width / height .
            @par
                The default for most fullscreen windows is 1.3333 - this is also assumed by Ogre unless you
                use this method to state otherwise.
        */
        virtual void setAspectRatio(float ratio);

        /** Retreives the current aspect ratio.
        */
        virtual float getAspectRatio(void) const;

        /** Sets frustum offsets, used in stereo rendering.
            @remarks
                You can set both horizontal and vertical plane offsets of "eye"; in
                stereo rendering frustum is moved in horizontal plane. To be able to
                render from two "eyes" you'll need two cameras rendering on two
                RenderTargets.
            @par
                The frustum offsets is in world coordinates, and default to (0, 0) - no offsets.
            @param
                offset The horizontal and vertical plane offsets.
        */
        virtual void setFrustumOffset(const CM::Vector2& offset);

        /** Sets frustum offsets, used in stereo rendering.
            @remarks
                You can set both horizontal and vertical plane offsets of "eye"; in
                stereo rendering frustum is moved in horizontal plane. To be able to
                render from two "eyes" you'll need two cameras rendering on two
                RenderTargets.
            @par
                The frustum offsets is in world coordinates, and default to (0, 0) - no offsets.
            @param
                horizontal The horizontal plane offset.
            @param
                vertical The vertical plane offset.
        */
        virtual void setFrustumOffset(float horizontal = 0.0, float vertical = 0.0);

        /** Retrieves the frustum offsets.
        */
        virtual const CM::Vector2& getFrustumOffset() const;

        /** Sets frustum focal length (used in stereo rendering).
            @param
                focalLength The distance to the focal plane from the frustum in world coordinates.
        */
        virtual void setFocalLength(float focalLength = 1.0);

        /** Returns focal length of frustum.
        */
        virtual float getFocalLength() const;

		/** Manually set the extents of the frustum.
		@param left, right, top, bottom The position where the side clip planes intersect
			the near clip plane, in eye space
		*/
		virtual void setFrustumExtents(float left, float right, float top, float bottom);
		/** Reset the frustum extents to be automatically derived from other params. */
		virtual void resetFrustumExtents(); 
		/** Get the extents of the frustum in view space. */
		virtual void getFrustumExtents(float& outleft, float& outright, float& outtop, float& outbottom) const;


        /** Gets the projection matrix for this frustum adjusted for the current
			rendersystem specifics (may be right or left-handed, depth range
			may vary).
        @remarks
            This method retrieves the rendering-API dependent version of the projection
            matrix. If you want a 'typical' projection matrix then use 
            getProjectionMatrix.

        */
        virtual const CM::Matrix4& getProjectionMatrixRS(void) const;
        /** Gets the depth-adjusted projection matrix for the current rendersystem,
			but one which still conforms to right-hand rules.
        @remarks
            This differs from the rendering-API dependent getProjectionMatrix
            in that it always returns a right-handed projection matrix result 
            no matter what rendering API is being used - this is required for
            vertex and fragment programs for example. However, the resulting depth
            range may still vary between render systems since D3D uses [0,1] and 
            GL uses [-1,1], and the range must be kept the same between programmable
            and fixed-function pipelines.
        */
        virtual const CM::Matrix4& getProjectionMatrixWithRSDepth(void) const;
        /** Gets the normal projection matrix for this frustum, ie the 
        projection matrix which conforms to standard right-handed rules and
        uses depth range [-1,+1].
        @remarks
            This differs from the rendering-API dependent getProjectionMatrixRS
            in that it always returns a right-handed projection matrix with depth
            range [-1,+1], result no matter what rendering API is being used - this
            is required for some uniform algebra for example.
        */
        virtual const CM::Matrix4& getProjectionMatrix(void) const;

        /** Gets the view matrix for this frustum. Mainly for use by OGRE internally.
        */
        virtual const CM::Matrix4& getViewMatrix(void) const;

		/** Set whether to use a custom view matrix on this frustum.
		@remarks
			This is an advanced method which allows you to manually set
			the view matrix on this frustum, rather than having it calculate
			itself based on it's position and orientation. 
		@note
			After enabling a custom view matrix, the frustum will no longer
			update on its own based on position / orientation changes. You 
			are completely responsible for keeping the view matrix up to date.
			The custom matrix will be returned from getViewMatrix.
		@param enable If true, the custom view matrix passed as the second 
			parameter will be used in preference to an auto calculated one. If
			false, the frustum will revert to auto calculating the view matrix.
		@param viewMatrix The custom view matrix to use, the matrix must be an
            affine matrix.
		@see Frustum::setCustomProjectionMatrix, Matrix4::isAffine
		*/
		virtual void setCustomViewMatrix(bool enable, 
			const CM::Matrix4& viewMatrix = CM::Matrix4::IDENTITY);
		/// Returns whether a custom view matrix is in use
		virtual bool isCustomViewMatrixEnabled(void) const 
		{ return mCustomViewMatrix; }
		
		/** Set whether to use a custom projection matrix on this frustum.
		@remarks
			This is an advanced method which allows you to manually set
			the projection matrix on this frustum, rather than having it 
			calculate itself based on it's position and orientation. 
		@note
			After enabling a custom projection matrix, the frustum will no 
			longer update on its own based on field of view and near / far
			distance changes. You are completely responsible for keeping the 
			projection matrix up to date if those values change. The custom 
			matrix will be returned from getProjectionMatrix and derivative
			functions.
		@param enable If true, the custom projection matrix passed as the 
			second parameter will be used in preference to an auto calculated 
			one. If	false, the frustum will revert to auto calculating the 
			projection matrix.
		@param projectionMatrix The custom view matrix to use
		@see Frustum::setCustomViewMatrix
		*/
		virtual void setCustomProjectionMatrix(bool enable, 
			const CM::Matrix4& projectionMatrix = CM::Matrix4::IDENTITY);
		/// Returns whether a custom projection matrix is in use
		virtual bool isCustomProjectionMatrixEnabled(void) const
		{ return mCustomProjMatrix; }

		/** Retrieves the clipping planes of the frustum (world space).
        @remarks
            The clipping planes are ordered as declared in enumerate constants FrustumPlane.
        */
        virtual const CM::Plane* getFrustumPlanes(void) const;

        /** Retrieves a specified plane of the frustum (world space).
            @remarks
                Gets a reference to one of the planes which make up the frustum frustum, e.g. for clipping purposes.
        */
        virtual const CM::Plane& getFrustumPlane( unsigned short plane ) const;

        /** Tests whether the given container is visible in the Frustum.
            @param
                bound Bounding box to be checked (world space)
            @param
                culledBy Optional pointer to an int which will be filled by the plane number which culled
                the box if the result was false;
            @returns
                If the box was visible, true is returned.
            @par
                Otherwise, false is returned.
        */
        virtual bool isVisible(const CM::AxisAlignedBox& bound, FrustumPlane* culledBy = 0) const;

        /** Tests whether the given container is visible in the Frustum.
            @param
                bound Bounding sphere to be checked (world space)
            @param
                culledBy Optional pointer to an int which will be filled by the plane number which culled
                the box if the result was false;
            @returns
                If the sphere was visible, true is returned.
            @par
                Otherwise, false is returned.
        */
        virtual bool isVisible(const CM::Sphere& bound, FrustumPlane* culledBy = 0) const;

        /** Tests whether the given vertex is visible in the Frustum.
            @param
                vert Vertex to be checked (world space)
            @param
                culledBy Optional pointer to an int which will be filled by the plane number which culled
                the box if the result was false;
            @returns
                If the box was visible, true is returned.
            @par
                Otherwise, false is returned.
        */
        virtual bool isVisible(const CM::Vector3& vert, FrustumPlane* culledBy = 0) const;

        /** Overridden from MovableObject */
        const CM::AxisAlignedBox& getBoundingBox(void) const;

        /** Overridden from MovableObject */
		float getBoundingRadius(void) const;

        /** Gets the world space corners of the frustum.
        @remarks
            The corners are ordered as follows: top-right near, 
            top-left near, bottom-left near, bottom-right near, 
            top-right far, top-left far, bottom-left far, bottom-right far.
        */
        virtual const CM::Vector3* getWorldSpaceCorners(void) const;

        /** Sets the type of projection to use (orthographic or perspective). Default is perspective.
        */
        virtual void setProjectionType(ProjectionType pt);

        /** Retrieves info on the type of projection used (orthographic or perspective).
        */
        virtual ProjectionType getProjectionType(void) const;

		/** Sets the orthographic window settings, for use with orthographic rendering only. 
		@note Calling this method will recalculate the aspect ratio, use 
			setOrthoWindowHeight or setOrthoWindowWidth alone if you wish to 
			preserve the aspect ratio but just fit one or other dimension to a 
			particular size.
		@param w, h The dimensions of the view window in world units
		*/
		virtual void setOrthoWindow(float w, float h);
		/** Sets the orthographic window height, for use with orthographic rendering only. 
		@note The width of the window will be calculated from the aspect ratio. 
		@param h The height of the view window in world units
		*/
		virtual void setOrthoWindowHeight(float h);
		/** Sets the orthographic window width, for use with orthographic rendering only. 
		@note The height of the window will be calculated from the aspect ratio. 
		@param w The width of the view window in world units
		*/
		virtual void setOrthoWindowWidth(float w);
		/** Gets the orthographic window height, for use with orthographic rendering only. 
		*/
		virtual float getOrthoWindowHeight() const;
		/** Gets the orthographic window width, for use with orthographic rendering only. 
		@note This is calculated from the orthographic height and the aspect ratio
		*/
		virtual float getOrthoWindowWidth() const;

		void setIgnoreSceneRenderables(bool value) { mIgnoreSceneRenderables = true; }
		bool getIgnoreSceneRenderables() const { return mIgnoreSceneRenderables; }

        /** Project a sphere onto the near plane and get the bounding rectangle. 
        @param sphere The world-space sphere to project
        @param radius Radius of the sphere
        @param left, top, right, bottom Pointers to destination values, these
            will be completed with the normalised device coordinates (in the 
            range {-1,1})
        @returns true if the sphere was projected to a subset of the near plane,
            false if the entire near plane was contained
        */
        virtual bool projectSphere(const CM::Sphere& sphere, 
            float* left, float* top, float* right, float* bottom) const;

        /// Small constant used to reduce far plane projection to avoid inaccuracies
        static const float INFINITE_FAR_PLANE_ADJUST;
    protected:
		CM::ViewportPtr mViewport;

    public:
        /** Standard destructor.
        */
        virtual ~Camera();

		void initialize(CM::RenderTargetPtr target = nullptr,
			float left = 0.0f, float top = 0.0f,
			float width = 1.0f, float height = 1.0f,
			int ZOrder = 0);

		CM::ViewportPtr getViewport() const { return mViewport; }

		/************************************************************************/
		/* 						COMPONENT OVERRIDES                      		*/
		/************************************************************************/
	protected:
		friend class CM::SceneObject;

		/** Standard constructor.
        */
		Camera(const CM::HSceneObject& parent);

	public:
		virtual void update() {}

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class CameraRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;

	protected:
		Camera() {} // Serialization only
     };
	 /** @} */
	 /** @} */

} // namespace CamelotFramework
#endif
