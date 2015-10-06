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

        /// <inheritdoc/>
        protected internal override void Initialize()
        {
            BuildGUI();
        }

        /// <inheritdoc/>
        protected internal override void Refresh()
        {
            Camera camera = InspectedObject as Camera;
            if (camera == null)
                return;

            ProjectionType projType = camera.ProjectionType;
            if (projectionTypeField.Value != (ulong)projType)
            {
                projectionTypeField.Value = (ulong)projType;
                ToggleTypeSpecificFields(projType);
            }

            fieldOfView.Value = camera.FieldOfView.Degrees;
            orthoHeight.Value = camera.OrthoHeight;
            aspectField.Value = camera.AspectRatio;
            nearPlaneField.Value = camera.NearClipPlane;
            farPlaneField.Value = camera.FarClipPlane;
            viewportXField.Value = camera.ViewportRect.x;
            viewportYField.Value = camera.ViewportRect.y;
            viewportWidthField.Value = camera.ViewportRect.width;
            viewportHeightField.Value = camera.ViewportRect.height;
            clearFlagsFields.Value = (ulong)camera.ClearFlags;
            clearStencilField.Value = camera.ClearStencil;
            clearDepthField.Value = camera.ClearDepth;
            clearColorField.Value = camera.ClearColor;
            priorityField.Value = camera.Priority;

            if (layersValue != camera.Layers)
            {
                bool[] states = new bool[64];
                for (int i = 0; i < states.Length; i++)
                    states[i] = (camera.Layers & Layers.Values[i]) == Layers.Values[i];

                layersField.States = states;
                layersValue = camera.Layers;
            }
        }

        /// <summary>
        /// Recreates all the GUI elements used by this inspector.
        /// </summary>
        private void BuildGUI()
        {
            if (InspectedObject != null)
            {
                Camera camera = (Camera)InspectedObject;

                projectionTypeField.OnSelectionChanged += x =>
                {
                    camera.ProjectionType = (ProjectionType)x;
                    ToggleTypeSpecificFields((ProjectionType)x);
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

                Layout.AddElement(projectionTypeField);
                Layout.AddElement(fieldOfView);
                Layout.AddElement(orthoHeight);
                Layout.AddElement(aspectField);
                Layout.AddElement(nearPlaneField);
                Layout.AddElement(farPlaneField);
                GUILayoutX viewportTopLayout = Layout.AddLayoutX();
                viewportTopLayout.AddElement(new GUILabel(new LocEdString("Viewport"), GUIOption.FixedWidth(100)));
                GUILayoutY viewportContentLayout = viewportTopLayout.AddLayoutY();

                GUILayoutX viewportTopRow = viewportContentLayout.AddLayoutX();
                viewportTopRow.AddElement(viewportXField);
                viewportTopRow.AddElement(viewportWidthField);

                GUILayoutX viewportBotRow = viewportContentLayout.AddLayoutX();
                viewportBotRow.AddElement(viewportYField);
                viewportBotRow.AddElement(viewportHeightField);

                Layout.AddElement(clearFlagsFields);
                Layout.AddElement(clearColorField);
                Layout.AddElement(clearDepthField);
                Layout.AddElement(clearStencilField);
                Layout.AddElement(priorityField);
                Layout.AddElement(layersField);

                ToggleTypeSpecificFields(camera.ProjectionType);
            }
        }

        /// <summary>
        /// Enables or disables different GUI elements depending on the projection type.
        /// </summary>
        /// <param name="type">Projection type to show GUI elements for.</param>
        private void ToggleTypeSpecificFields(ProjectionType type)
        {
            if (type == ProjectionType.Orthographic)
            {
                fieldOfView.Active = false;
                orthoHeight.Active = true;
            }
            else
            {
                fieldOfView.Active = true;
                orthoHeight.Active = false;
            }
        }
    }
}
