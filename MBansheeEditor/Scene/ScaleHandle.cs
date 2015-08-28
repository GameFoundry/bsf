using BansheeEngine;

namespace BansheeEditor
{
    public sealed class ScaleHandle : DefaultHandle
    {
        private const float SMALL_CUBE_SIZE = 0.175f;
        private const float CENTER_CUBE_SIZE = 0.33f;

        private Vector3 delta;

        private HandleSliderLine xAxis;
        private HandleSliderLine yAxis;
        private HandleSliderLine zAxis;

        private HandleSliderPlane freeAxis;

        public Vector3 Delta
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

        public ScaleHandle()
        {
            xAxis = new HandleSliderLine(this, Vector3.XAxis, 1.0f);
            yAxis = new HandleSliderLine(this, Vector3.YAxis, 1.0f);
            zAxis = new HandleSliderLine(this, Vector3.ZAxis, 1.0f);

            freeAxis = new HandleSliderPlane(this, Vector3.XAxis, Vector3.YAxis, 0.4f);
        }

        protected override void PreInput()
        {
            xAxis.Position = position;
            yAxis.Position = position;
            zAxis.Position = position;

            xAxis.Rotation = rotation;
            yAxis.Rotation = rotation;
            zAxis.Rotation = rotation;

            float handleSize = Handles.GetHandleSize(EditorApplication.SceneViewCamera, position);
            Vector3 freeAxisOffset = (Vector3.XAxis * -0.2f + Vector3.YAxis * -0.2f) * handleSize;
            freeAxis.Rotation = EditorApplication.SceneViewCamera.SceneObject.Rotation;
            freeAxis.Position = position + freeAxis.Rotation.Rotate(freeAxisOffset);
        }

        protected override void PostInput()
        {
            delta = Vector3.Zero;

            delta += xAxis.Delta * GetXDir() * 0.1f;
            delta += yAxis.Delta * GetYDir() * 0.1f;
            delta += zAxis.Delta * GetZDir() * 0.1f;
            delta += (freeAxis.Delta.x + freeAxis.Delta.y) * Vector3.One * 0.1f;
        }

        protected override void Draw()
        {
            HandleDrawing.SetTransform(Matrix4.TRS(Position, Rotation, Vector3.One));
            float handleSize = Handles.GetHandleSize(EditorApplication.SceneViewCamera, position);

            // Draw 1D sliders
            Vector3 smallCubeExtents = new Vector3(SMALL_CUBE_SIZE*0.5f, SMALL_CUBE_SIZE*0.5f, SMALL_CUBE_SIZE*0.5f);

            if (xAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.White);
            else if (xAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.BansheeOrange);
            else
                HandleDrawing.SetColor(Color.Red);

            Vector3 xCubeOffset = Vector3.XAxis * SMALL_CUBE_SIZE * 0.5f;
            Vector3 xCubeStart = Vector3.XAxis - xCubeOffset;
            
            HandleDrawing.DrawLine(Vector3.Zero, xCubeStart, handleSize);
            HandleDrawing.DrawCube(xCubeStart + xCubeOffset, smallCubeExtents, handleSize);

            if (yAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.White);
            else if (yAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.BansheeOrange);
            else
                HandleDrawing.SetColor(Color.Green);

            Vector3 yCubeOffset = Vector3.YAxis * SMALL_CUBE_SIZE * 0.5f;
            Vector3 yCubeStart = Vector3.YAxis - yCubeOffset;

            HandleDrawing.DrawLine(Vector3.Zero, yCubeStart, handleSize);
            HandleDrawing.DrawCube(yCubeStart + yCubeOffset, smallCubeExtents, handleSize);

            if (zAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.White);
            else if (zAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.BansheeOrange);
            else
                HandleDrawing.SetColor(Color.Blue);

            Vector3 zCubeOffset = Vector3.ZAxis * SMALL_CUBE_SIZE * 0.5f;
            Vector3 zCubeStart = Vector3.ZAxis - zCubeOffset;

            HandleDrawing.DrawLine(Vector3.Zero, zCubeStart, handleSize);
            HandleDrawing.DrawCube(zCubeStart + zCubeOffset, smallCubeExtents, handleSize);

            // Draw free scale handle
            if (freeAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.SetColor(Color.White);
            else if (freeAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.SetColor(Color.BansheeOrange);
            else
                HandleDrawing.SetColor(Color.White);

            //// Rotate it so it always faces the camera, and move it forward a bit to always render in front
            Vector3 bottomLeft = -Vector3.XAxis * 0.2f - Vector3.YAxis * 0.2f;
            Vector3 topLeft = -Vector3.XAxis * 0.2f + Vector3.YAxis * 0.2f;
            Vector3 topRight = Vector3.XAxis * 0.2f + Vector3.YAxis * 0.2f;
            Vector3 bottomRight = Vector3.XAxis * 0.2f - Vector3.YAxis * 0.2f;

            Vector3 offset = Vector3.ZAxis*0.1f;

            Quaternion cameraRot = EditorApplication.SceneViewCamera.SceneObject.Rotation;
            bottomLeft = cameraRot.Rotate(bottomLeft + offset);
            topLeft = cameraRot.Rotate(topLeft + offset);
            topRight = cameraRot.Rotate(topRight + offset);
            bottomRight = cameraRot.Rotate(bottomRight + offset);

            HandleDrawing.DrawLine(bottomLeft, bottomRight, handleSize);
            HandleDrawing.DrawLine(bottomLeft, topLeft, handleSize);
            HandleDrawing.DrawLine(topLeft, topRight, handleSize);
            HandleDrawing.DrawLine(bottomRight, topRight, handleSize);
        }

        private Vector3 GetXDir()
        {
            return rotation.Rotate(Vector3.XAxis);
        }

        private Vector3 GetYDir()
        {
            return rotation.Rotate(Vector3.YAxis);
        }

        private Vector3 GetZDir()
        {
            return rotation.Rotate(Vector3.ZAxis);
        }
    }
}
