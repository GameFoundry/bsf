using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="Camera"/> component.
    /// </summary>
    [CustomInspector(typeof(Camera))]
    public class CameraInspector : Inspector
    {
        private bool isInitialized;
        private GUIEnumField mProjectionTypeField = new GUIEnumField(typeof(ProjectionType), new LocEdString("Projection type"));
        private GUISliderField mFieldOfView = new GUISliderField(1, 360, new LocEdString("Field of view"));
        private GUIFloatField mOrthoHeight = new GUIFloatField(new LocEdString("Orthographic height"));
        private GUIFloatField mAspectField = new GUIFloatField(new LocEdString("Aspect ratio"));
        private GUIFloatField mNearPlaneField = new GUIFloatField(new LocEdString("Near plane"));
        private GUIFloatField mFarPlaneField = new GUIFloatField(new LocEdString("Far plane"));
        private GUIFloatField mViewportXField = new GUIFloatField(new LocEdString("X"), 30);
        private GUIFloatField mViewportYField = new GUIFloatField(new LocEdString("Y"), 30);
        private GUIFloatField mViewportWidthField = new GUIFloatField(new LocEdString("Width"), 30);
        private GUIFloatField mViewportHeightField = new GUIFloatField(new LocEdString("Height"), 30);
        private GUIEnumField mClearFlagsFields = new GUIEnumField(typeof (ClearFlags), new LocEdString("Clear flags"));
        private GUIIntField mClearStencilField = new GUIIntField(new LocEdString("Clear stencil"));
        private GUIFloatField mClearDepthField = new GUIFloatField(new LocEdString("Clear depth"));
        private GUIColorField mClearColorField = new GUIColorField(new LocEdString("Clear color"));
        private GUIIntField mPriorityField = new GUIIntField(new LocEdString("Render priority"));

        /// <summary>
        /// Initializes required data the first time <see cref="Refresh"/> is called.
        /// </summary>
        private void Initialize()
        {
            if (referencedObject != null)
            {
                Camera camera = (Camera) referencedObject;

                mProjectionTypeField.OnSelectionChanged += x =>
                {
                    camera.ProjectionType = (ProjectionType) x;

                    if (camera.ProjectionType == ProjectionType.Orthographic)
                    {
                        mFieldOfView.Visible = false;
                        mOrthoHeight.Visible = true;
                    }
                    else
                    {
                        mFieldOfView.Visible = true;
                        mOrthoHeight.Visible = false;
                    }
                };

                Debug.Log(camera.FieldOfView);
                
                mFieldOfView.OnChanged += x => camera.FieldOfView = x;
                mOrthoHeight.OnChanged += x => camera.OrthoHeight = x;
                mAspectField.OnChanged += x => camera.AspectRatio = x;
                mNearPlaneField.OnChanged += x => camera.NearClipPlane = x;
                mFarPlaneField.OnChanged += x => camera.FarClipPlane = x;
                mViewportXField.OnChanged += x => 
                    { Rect2 rect = camera.ViewportRect; rect.x = x; camera.ViewportRect = rect; };
                mViewportYField.OnChanged += x => 
                    { Rect2 rect = camera.ViewportRect; rect.y = x; camera.ViewportRect = rect; };
                mViewportWidthField.OnChanged += x => 
                    { Rect2 rect = camera.ViewportRect; rect.width = x; camera.ViewportRect = rect; };
                mViewportHeightField.OnChanged += x => 
                    { Rect2 rect = camera.ViewportRect; rect.height = x; camera.ViewportRect = rect; };
                mClearFlagsFields.OnSelectionChanged += x => camera.ClearFlags = (ClearFlags)x;
                mClearStencilField.OnChanged += x => camera.ClearStencil = (ushort)x;
                mClearDepthField.OnChanged += x => camera.ClearDepth = x;
                mClearColorField.OnChanged += x => camera.ClearColor = x;
                mPriorityField.OnChanged += x => camera.Priority = x;

                layout.AddElement(mProjectionTypeField);
                layout.AddElement(mFieldOfView);
                layout.AddElement(mOrthoHeight);
                layout.AddElement(mAspectField);
                layout.AddElement(mNearPlaneField);
                layout.AddElement(mFarPlaneField);
                GUILayoutX viewportTopLayout = layout.AddLayoutX();
                viewportTopLayout.AddElement(new GUILabel(new LocEdString("Viewport"), GUIOption.FixedWidth(100)));
                GUILayoutY viewportContentLayout = viewportTopLayout.AddLayoutY();

                GUILayoutX viewportTopRow = viewportContentLayout.AddLayoutX();
                viewportTopRow.AddElement(mViewportXField);
                viewportTopRow.AddElement(mViewportWidthField);

                GUILayoutX viewportBotRow = viewportContentLayout.AddLayoutX();
                viewportBotRow.AddElement(mViewportYField);
                viewportBotRow.AddElement(mViewportHeightField);

                layout.AddElement(mClearFlagsFields);
                layout.AddElement(mClearColorField);
                layout.AddElement(mClearDepthField);
                layout.AddElement(mClearStencilField);
                layout.AddElement(mPriorityField);

                if (camera.ProjectionType == ProjectionType.Orthographic)
                    mFieldOfView.Visible = false;
                else
                    mOrthoHeight.Visible = false;
            }

            isInitialized = true;
        }

        /// <inheritdoc/>
        internal override bool Refresh()
        {
            if (!isInitialized)
                Initialize();

            Camera camera = referencedObject as Camera;
            if (camera == null)
                return false;

            bool anythingModified = false;

            if (mProjectionTypeField.Value != (int) camera.ProjectionType)
            {
                mProjectionTypeField.Value = (int)camera.ProjectionType;
                anythingModified = true;
            }

            if (mFieldOfView.Value != camera.FieldOfView.Degrees)
            {
                mFieldOfView.Value = camera.FieldOfView.Degrees;
                anythingModified = true;
            }

            if (mOrthoHeight.Value != camera.OrthoHeight)
            {
                mOrthoHeight.Value = camera.OrthoHeight;
                anythingModified = true;
            }

            if (mAspectField.Value != camera.AspectRatio)
            {
                mAspectField.Value = camera.AspectRatio;
                anythingModified = true;
            }

            if (mNearPlaneField.Value != camera.NearClipPlane)
            {
                mNearPlaneField.Value = camera.NearClipPlane;
                anythingModified = true;
            }

            if (mFarPlaneField.Value != camera.FarClipPlane)
            {
                mFarPlaneField.Value = camera.FarClipPlane;
                anythingModified = true;
            }

            if (mViewportXField.Value != camera.ViewportRect.x)
            {
                mViewportXField.Value = camera.ViewportRect.x;
                anythingModified = true;
            }

            if (mViewportYField.Value != camera.ViewportRect.y)
            {
                mViewportYField.Value = camera.ViewportRect.y;
                anythingModified = true;
            }

            if (mViewportWidthField.Value != camera.ViewportRect.width)
            {
                mViewportWidthField.Value = camera.ViewportRect.width;
                anythingModified = true;
            }

            if (mViewportHeightField.Value != camera.ViewportRect.height)
            {
                mViewportHeightField.Value = camera.ViewportRect.height;
                anythingModified = true;
            }

            if (mClearFlagsFields.Value != (int)camera.ClearFlags)
            {
                mClearFlagsFields.Value = (int)camera.ClearFlags;
                anythingModified = true;
            }

            if (mClearStencilField.Value != camera.ClearStencil)
            {
                mClearStencilField.Value = camera.ClearStencil;
                anythingModified = true;
            }

            if (mClearDepthField.Value != camera.ClearDepth)
            {
                mClearDepthField.Value = camera.ClearDepth;
                anythingModified = true;
            }

            if (mClearColorField.Value != camera.ClearColor)
            {
                mClearColorField.Value = camera.ClearColor;
                anythingModified = true;
            }

            if (mPriorityField.Value != camera.Priority)
            {
                mPriorityField.Value = camera.Priority;
                anythingModified = true;
            }

            return anythingModified;
        }
    }
}
