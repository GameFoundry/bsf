using BansheeEngine;

namespace BansheeEditor
{
    public sealed class MoveHandle : DefaultHandle
    {
        private const float CONE_HEIGHT = 0.25f;
        private const float CONE_RADIUS = 0.175f;

        private Vector3 delta;

        private HandleSliderLine xAxis;
        private HandleSliderLine yAxis;
        private HandleSliderLine zAxis;

        private HandleSliderPlane xyPlane;
        private HandleSliderPlane yzPlane;
        private HandleSliderPlane zxPlane;

        public Vector3 Delta
        {
            get { return delta; }
        }

        internal override bool IsDragged()
        {
            return xAxis.State == HandleSlider.StateType.Active ||
                    yAxis.State == HandleSlider.StateType.Active ||
                    zAxis.State == HandleSlider.StateType.Active ||
                    xyPlane.State == HandleSlider.StateType.Active ||
                    yzPlane.State == HandleSlider.StateType.Active ||
                    zxPlane.State == HandleSlider.StateType.Active;
        }

        public MoveHandle()
        {
            xAxis = new HandleSliderLine(this, Vector3.xAxis, 1.0f);
            yAxis = new HandleSliderLine(this, Vector3.yAxis, 1.0f);
            zAxis = new HandleSliderLine(this, Vector3.zAxis, 1.0f);

            xyPlane = new HandleSliderPlane(this, Vector3.xAxis, Vector3.yAxis, 0.3f);
            yzPlane = new HandleSliderPlane(this, Vector3.yAxis, Vector3.zAxis, 0.3f);
            zxPlane = new HandleSliderPlane(this, Vector3.zAxis, Vector3.xAxis, 0.3f);
        }

        protected override void PreInput()
        {
            xAxis.Position = position;
            yAxis.Position = position;
            zAxis.Position = position;

            xyPlane.Position = position;
            yzPlane.Position = position;
            zxPlane.Position = position;

            xAxis.Rotation = rotation;
            yAxis.Rotation = rotation;
            zAxis.Rotation = rotation;

            xyPlane.Rotation = rotation;
            yzPlane.Rotation = rotation;
            zxPlane.Rotation = rotation;
        }

        protected override void PostInput()
        {
            delta = Vector3.zero;

            if (Handles.MoveHandleSnapActive)
            {
                delta += Handles.SnapValue(xAxis.Delta, Handles.MoveSnapAmount) * GetXDir();
                delta += Handles.SnapValue(yAxis.Delta, Handles.MoveSnapAmount) * GetYDir();
                delta += Handles.SnapValue(zAxis.Delta, Handles.MoveSnapAmount) * GetZDir();

                delta += Handles.SnapValue(xyPlane.Delta.x, Handles.MoveSnapAmount) * GetXDir();
                delta += Handles.SnapValue(xyPlane.Delta.y, Handles.MoveSnapAmount) * GetYDir();
                delta += Handles.SnapValue(yzPlane.Delta.x, Handles.MoveSnapAmount) * GetYDir();
                delta += Handles.SnapValue(yzPlane.Delta.y, Handles.MoveSnapAmount) * GetZDir();
                delta += Handles.SnapValue(zxPlane.Delta.x, Handles.MoveSnapAmount) * GetZDir();
                delta += Handles.SnapValue(zxPlane.Delta.y, Handles.MoveSnapAmount) * GetXDir();
            }
            else
            {
                delta += xAxis.Delta * GetXDir();
                delta += yAxis.Delta * GetYDir();
                delta += zAxis.Delta * GetZDir();

                delta += xyPlane.Delta.x * GetXDir();
                delta += xyPlane.Delta.y * GetYDir();
                delta += yzPlane.Delta.x * GetYDir();
                delta += yzPlane.Delta.y * GetZDir();
                delta += zxPlane.Delta.x * GetZDir();
                delta += zxPlane.Delta.y * GetXDir();
            }
        }

        protected override void Draw()
        {
            HandleDrawing.SetTransform(Matrix4.TRS(Position, Rotation, Vector3.one));
            float handleSize = Handles.GetHandleSize(EditorApplication.SceneViewCamera, position);

            // Draw 1D arrows
            Color axisHover = new Color(0.8f, 0.8f, 0.8f, 1.0f);

            if (xAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.white);
            else if(xAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.red * axisHover);
            else
                HandleDrawing.SetColor(Color.red);

            Vector3 xConeStart = Vector3.xAxis*(1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(Vector3.zero, xConeStart, handleSize);
            HandleDrawing.DrawCone(xConeStart, Vector3.xAxis, CONE_HEIGHT, CONE_RADIUS, handleSize);

            if (yAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.white);
            else if (yAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.green * axisHover);
            else
                HandleDrawing.SetColor(Color.green);

            Vector3 yConeStart = Vector3.yAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(Vector3.zero, yConeStart, handleSize);
            HandleDrawing.DrawCone(yConeStart, Vector3.yAxis, CONE_HEIGHT, CONE_RADIUS, handleSize);

            if (zAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.white);
            else if (zAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.blue * axisHover);
            else
                HandleDrawing.SetColor(Color.blue);

            Vector3 zConeStart = Vector3.zAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(Vector3.zero, zConeStart, handleSize);
            HandleDrawing.DrawCone(zConeStart, Vector3.zAxis, CONE_HEIGHT, CONE_RADIUS, handleSize);

            // Draw 2D planes
            Color planeNormal = new Color(1.0f, 1.0f, 1.0f, 0.2f);
            Color planeHover = new Color(1.0f, 1.0f, 1.0f, 0.4f);
            Color planeActive = new Color(1.0f, 1.0f, 1.0f, 0.6f);

            Vector3 planeXOffset = Vector3.xAxis * 0.3f;
            Vector3 planeYOffset = Vector3.yAxis * 0.3f;
            Vector3 planeZOffset = Vector3.zAxis * 0.3f;

            //// XY plane
            HandleDrawing.SetColor(Color.blue);

            HandleDrawing.DrawLine(planeXOffset, planeXOffset + planeYOffset, handleSize);
            HandleDrawing.DrawLine(planeYOffset, planeYOffset + planeXOffset, handleSize);

            if (xyPlane.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.blue * planeActive);
            else if (xyPlane.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.blue * planeHover);
            else
                HandleDrawing.SetColor(Color.blue * planeNormal);

            Rect3 xyPlaneArea = new Rect3(
                (planeXOffset + planeYOffset) * 0.5f,
                new Vector3[] { Vector3.xAxis, Vector3.yAxis}, 
                new float[] { 0.15f, 0.15f});
            HandleDrawing.DrawRect(xyPlaneArea, handleSize);

            //// YZ plane
            HandleDrawing.SetColor(Color.red);

            HandleDrawing.DrawLine(planeYOffset, planeYOffset + planeZOffset, handleSize);
            HandleDrawing.DrawLine(planeZOffset, planeZOffset + planeYOffset, handleSize);

            if (yzPlane.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.red * planeActive);
            else if (yzPlane.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.red * planeHover);
            else
                HandleDrawing.SetColor(Color.red * planeNormal);

            Rect3 yzPlaneArea = new Rect3(
                (planeYOffset + planeZOffset) * 0.5f,
                new Vector3[] { Vector3.yAxis, Vector3.zAxis },
                new float[] { 0.15f, 0.15f });

            HandleDrawing.DrawRect(yzPlaneArea, handleSize);

            //// ZX plane
            HandleDrawing.SetColor(Color.green);

            HandleDrawing.DrawLine(planeZOffset, planeZOffset + planeXOffset, handleSize);
            HandleDrawing.DrawLine(planeXOffset, planeXOffset + planeZOffset, handleSize);

            if (zxPlane.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.green * planeActive);
            else if (zxPlane.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.green * planeHover);
            else
                HandleDrawing.SetColor(Color.green * planeNormal);

            Rect3 zxPlaneArea = new Rect3(
                (planeZOffset + planeXOffset) * 0.5f,
                new Vector3[] { Vector3.zAxis, Vector3.xAxis },
                new float[] { 0.15f, 0.15f });
            HandleDrawing.DrawRect(zxPlaneArea, handleSize);
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
    }
}
