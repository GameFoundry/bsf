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

        public Vector3 Delta
        {
            get { return delta; }
        }

        internal override bool IsDragged()
        {
            return xAxis.State == HandleSlider.StateType.Active ||
                    yAxis.State == HandleSlider.StateType.Active ||
                    zAxis.State == HandleSlider.StateType.Active;
        }

        public MoveHandle()
        {
            xAxis = new HandleSliderLine(this, Vector3.xAxis, 1.0f);
            yAxis = new HandleSliderLine(this, Vector3.yAxis, 1.0f);
            zAxis = new HandleSliderLine(this, Vector3.zAxis, 1.0f);
        }

        protected override void PreInput()
        {
            xAxis.Position = position;
            yAxis.Position = position;
            zAxis.Position = position;

            xAxis.Rotation = rotation;
            yAxis.Rotation = rotation;
            zAxis.Rotation = rotation;
        }

        protected override void PostInput()
        {
            delta = Vector3.zero;

            if (Handles.MoveHandleSnapActive)
            {
                delta += Handles.SnapValue(xAxis.Delta, Handles.MoveSnapAmount) * GetXDir();
                delta += Handles.SnapValue(yAxis.Delta, Handles.MoveSnapAmount) * GetYDir();
                delta += Handles.SnapValue(zAxis.Delta, Handles.MoveSnapAmount) * GetZDir();
            }
            else
            {
                delta += xAxis.Delta * GetXDir();
                delta += yAxis.Delta * GetYDir();
                delta += zAxis.Delta * GetZDir();
            }
        }

        protected override void Draw()
        {
            HandleDrawing.SetTransform(Matrix4.TRS(Position, Rotation, Vector3.one));

            if (xAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.white);
            else if(xAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.red * 0.8f);
            else
                HandleDrawing.SetColor(Color.red);

            float handleSize = Handles.GetHandleSize(EditorApplication.SceneViewCamera, position);

            Vector3 xConeStart = Vector3.xAxis*(1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(Vector3.zero, xConeStart, handleSize);
            HandleDrawing.DrawCone(xConeStart, Vector3.xAxis, CONE_HEIGHT, CONE_RADIUS, handleSize);

            if (yAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.white);
            else if (yAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.green * 0.8f);
            else
                HandleDrawing.SetColor(Color.green);

            Vector3 yConeStart = Vector3.yAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(Vector3.zero, yConeStart, handleSize);
            HandleDrawing.DrawCone(yConeStart, Vector3.yAxis, CONE_HEIGHT, CONE_RADIUS, handleSize);

            if (zAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.white);
            else if (zAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.blue * 0.8f);
            else
                HandleDrawing.SetColor(Color.blue);

            Vector3 zConeStart = Vector3.zAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(Vector3.zero, zConeStart, handleSize);
            HandleDrawing.DrawCone(zConeStart, Vector3.zAxis, CONE_HEIGHT, CONE_RADIUS, handleSize);
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
