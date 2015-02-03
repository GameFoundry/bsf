using BansheeEngine;

namespace BansheeEditor
{
    public sealed class RotateHandle : DefaultHandle
    {
        private Quaternion delta;

        private HandleSliderDisc xAxis;
        private HandleSliderDisc yAxis;
        private HandleSliderDisc zAxis;

        private HandleSliderDisc freeAxis;

        public Quaternion Delta
        {
            get { return delta; }
        }

        internal override bool IsDragged()
        {
            return xAxis.State == HandleSlider.StateType.Active ||
                    yAxis.State == HandleSlider.StateType.Active ||
                    zAxis.State == HandleSlider.StateType.Active ||
                    freeAxis.State == HandleSlider.StateType.Active;
        }

        public RotateHandle()
        {
            xAxis = new HandleSliderDisc(this, Vector3.xAxis, 1.0f);
            yAxis = new HandleSliderDisc(this, Vector3.yAxis, 1.0f);
            zAxis = new HandleSliderDisc(this, Vector3.zAxis, 1.0f);
            freeAxis = new HandleSliderDisc(this, Vector3.zAxis, 1.0f);
        }

        protected override void PreInput()
        {
            xAxis.Position = position;
            yAxis.Position = position;
            zAxis.Position = position;

            freeAxis.Position = position;
            freeAxis.Rotation = EditorApplication.SceneViewCamera.sceneObject.Rotation;

            xAxis.SetCutoffPlane(GetXStartAngle(), true);
            yAxis.SetCutoffPlane(GetYStartAngle(), true);
            zAxis.SetCutoffPlane(GetZStartAngle(), true);
            freeAxis.SetCutoffPlane(0.0f, false);
        }

        protected override void PostInput()
        {
            delta = Quaternion.identity;

            Degree xValue = 0.0f;
            Degree yValue = 0.0f;
            Degree zValue = 0.0f;

            if (Handles.RotateHandleSnapActive)
            {
                xValue = Handles.SnapValue(xAxis.Delta, Handles.RotateSnapAmount);
                yValue = Handles.SnapValue(yAxis.Delta, Handles.RotateSnapAmount);
                zValue = Handles.SnapValue(zAxis.Delta, Handles.RotateSnapAmount);
            }
            else
            {
                xValue = xAxis.Delta;
                yValue = yAxis.Delta;
                zValue = zAxis.Delta;
            }

            delta = Quaternion.FromAxisAngle(GetXDir(), xValue) * delta;
            delta = Quaternion.FromAxisAngle(GetYDir(), yValue) * delta;
            delta = Quaternion.FromAxisAngle(GetZDir(), zValue) * delta;

            Matrix4 cameraToWorld = EditorApplication.SceneViewCamera.ViewMatrixInverse;

            Vector3 rotDir = cameraToWorld.MultiplyAffine(new Vector3(-Input.PointerDelta.y, -Input.PointerDelta.x, 0));
            delta = Quaternion.FromAxisAngle(rotDir.Normalized, Input.PointerDelta.Magnitude) * delta;
        }

        protected override void Draw()
        {
            HandleDrawing.SetTransform(Matrix4.TRS(Position, Quaternion.identity, Vector3.one));
            float handleSize = Handles.GetHandleSize(EditorApplication.SceneViewCamera, position);

            // Draw arcs
            if (xAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.White);
            else if(xAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.BansheeOrange);
            else
                HandleDrawing.SetColor(Color.Red);

            HandleDrawing.DrawWireArc(Vector3.zero, GetXDir(), 1.0f, GetXStartAngle(), 180.0f, handleSize);

            if (yAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.White);
            else if (yAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.BansheeOrange);
            else
                HandleDrawing.SetColor(Color.Green);

            HandleDrawing.DrawWireArc(Vector3.zero, GetYDir(), 1.0f, GetYStartAngle(), 180.0f, handleSize);

            if (zAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.White);
            else if (zAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.BansheeOrange);
            else
                HandleDrawing.SetColor(Color.Blue);

            HandleDrawing.DrawWireArc(Vector3.zero, GetZDir(), 1.0f, GetZStartAngle(), 180.0f, handleSize);

            // Draw active rotation pie
            Color gray = new Color(1.0f, 1.0f, 1.0f, 0.3f);
            HandleDrawing.SetColor(gray);

            if (xAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.DrawArc(Vector3.zero, GetXDir(), 1.0f, xAxis.StartAngle, xAxis.Delta, handleSize);
            else if (yAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.DrawArc(Vector3.zero, GetYDir(), 1.0f, yAxis.StartAngle, yAxis.Delta, handleSize);
            else if (zAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.DrawArc(Vector3.zero, GetZDir(), 1.0f, zAxis.StartAngle, zAxis.Delta, handleSize);

            // Draw free rotate handle
            if (freeAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.White);
            else if (freeAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.BansheeOrange);
            else
                HandleDrawing.SetColor(Color.White);

            //// Rotate it so it always faces the camera, and move it forward a bit to always render in front
            Vector3 freeHandleNormal = EditorApplication.SceneViewCamera.sceneObject.Rotation.Rotate(GetZDir());
            Vector3 offset = freeHandleNormal*0.1f;

            HandleDrawing.DrawWireDisc(offset, freeHandleNormal, 1.0f, handleSize);
        }

        private Degree GetXStartAngle()
        {
            Vector3 xStartDir = Vector3.Cross(EditorApplication.SceneViewCamera.sceneObject.Forward, GetXDir());
            return PointOnCircleToAngle(GetXDir(), xStartDir);
        }

        private Degree GetYStartAngle()
        {
            Vector3 yStartDir = Vector3.Cross(EditorApplication.SceneViewCamera.sceneObject.Forward, GetYDir());
            return PointOnCircleToAngle(GetYDir(), yStartDir);
        }

        private Degree GetZStartAngle()
        {
            Vector3 zStartDir = Vector3.Cross(EditorApplication.SceneViewCamera.sceneObject.Forward, GetZDir());
            return PointOnCircleToAngle(GetZDir(), zStartDir);
        }

        private Vector3 GetXDir()
        {
             return Vector3.xAxis;
        }

        private Vector3 GetYDir()
        {
            return Vector3.yAxis;
        }

        private Vector3 GetZDir()
        {
            return Vector3.zAxis;
        }

        private Degree PointOnCircleToAngle(Vector3 up, Vector3 point)
        {
            Quaternion rot = Quaternion.FromToRotation(up, Vector3.yAxis);

            Matrix4 worldToPlane = Matrix4.TRS(Vector3.zero, rot, Vector3.one);
            point = worldToPlane.MultiplyAffine(point);

            return (MathEx.Atan2(-point.z, -point.x) + MathEx.Pi) * MathEx.Rad2Deg;
        }
    }
}
