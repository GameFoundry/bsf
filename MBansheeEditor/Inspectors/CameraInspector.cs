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
        private GUIEnumField projectionTypeField = new GUIEnumField(typeof(ProjectionType), new LocEdString("Projection type"));
        private GUISliderField fieldOfView = new GUISliderField(1, 360, new LocEdString("Field of view"));
        private GUIFloatField orthoHeight = new GUIFloatField(new LocEdString("Orthographic height"));
        private GUIFloatField aspectField = new GUIFloatField(new LocEdString("Aspect ratio"));
        private GUIFloatField nearPlaneField = new GUIFloatField(new LocEdString("Near plane"));
        private GUIFloatField farPlaneField = new GUIFloatField(new LocEdString("Far plane"));
        private GUIFloatField viewportXField = new GUIFloatField(new LocEdString("X"), 30);
        private GUIFloatField viewportYField = new GUIFloatField(new LocEdString("Y"), 30);
        private GUIFloatField viewportWidthField = new GUIFloatField(new LocEdString("Width"), 30);
        private GUIFloatField viewportHeightField = new GUIFloatField(new LocEdString("Height"), 30);
        private GUIEnumField clearFlagsFields = new GUIEnumField(typeof (ClearFlags), true, new LocEdString("Clear flags"));
        private GUIIntField clearStencilField = new GUIIntField(new LocEdString("Clear stencil"));
        private GUIFloatField clearDepthField = new GUIFloatField(new LocEdString("Clear depth"));
        private GUIColorField clearColorField = new GUIColorField(new LocEdString("Clear color"));
        private GUIIntField priorityField = new GUIIntField(new LocEdString("Render priority"));
        private GUIListBoxField layersField = new GUIListBoxField(Layers.Names, true, new LocEdString("Layers"));

        private ulong layersValue = 0;

        /// <summary>
        /// Initializes required data the first time <see cref="Refresh"/> is called.
        /// </summary>
        private void Initialize()
        {
            if (referencedObject != null)
            {
                Camera camera = (Camera) referencedObject;

                projectionTypeField.OnSelectionChanged += x =>
                {
                    camera.ProjectionType = (ProjectionType) x;

                    if (camera.ProjectionType == ProjectionType.Orthographic)
                    {
                        fieldOfView.Visible = false;
                        orthoHeight.Visible = true;
                    }
                    else
                    {
                        fieldOfView.Visible = true;
                        orthoHeight.Visible = false;
                    }
                };

                fieldOfView.OnChanged += x => camera.FieldOfView = x;
                orthoHeight.OnChanged += x => camera.OrthoHeight = x;
                aspectField.OnChanged += x => camera.AspectRatio = x;
                nearPlaneField.OnChanged += x => camera.NearClipPlane = x;
                farPlaneField.OnChanged += x => camera.FarClipPlane = x;
                viewportXField.OnChanged += x => 
                    { Rect2 rect = camera.ViewportRect; rect.x = x; camera.ViewportRect = rect; };
                viewportYField.OnChanged += x => 
                    { Rect2 rect = camera.ViewportRect; rect.y = x; camera.ViewportRect = rect; };
                viewportWidthField.OnChanged += x => 
                    { Rect2 rect = camera.ViewportRect; rect.width = x; camera.ViewportRect = rect; };
                viewportHeightField.OnChanged += x => 
                    { Rect2 rect = camera.ViewportRect; rect.height = x; camera.ViewportRect = rect; };
                clearFlagsFields.OnSelectionChanged += x => camera.ClearFlags = (ClearFlags)x;
                clearStencilField.OnChanged += x => camera.ClearStencil = (ushort)x;
                clearDepthField.OnChanged += x => camera.ClearDepth = x;
                clearColorField.OnChanged += x => camera.ClearColor = x;
                priorityField.OnChanged += x => camera.Priority = x;
                layersField.OnSelectionChanged += x =>
                {
                    ulong layers = 0;
                    bool[] states = layersField.States;
                    for (int i = 0; i < states.Length; i++)
                        layers |= states[i] ? Layers.Values[i] : 0;

                    layersValue = layers;
                    camera.Layers = layers;
                };

                layout.AddElement(projectionTypeField);
                layout.AddElement(fieldOfView);
                layout.AddElement(orthoHeight);
                layout.AddElement(aspectField);
                layout.AddElement(nearPlaneField);
                layout.AddElement(farPlaneField);
                GUILayoutX viewportTopLayout = layout.AddLayoutX();
                viewportTopLayout.AddElement(new GUILabel(new LocEdString("Viewport"), GUIOption.FixedWidth(100)));
                GUILayoutY viewportContentLayout = viewportTopLayout.AddLayoutY();

                GUILayoutX viewportTopRow = viewportContentLayout.AddLayoutX();
                viewportTopRow.AddElement(viewportXField);
                viewportTopRow.AddElement(viewportWidthField);

                GUILayoutX viewportBotRow = viewportContentLayout.AddLayoutX();
                viewportBotRow.AddElement(viewportYField);
                viewportBotRow.AddElement(viewportHeightField);

                layout.AddElement(clearFlagsFields);
                layout.AddElement(clearColorField);
                layout.AddElement(clearDepthField);
                layout.AddElement(clearStencilField);
                layout.AddElement(priorityField);
                layout.AddElement(layersField);

                if (camera.ProjectionType == ProjectionType.Orthographic)
                    fieldOfView.Visible = false;
                else
                    orthoHeight.Visible = false;
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

            if (projectionTypeField.Value != (ulong)camera.ProjectionType)
            {
                projectionTypeField.Value = (ulong)camera.ProjectionType;
                anythingModified = true;
            }

            if (fieldOfView.Value != camera.FieldOfView.Degrees)
            {
                fieldOfView.Value = camera.FieldOfView.Degrees;
                anythingModified = true;
            }

            if (orthoHeight.Value != camera.OrthoHeight)
            {
                orthoHeight.Value = camera.OrthoHeight;
                anythingModified = true;
            }

            if (aspectField.Value != camera.AspectRatio)
            {
                aspectField.Value = camera.AspectRatio;
                anythingModified = true;
            }

            if (nearPlaneField.Value != camera.NearClipPlane)
            {
                nearPlaneField.Value = camera.NearClipPlane;
                anythingModified = true;
            }

            if (farPlaneField.Value != camera.FarClipPlane)
            {
                farPlaneField.Value = camera.FarClipPlane;
                anythingModified = true;
            }

            if (viewportXField.Value != camera.ViewportRect.x)
            {
                viewportXField.Value = camera.ViewportRect.x;
                anythingModified = true;
            }

            if (viewportYField.Value != camera.ViewportRect.y)
            {
                viewportYField.Value = camera.ViewportRect.y;
                anythingModified = true;
            }

            if (viewportWidthField.Value != camera.ViewportRect.width)
            {
                viewportWidthField.Value = camera.ViewportRect.width;
                anythingModified = true;
            }

            if (viewportHeightField.Value != camera.ViewportRect.height)
            {
                viewportHeightField.Value = camera.ViewportRect.height;
                anythingModified = true;
            }

            if (clearFlagsFields.Value != (ulong)camera.ClearFlags)
            {
                clearFlagsFields.Value = (ulong)camera.ClearFlags;
                anythingModified = true;
            }

            if (clearStencilField.Value != camera.ClearStencil)
            {
                clearStencilField.Value = camera.ClearStencil;
                anythingModified = true;
            }

            if (clearDepthField.Value != camera.ClearDepth)
            {
                clearDepthField.Value = camera.ClearDepth;
                anythingModified = true;
            }

            if (clearColorField.Value != camera.ClearColor)
            {
                clearColorField.Value = camera.ClearColor;
                anythingModified = true;
            }

            if (priorityField.Value != camera.Priority)
            {
                priorityField.Value = camera.Priority;
                anythingModified = true;
            }

            if (layersValue != camera.Layers)
            {
                bool[] states = new bool[64];
                for (int i = 0; i < states.Length; i++)
                    states[i] = (camera.Layers & Layers.Values[i]) == Layers.Values[i];

                layersField.States = states;
                layersValue = camera.Layers;

                anythingModified = true;
            }

            return anythingModified;
        }
    }
}
