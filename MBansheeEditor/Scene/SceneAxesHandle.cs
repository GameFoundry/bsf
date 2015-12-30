using System;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Draws axes that display the orientation of the scene camera. Also allows you to change camera orientation along
    /// one of the axes, or change projection modes.
    /// </summary>
    [CustomHandle(null)]
    internal class SceneAxesHandle : Handle
    {
        public const UInt64 LAYER = 0x7000000000000000;

        private const float CONE_HEIGHT = 0.25f;
        private const float CONE_RADIUS = 0.175f;

        private HandleSliderLine xAxis;
        private HandleSliderLine yAxis;
        private HandleSliderLine zAxis;
        private HandleSliderPlane projTypePlane;

        private bool[] clickStates = new bool[4];

        private Vector3 position = Vector3.Zero;
        private Quaternion rotation = Quaternion.Identity;

        /// <summary>
        /// Creates a new scene axes handle.
        /// </summary>
        public SceneAxesHandle()
        {
            xAxis = new HandleSliderLine(this, Vector3.XAxis, 1.0f, false, LAYER);
            yAxis = new HandleSliderLine(this, Vector3.YAxis, 1.0f, false, LAYER);
            zAxis = new HandleSliderLine(this, Vector3.ZAxis, 1.0f, false, LAYER);

            projTypePlane = new HandleSliderPlane(this, Vector3.XAxis, Vector3.YAxis, 0.4f, false, LAYER);
        }

        /// <inheritdoc/>
        protected internal override void PreInput()
        {
            Camera cam = EditorApplication.SceneViewCamera;
            if (cam == null)
                return;

            position = new Vector3(0, 0, -5.0f);
            rotation = cam.SceneObject.Rotation;

            xAxis.Position = position;
            yAxis.Position = position;
            zAxis.Position = position;

            xAxis.Rotation = rotation;
            yAxis.Rotation = rotation;
            zAxis.Rotation = rotation;

            Vector3 freeAxisOffset = new Vector3(-0.2f, -0.2f, 0.2f);
            projTypePlane.Rotation = Quaternion.Identity;
            projTypePlane.Position = position + freeAxisOffset;
        }

        /// <inheritdoc/>
        protected internal override void PostInput()
        {
            var axes = new []
            {
                new Tuple<HandleSlider, Action>(xAxis, () => RotateCameraTo(Vector3.XAxis)),
                new Tuple<HandleSlider, Action>(yAxis, () => RotateCameraTo(Vector3.YAxis)),
                new Tuple<HandleSlider, Action>(zAxis, () => RotateCameraTo(Vector3.ZAxis)),
                new Tuple<HandleSlider, Action>(projTypePlane, ToggleProjectionType)
            };

            for (int i = 0; i < axes.Length; i++)
            {
                if (axes[i].Item1.State == HandleSlider.StateType.Active)
                {
                    if (!clickStates[i])
                    {
                        axes[i].Item2();
                        clickStates[i] = true;
                    }
                }
                else
                {
                    clickStates[i] = false;
                }
            }
        }

        /// <inheritdoc/>
        protected internal override void Draw()
        {
            HandleDrawing.Layer = LAYER;
            HandleDrawing.Transform = Matrix4.TRS(position, rotation, Vector3.One);
            Vector3 cameraForward = EditorApplication.SceneViewCamera.SceneObject.Forward;

            // Draw 1D arrows
            Color xColor = Color.Red;
            if (xAxis.State == HandleSlider.StateType.Active)
                xColor = Color.White;
            else if (xAxis.State == HandleSlider.StateType.Hover)
                xColor = Color.BansheeOrange;

            xColor.a = MathEx.Lerp(1.0f, 0.0f, MathEx.Abs(Vector3.Dot(cameraForward, Vector3.XAxis)), 0.8f, 1.0f);
            HandleDrawing.Color = xColor;

            Vector3 xConeStart = Vector3.XAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(Vector3.Zero, xConeStart);
            HandleDrawing.DrawCone(xConeStart, Vector3.XAxis, CONE_HEIGHT, CONE_RADIUS);

            Color yColor = Color.Green;
            if (yAxis.State == HandleSlider.StateType.Active)
                yColor = Color.White;
            else if (yAxis.State == HandleSlider.StateType.Hover)
                yColor = Color.BansheeOrange;

            yColor.a = MathEx.Lerp(1.0f, 0.0f, MathEx.Abs(Vector3.Dot(cameraForward, Vector3.YAxis)), 0.8f, 1.0f);
            HandleDrawing.Color = yColor;

            Vector3 yConeStart = Vector3.YAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(Vector3.Zero, yConeStart);
            HandleDrawing.DrawCone(yConeStart, Vector3.YAxis, CONE_HEIGHT, CONE_RADIUS);

            Color zColor = Color.Blue;
            if (zAxis.State == HandleSlider.StateType.Active)
                zColor = Color.White;
            else if (zAxis.State == HandleSlider.StateType.Hover)
                zColor = Color.BansheeOrange;

            zColor.a = MathEx.Lerp(1.0f, 0.0f, MathEx.Abs(Vector3.Dot(cameraForward, Vector3.ZAxis)), 0.8f, 1.0f);
            HandleDrawing.Color = zColor;

            Vector3 zConeStart = Vector3.ZAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(Vector3.Zero, zConeStart);
            HandleDrawing.DrawCone(zConeStart, Vector3.ZAxis, CONE_HEIGHT, CONE_RADIUS);

            // Draw projection type handle
            if (projTypePlane.State == HandleSlider.StateType.Active)
                HandleDrawing.Color = Color.White;
            else if (projTypePlane.State == HandleSlider.StateType.Hover)
                HandleDrawing.Color = Color.BansheeOrange;
            else
                HandleDrawing.Color = Color.White;

            HandleDrawing.DrawCube(Vector3.Zero, new Vector3(0.2f, 0.2f, 0.2f));

            // TODO - Add a text notifying the user whether ortho/proj is active
        }

        private void RotateCameraTo(Vector3 axis)
        {
            // TODO - Rotate to the provided axis. If already looking at that axis, rotate in the opposite direction (-axis)
        }

        private void ToggleProjectionType()
        {
            // TODO - Switch between ortographic and perspective
        }
    }
}
