//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Handle that allows an object to be rotated around the three primary axes, as well as a free axis currently
    /// facing the camera.
    /// </summary>
    public sealed class RotateHandle : DefaultHandle
    {
        private Quaternion delta;

        private HandleSliderDisc xAxis;
        private HandleSliderDisc yAxis;
        private HandleSliderDisc zAxis;

        private HandleSliderDisc freeAxis;

        private bool isDragged;
        private Quaternion dragStartRotation;

        /// <summary>
        /// Amount of rotation applied since the last frame. Only valid while the handle is being dragged.
        /// </summary>
        public Quaternion Delta
        {
            get { return delta; }
        }

        /// <inheritdoc/>
        internal override bool IsDragged()
        {
            return xAxis.State == HandleSlider.StateType.Active ||
                    yAxis.State == HandleSlider.StateType.Active ||
                    zAxis.State == HandleSlider.StateType.Active ||
                    freeAxis.State == HandleSlider.StateType.Active;
        }

        /// <summary>
        /// Creates a new rotation handle.
        /// </summary>
        public RotateHandle()
        {
            xAxis = new HandleSliderDisc(this, Vector3.XAxis, 1.0f);
            yAxis = new HandleSliderDisc(this, Vector3.YAxis, 1.0f);
            zAxis = new HandleSliderDisc(this, Vector3.ZAxis, 1.0f);
            freeAxis = new HandleSliderDisc(this, -Vector3.ZAxis, 1.2f);
        }

        /// <inheritdoc/>
        protected internal override void PreInput()
        {
            xAxis.Position = position;
            yAxis.Position = position;
            zAxis.Position = position;
            freeAxis.Position = position;

            Quaternion handleRotation = isDragged ? dragStartRotation : Rotation;
            xAxis.Rotation = handleRotation;
            yAxis.Rotation = handleRotation;
            zAxis.Rotation = handleRotation;
            freeAxis.Rotation = EditorApplication.SceneViewCamera.SceneObject.Rotation;

            xAxis.SetCutoffPlane(GetXStartAngle(isDragged), true);
            yAxis.SetCutoffPlane(GetYStartAngle(isDragged), true);
            zAxis.SetCutoffPlane(GetZStartAngle(isDragged), true);
        }

        /// <inheritdoc/>
        protected internal override void PostInput()
        {
            if (IsDragged())
            {
                if (!isDragged)
                {
                    isDragged = true;
                    dragStartRotation = Rotation;
                }
            }
            else
            {
                isDragged = false;
                dragStartRotation = Quaternion.Identity;
            }

            Degree xValue = (Degree)0.0f;
            Degree yValue = (Degree)0.0f;
            Degree zValue = (Degree)0.0f;
            Degree freeAxisValue = (Degree)0.0f;

            if (Handles.RotateHandleSnapActive)
            {
                xValue = Handles.SnapValue(xAxis.Delta, Handles.RotateSnapAmount);
                yValue = Handles.SnapValue(yAxis.Delta, Handles.RotateSnapAmount);
                zValue = Handles.SnapValue(zAxis.Delta, Handles.RotateSnapAmount);
                freeAxisValue = Handles.SnapValue(freeAxis.Delta, Handles.RotateSnapAmount);
            }
            else
            {
                xValue = xAxis.Delta;
                yValue = yAxis.Delta;
                zValue = zAxis.Delta;
                freeAxisValue = freeAxis.Delta;
            }

            Vector3 cameraForward = -(dragStartRotation.Inverse * EditorApplication.SceneViewCamera.SceneObject.Rotation).Forward;

            delta = Quaternion.FromEuler(xValue, yValue, zValue);
            delta *= Quaternion.FromAxisAngle(cameraForward, freeAxisValue);
        }

        /// <inheritdoc/>
        protected internal override void Draw()
        {
            HandleDrawing.Layer = 1;
            HandleDrawing.Transform = Matrix4.TRS(Position, Rotation, Vector3.One);
            float handleSize = Handles.GetHandleSize(EditorApplication.SceneViewCamera, position);

            // Draw arcs
            if (xAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.Color = Color.White;
            else if(xAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.Color = Color.BansheeOrange;
            else
                HandleDrawing.Color = Color.Red;

            HandleDrawing.DrawWireArc(Vector3.Zero, Vector3.XAxis, 1.0f, GetXStartAngle(false), new Degree(180.0f), handleSize);

            if (yAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.Color = Color.White;
            else if (yAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.Color = Color.BansheeOrange;
            else
                HandleDrawing.Color = Color.Green;

            HandleDrawing.DrawWireArc(Vector3.Zero, Vector3.YAxis, 1.0f, GetYStartAngle(false), new Degree(180.0f), handleSize);

            if (zAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.Color = Color.White;
            else if (zAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.Color = Color.BansheeOrange;
            else
                HandleDrawing.Color = Color.Blue;

            HandleDrawing.DrawWireArc(Vector3.Zero, Vector3.ZAxis, 1.0f, GetZStartAngle(false), new Degree(180.0f), handleSize);

            // Draw "bounds" and free handle
            Color gray = new Color(1.0f, 1.0f, 1.0f, 0.3f);
            Vector3 cameraNormal = EditorApplication.SceneViewCamera.SceneObject.Rotation.Rotate(Vector3.ZAxis);
            HandleDrawing.Transform = Matrix4.TRS(Position, Quaternion.Identity, Vector3.One);
            HandleDrawing.Color = gray;

            HandleDrawing.DrawWireDisc(cameraNormal * 0.1f, cameraNormal, 1.0f, handleSize);

            if (freeAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.Color = Color.White;
            else if (freeAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.Color = Color.BansheeOrange;
            else
                HandleDrawing.Color = gray;

            HandleDrawing.DrawWireDisc(Vector3.Zero, cameraNormal, 1.2f, handleSize);

            // Draw active rotation pie
            HandleDrawing.Color = gray;
            HandleDrawing.Transform = Matrix4.TRS(Position, EditorApplication.SceneViewCamera.SceneObject.Rotation, Vector3.One);

            if (freeAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.DrawArc(Vector3.Zero, -Vector3.ZAxis, 1.2f, freeAxis.StartAngle, -freeAxis.Delta, handleSize);

            HandleDrawing.Transform = Matrix4.TRS(Position, dragStartRotation, Vector3.One);

            if (xAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.DrawArc(Vector3.Zero, Vector3.XAxis, 1.0f, xAxis.StartAngle, -xAxis.Delta, handleSize);
            else if (yAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.DrawArc(Vector3.Zero, Vector3.YAxis, 1.0f, yAxis.StartAngle, -yAxis.Delta, handleSize);
            else if (zAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.DrawArc(Vector3.Zero, Vector3.ZAxis, 1.0f, zAxis.StartAngle, -zAxis.Delta, handleSize);
        }

        /// <summary>
        /// The rotate handle only displays the 180 degree arc facing the camera and this method returns the angle at which 
        /// the arc starts for the X axis.
        /// </summary>
        /// <param name="frozen">Determines should the local handle rotation be taken into account, or should it be frozen
        ///                      to the value when handle drag started. This is useful because we do not want the visible
        ///                      arc to change while the user is in the process of rotating the handle.</param>
        /// <returns>Angle at which to display the visible arc for the X axis rotations.</returns>
        private Degree GetXStartAngle(bool frozen)
        {
            Quaternion handleRotation = frozen ? dragStartRotation : Rotation;

            Vector3 xStartDir = Vector3.Cross(handleRotation.Inverse.Rotate(EditorApplication.SceneViewCamera.SceneObject.Forward), Vector3.XAxis);
            return PointOnCircleToAngle(Vector3.XAxis, xStartDir);
        }

        /// <summary>
        /// The rotate handle only displays the 180 degree arc facing the camera and this method returns the angle at which 
        /// the arc starts for the Y axis.
        /// </summary>
        /// <param name="frozen">Determines should the local handle rotation be taken into account, or should it be frozen
        ///                      to the value when handle drag started. This is useful because we do not want the visible
        ///                      arc to change while the user is in the process of rotating the handle.</param>
        /// <returns>Angle at which to display the visible arc for the Y axis rotations.</returns>
        private Degree GetYStartAngle(bool frozen)
        {
            Quaternion handleRotation = frozen ? dragStartRotation : Rotation;

            Vector3 yStartDir = Vector3.Cross(handleRotation.Inverse.Rotate(EditorApplication.SceneViewCamera.SceneObject.Forward), Vector3.YAxis);
            return PointOnCircleToAngle(Vector3.YAxis, yStartDir);
        }

        /// <summary>
        /// The rotate handle only displays the 180 degree arc facing the camera and this method returns the angle at which 
        /// the arc starts for the Z axis.
        /// </summary>
        /// <param name="frozen">Determines should the local handle rotation be taken into account, or should it be frozen
        ///                      to the value when handle drag started. This is useful because we do not want the visible
        ///                      arc to change while the user is in the process of rotating the handle.</param>
        /// <returns>Angle at which to display the visible arc for the Z axis rotations.</returns>
        private Degree GetZStartAngle(bool frozen)
        {
            Quaternion handleRotation = frozen ? dragStartRotation : Rotation;

            Vector3 zStartDir = Vector3.Cross(handleRotation.Inverse.Rotate(EditorApplication.SceneViewCamera.SceneObject.Forward), Vector3.ZAxis);
            return PointOnCircleToAngle(Vector3.ZAxis, zStartDir);
        }

        /// <summary>
        /// Converts a point on the circle to an angle on the circle.
        /// </summary>
        /// <param name="up">Up vector determining the orientation of the circle.</param>
        /// <param name="point">Point along a unit circle centered around the origin.</param>
        /// <returns>Angle at which the provided point is located on the circle.</returns>
        private Degree PointOnCircleToAngle(Vector3 up, Vector3 point)
        {
            Quaternion rot = Quaternion.FromToRotation(up, Vector3.YAxis);

            Matrix4 worldToPlane = Matrix4.TRS(Vector3.Zero, rot, Vector3.One);
            point = worldToPlane.MultiplyDirection(point);

            return (MathEx.Atan2(-point.z, -point.x) + MathEx.Pi);
        }
    }
}
