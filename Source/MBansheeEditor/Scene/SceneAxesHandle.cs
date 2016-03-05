//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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
        private const float BOX_EXTENT = 0.2f;
        private const float DISABLE_THRESHOLD = 0.9f;

        private HandleSliderLine xAxis;
        private HandleSliderLine yAxis;
        private HandleSliderLine zAxis;
        private HandleSliderLine xNegAxis;
        private HandleSliderLine yNegAxis;
        private HandleSliderLine zNegAxis;
        private HandleSliderPlane projTypePlane;

        private bool[] clickStates = new bool[7];

        private Vector3 position = Vector3.Zero;
        private Quaternion rotation = Quaternion.Identity;

        /// <summary>
        /// Creates a new scene axes handle.
        /// </summary>
        public SceneAxesHandle()
        {
            xAxis = new HandleSliderLine(this, Vector3.XAxis, 1.0f - BOX_EXTENT, false, LAYER);
            yAxis = new HandleSliderLine(this, Vector3.YAxis, 1.0f - BOX_EXTENT, false, LAYER);
            zAxis = new HandleSliderLine(this, Vector3.ZAxis, 1.0f - BOX_EXTENT, false, LAYER);

            xNegAxis = new HandleSliderLine(this, -Vector3.XAxis, 1.0f - BOX_EXTENT, false, LAYER);
            yNegAxis = new HandleSliderLine(this, -Vector3.YAxis, 1.0f - BOX_EXTENT, false, LAYER);
            zNegAxis = new HandleSliderLine(this, -Vector3.ZAxis, 1.0f - BOX_EXTENT, false, LAYER);

            projTypePlane = new HandleSliderPlane(this, Vector3.XAxis, Vector3.YAxis, BOX_EXTENT * 2.0f, false, LAYER);
        }

        /// <inheritdoc/>
        protected internal override void PreInput()
        {
            Camera cam = EditorApplication.SceneViewCamera;
            if (cam == null)
                return;

            position = new Vector3(0, 0, -5.0f);
            rotation = cam.SceneObject.Rotation.Inverse;

            Vector3 xOffset = rotation.Rotate(new Vector3(BOX_EXTENT, 0.0f, 0.0f));
            Vector3 yOffset = rotation.Rotate(new Vector3(0.0f, BOX_EXTENT, 0.0f));
            Vector3 zOffset = rotation.Rotate(new Vector3(0.0f, 0.0f, BOX_EXTENT));

            xAxis.Position = position + xOffset;
            yAxis.Position = position + yOffset;
            zAxis.Position = position + zOffset;

            xAxis.Rotation = rotation;
            yAxis.Rotation = rotation;
            zAxis.Rotation = rotation;

            xNegAxis.Position = position - xOffset;
            yNegAxis.Position = position - yOffset;
            zNegAxis.Position = position - zOffset;

            xNegAxis.Rotation = rotation;
            yNegAxis.Rotation = rotation;
            zNegAxis.Rotation = rotation;

            Vector3 cameraForward = cam.SceneObject.Forward;
            xAxis.Enabled = MathEx.Abs(Vector3.Dot(cameraForward, Vector3.XAxis)) < DISABLE_THRESHOLD;
            yAxis.Enabled = MathEx.Abs(Vector3.Dot(cameraForward, Vector3.YAxis)) < DISABLE_THRESHOLD;
            zAxis.Enabled = MathEx.Abs(Vector3.Dot(cameraForward, Vector3.ZAxis)) < DISABLE_THRESHOLD;

            xNegAxis.Enabled = MathEx.Abs(Vector3.Dot(cameraForward, Vector3.XAxis)) < DISABLE_THRESHOLD;
            yNegAxis.Enabled = MathEx.Abs(Vector3.Dot(cameraForward, Vector3.YAxis)) < DISABLE_THRESHOLD;
            zNegAxis.Enabled = MathEx.Abs(Vector3.Dot(cameraForward, Vector3.ZAxis)) < DISABLE_THRESHOLD;

            Vector3 freeAxisOffset = new Vector3(-BOX_EXTENT, -BOX_EXTENT, 0.2f);
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
                new Tuple<HandleSlider, Action>(xNegAxis, () => RotateCameraTo(-Vector3.XAxis)),
                new Tuple<HandleSlider, Action>(yNegAxis, () => RotateCameraTo(-Vector3.YAxis)),
                new Tuple<HandleSlider, Action>(zNegAxis, () => RotateCameraTo(-Vector3.ZAxis)),
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

            Vector3 xLineStart = Vector3.XAxis* BOX_EXTENT;
            Vector3 xConeStart = Vector3.XAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(xLineStart, xConeStart);
            HandleDrawing.DrawCone(xConeStart, Vector3.XAxis, CONE_HEIGHT, CONE_RADIUS);

            Color yColor = Color.Green;
            if (yAxis.State == HandleSlider.StateType.Active)
                yColor = Color.White;
            else if (yAxis.State == HandleSlider.StateType.Hover)
                yColor = Color.BansheeOrange;

            yColor.a = MathEx.Lerp(1.0f, 0.0f, MathEx.Abs(Vector3.Dot(cameraForward, Vector3.YAxis)), 0.8f, 1.0f);
            HandleDrawing.Color = yColor;

            Vector3 yLineStart = Vector3.YAxis * BOX_EXTENT;
            Vector3 yConeStart = Vector3.YAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(yLineStart, yConeStart);
            HandleDrawing.DrawCone(yConeStart, Vector3.YAxis, CONE_HEIGHT, CONE_RADIUS);

            Color zColor = Color.Blue;
            if (zAxis.State == HandleSlider.StateType.Active)
                zColor = Color.White;
            else if (zAxis.State == HandleSlider.StateType.Hover)
                zColor = Color.BansheeOrange;

            zColor.a = MathEx.Lerp(1.0f, 0.0f, MathEx.Abs(Vector3.Dot(cameraForward, Vector3.ZAxis)), 0.8f, 1.0f);
            HandleDrawing.Color = zColor;

            Vector3 zLineStart = Vector3.ZAxis * BOX_EXTENT;
            Vector3 zConeStart = Vector3.ZAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(zLineStart, zConeStart);
            HandleDrawing.DrawCone(zConeStart, Vector3.ZAxis, CONE_HEIGHT, CONE_RADIUS);

            // Draw negative 1D arrows
            Color xNegColor = Color.LightGray;
            if (xNegAxis.State == HandleSlider.StateType.Active)
                xNegColor = Color.White;
            else if (xNegAxis.State == HandleSlider.StateType.Hover)
                xNegColor = Color.BansheeOrange;

            xNegColor.a = MathEx.Lerp(1.0f, 0.0f, MathEx.Abs(Vector3.Dot(cameraForward, Vector3.XAxis)), 0.8f, 1.0f);
            HandleDrawing.Color = xNegColor;

            Vector3 xNegLineStart = -Vector3.XAxis * BOX_EXTENT;
            Vector3 xNegConeStart = -Vector3.XAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(xNegLineStart, xNegConeStart);
            HandleDrawing.DrawCone(xNegConeStart, -Vector3.XAxis, CONE_HEIGHT, CONE_RADIUS);

            Color yNegColor = Color.LightGray;
            if (yNegAxis.State == HandleSlider.StateType.Active)
                yNegColor = Color.White;
            else if (yNegAxis.State == HandleSlider.StateType.Hover)
                yNegColor = Color.BansheeOrange;

            yNegColor.a = MathEx.Lerp(1.0f, 0.0f, MathEx.Abs(Vector3.Dot(cameraForward, Vector3.YAxis)), 0.8f, 1.0f);
            HandleDrawing.Color = yNegColor;

            Vector3 yNegLineStart = -Vector3.YAxis * BOX_EXTENT;
            Vector3 yNegConeStart = -Vector3.YAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(yNegLineStart, yNegConeStart);
            HandleDrawing.DrawCone(yNegConeStart, -Vector3.YAxis, CONE_HEIGHT, CONE_RADIUS);

            Color zNegcolor = Color.LightGray;
            if (zNegAxis.State == HandleSlider.StateType.Active)
                zNegcolor = Color.White;
            else if (zNegAxis.State == HandleSlider.StateType.Hover)
                zNegcolor = Color.BansheeOrange;

            zNegcolor.a = MathEx.Lerp(1.0f, 0.0f, MathEx.Abs(Vector3.Dot(cameraForward, Vector3.ZAxis)), 0.8f, 1.0f);
            HandleDrawing.Color = zNegcolor;

            Vector3 zNegLineStart = -Vector3.ZAxis * BOX_EXTENT;
            Vector3 zNegConeStart = -Vector3.ZAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(zNegLineStart, zNegConeStart);
            HandleDrawing.DrawCone(zNegConeStart, -Vector3.ZAxis, CONE_HEIGHT, CONE_RADIUS);

            // Draw projection type handle
            if (projTypePlane.State == HandleSlider.StateType.Active)
                HandleDrawing.Color = Color.White;
            else if (projTypePlane.State == HandleSlider.StateType.Hover)
                HandleDrawing.Color = Color.BansheeOrange;
            else
                HandleDrawing.Color = Color.White;

            HandleDrawing.DrawCube(Vector3.Zero, new Vector3(BOX_EXTENT, BOX_EXTENT, BOX_EXTENT));
        }

        private void RotateCameraTo(Vector3 axis)
        {
            SceneWindow sceneWindow = EditorWindow.GetWindow<SceneWindow>();
            if (sceneWindow != null)
                sceneWindow.LookAlong(axis);
        }

        private void ToggleProjectionType()
        {
            SceneWindow sceneWindow = EditorWindow.GetWindow<SceneWindow>();
            if (sceneWindow != null)
            {
                if (sceneWindow.ProjectionType == ProjectionType.Orthographic)
                    sceneWindow.ProjectionType = ProjectionType.Perspective;
                else
                    sceneWindow.ProjectionType = ProjectionType.Orthographic;
            }
                
        }
    }
}
