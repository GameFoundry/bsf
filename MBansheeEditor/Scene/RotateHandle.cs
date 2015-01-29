using BansheeEngine;

namespace BansheeEditor
{
    public sealed class RotateHandle : DefaultHandle
    {
        private Quaternion delta;

        private HandleSliderDisc xAxis;
        private HandleSliderDisc yAxis;
        private HandleSliderDisc zAxis;

        public Quaternion Delta
        {
            get { return delta; }
        }

        internal override bool IsDragged()
        {
            return xAxis.State == HandleSlider.StateType.Active;
            //return xAxis.State == HandleSlider.StateType.Active ||
            //        yAxis.State == HandleSlider.StateType.Active ||
            //        zAxis.State == HandleSlider.StateType.Active;
        }

        public RotateHandle()
        {
            xAxis = new HandleSliderDisc(this, Vector3.xAxis, 1.0f);
            //yAxis = new HandleSliderDisc(this, Vector3.yAxis, 1.0f);
            //zAxis = new HandleSliderDisc(this, Vector3.zAxis, 1.0f);
        }

        protected override void PreInput()
        {
            xAxis.Position = position;
           // yAxis.Position = position;
            //zAxis.Position = position;

            xAxis.Rotation = rotation;
            //yAxis.Rotation = rotation;
            //zAxis.Rotation = rotation;
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
                //yValue = Handles.SnapValue(yAxis.Delta, Handles.RotateSnapAmount);
                //zValue = Handles.SnapValue(zAxis.Delta, Handles.RotateSnapAmount);
            }
            else
            {
                xValue = xAxis.Delta;
                //yValue = yAxis.Delta;
               // zValue = zAxis.Delta;
            }

            delta = Quaternion.FromAxisAngle(GetXDir(), xValue) * delta;
           // delta = Quaternion.FromAxisAngle(GetYDir(), yValue) * delta;
           // delta = Quaternion.FromAxisAngle(GetZDir(), zValue) * delta;
        }

        protected override void Draw()
        {
            HandleDrawing.SetTransform(Matrix4.TRS(Position, Rotation, Vector3.one));
            float handleSize = Handles.GetHandleSize(EditorApplication.SceneViewCamera, position);

            // Draw arcs
            Color axisHover = new Color(0.8f, 0.8f, 0.8f, 1.0f);

            if (xAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.white);
            else if(xAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.red * axisHover);
            else
                HandleDrawing.SetColor(Color.red);

            Vector3 xStartDir = Vector3.Cross(EditorApplication.SceneViewCamera.sceneObject.Forward, GetXDir());
            Degree xStartAngle = PointOnCircleToAngle(GetXDir(), xStartDir);

            Debug.Log("START ARC: " + xStartDir + " -- " + xStartAngle);

            HandleDrawing.DrawWireArc(Vector3.zero, GetXDir(), 1.0f, xStartAngle, 180.0f, handleSize);

            //if (yAxis.State == HandleSlider.StateType.Active)
            //    HandleDrawing.SetColor(Color.white);
            //else if (yAxis.State == HandleSlider.StateType.Hover)
            //    HandleDrawing.SetColor(Color.green * axisHover);
            //else
            //    HandleDrawing.SetColor(Color.green);

            //Vector3 yStartDir = Vector3.Cross(EditorApplication.SceneViewCamera.sceneObject.Forward, GetYDir());
            //Degree yStartAngle = PointOnCircleToAngle(GetYDir(), yStartDir);

            //HandleDrawing.DrawWireArc(Vector3.zero, GetYDir(), 1.0f, yStartAngle, 180.0f, handleSize);

            //if (zAxis.State == HandleSlider.StateType.Active)
            //    HandleDrawing.SetColor(Color.white);
            //else if (zAxis.State == HandleSlider.StateType.Hover)
            //    HandleDrawing.SetColor(Color.blue * axisHover);
            //else
            //    HandleDrawing.SetColor(Color.blue);

            //Vector3 zStartDir = Vector3.Cross(EditorApplication.SceneViewCamera.sceneObject.Forward, -GetZDir());
            //Degree zStartAngle = PointOnCircleToAngle(-GetZDir(), zStartDir);

            //HandleDrawing.DrawWireArc(Vector3.zero, -GetZDir(), 1.0f, zStartAngle, 180.0f, handleSize);

            // Draw active rotation pie
            Color gray = new Color(1.0f, 1.0f, 1.0f, 0.3f);
            HandleDrawing.SetColor(gray);

            if (xAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.DrawArc(Vector3.zero, GetXDir(), 1.0f, xAxis.StartAngle, xAxis.Delta, handleSize);
            //else if (yAxis.State == HandleSlider.StateType.Active)
            //    HandleDrawing.DrawArc(Vector3.zero, GetYDir(), 1.0f, yAxis.StartAngle, yAxis.Delta, handleSize);
            //else if (zAxis.State == HandleSlider.StateType.Active)
            //    HandleDrawing.DrawArc(Vector3.zero, -GetZDir(), 1.0f, zAxis.StartAngle, zAxis.Delta, handleSize);

            // TODO - Free rotate handle
        }

        private Vector3 GetXDir()
        {
             return rotation.Rotate(Vector3.xAxis);
        }

        private Vector3 GetYDir()
        {
            return rotation.Rotate(Vector3.yAxis);
        }

        private Vector3 GetZDir()
        {
            return rotation.Rotate(Vector3.zAxis);
        }

        private Degree PointOnCircleToAngle(Vector3 up, Vector3 point)
        {
            Vector3[] arcBasis = new Vector3[3];
            arcBasis[1] = up;
            Vector3.OrthogonalComplement(arcBasis[1], out arcBasis[2], out arcBasis[0]);

            Matrix4 worldToPlane = Matrix4.identity;
            worldToPlane.SetColumn(0, (Vector4)arcBasis[0]);
            worldToPlane.SetColumn(1, (Vector4)arcBasis[1]);
            worldToPlane.SetColumn(2, (Vector4)arcBasis[2]);

            point = worldToPlane.Multiply(point);

            return (MathEx.Atan2(-point.z, -point.x) + MathEx.Pi) * MathEx.Rad2Deg;
        }
    }
}
