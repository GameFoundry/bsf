using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Handle that allows an object to be translated along the three primary axes.
    /// </summary>
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

        /// <summary>
        /// Returns the amount of translation since last frame. Only valid while the handle is being dragged.
        /// </summary>
        public Vector3 Delta
        {
            get { return delta; }
        }

        /// <inheritdoc/>
        internal override bool IsDragged()
        {
            return xAxis.State == HandleSlider.StateType.Active ||
                    yAxis.State == HandleSlider.StateType.Active ||
                    zAxis.State == HandleSlider.StateType.Active ||
                    xyPlane.State == HandleSlider.StateType.Active ||
                    yzPlane.State == HandleSlider.StateType.Active ||
                    zxPlane.State == HandleSlider.StateType.Active;
        }

        /// <summary>
        /// Creates a new move handle.
        /// </summary>
        public MoveHandle()
        {
            xAxis = new HandleSliderLine(this, Vector3.XAxis, 1.0f);
            yAxis = new HandleSliderLine(this, Vector3.YAxis, 1.0f);
            zAxis = new HandleSliderLine(this, Vector3.ZAxis, 1.0f);

            xyPlane = new HandleSliderPlane(this, Vector3.XAxis, Vector3.YAxis, 0.3f);
            yzPlane = new HandleSliderPlane(this, Vector3.YAxis, Vector3.ZAxis, 0.3f);
            zxPlane = new HandleSliderPlane(this, Vector3.ZAxis, Vector3.XAxis, 0.3f);
        }

        /// <inheritdoc/>
        protected internal override void PreInput()
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

        /// <inheritdoc/>
        protected internal override void PostInput()
        {
            delta = Vector3.Zero;

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

        /// <inheritdoc/>
        protected internal override void Draw()
        {
            HandleDrawing.Layer = 1;
            HandleDrawing.Transform = Matrix4.TRS(Position, Rotation, Vector3.One);
            float handleSize = Handles.GetHandleSize(EditorApplication.SceneViewCamera, position);

            // Draw 1D arrows
            if (xAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.Color = Color.White;
            else if(xAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.Color = Color.BansheeOrange;
            else
                HandleDrawing.Color = Color.Red;

            Vector3 xConeStart = Vector3.XAxis*(1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(Vector3.Zero, xConeStart, handleSize);
            HandleDrawing.DrawCone(xConeStart, Vector3.XAxis, CONE_HEIGHT, CONE_RADIUS, handleSize);

            if (yAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.Color = Color.White;
            else if (yAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.Color = Color.BansheeOrange;
            else
                HandleDrawing.Color = Color.Green;

            Vector3 yConeStart = Vector3.YAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(Vector3.Zero, yConeStart, handleSize);
            HandleDrawing.DrawCone(yConeStart, Vector3.YAxis, CONE_HEIGHT, CONE_RADIUS, handleSize);

            if (zAxis.State == HandleSlider.StateType.Active)
                HandleDrawing.Color = Color.White;
            else if (zAxis.State == HandleSlider.StateType.Hover)
                HandleDrawing.Color = Color.BansheeOrange;
            else
                HandleDrawing.Color = Color.Blue;

            Vector3 zConeStart = Vector3.ZAxis * (1.0f - CONE_HEIGHT);
            HandleDrawing.DrawLine(Vector3.Zero, zConeStart, handleSize);
            HandleDrawing.DrawCone(zConeStart, Vector3.ZAxis, CONE_HEIGHT, CONE_RADIUS, handleSize);

            // Draw 2D planes
            Color planeNormal = new Color(1.0f, 1.0f, 1.0f, 0.2f);
            Color planeHover = new Color(1.0f, 1.0f, 1.0f, 0.4f);
            Color planeActive = new Color(1.0f, 1.0f, 1.0f, 0.6f);

            Vector3 planeXOffset = Vector3.XAxis * 0.3f;
            Vector3 planeYOffset = Vector3.YAxis * 0.3f;
            Vector3 planeZOffset = Vector3.ZAxis * 0.3f;

            //// XY plane
            HandleDrawing.Color = Color.Blue;

            HandleDrawing.DrawLine(planeXOffset, planeXOffset + planeYOffset, handleSize);
            HandleDrawing.DrawLine(planeYOffset, planeYOffset + planeXOffset, handleSize);

            if (xyPlane.State == HandleSlider.StateType.Active)
                HandleDrawing.Color = Color.Blue * planeActive;
            else if (xyPlane.State == HandleSlider.StateType.Hover)
                HandleDrawing.Color = Color.Blue * planeHover;
            else
                HandleDrawing.Color = Color.Blue * planeNormal;

            Rect3 xyPlaneArea = new Rect3(
                (planeXOffset + planeYOffset) * 0.5f,
                new Vector3[] { Vector3.XAxis, Vector3.YAxis}, 
                new float[] { 0.15f, 0.15f});
            HandleDrawing.DrawRect(xyPlaneArea, handleSize);

            //// YZ plane
            HandleDrawing.Color = Color.Red;

            HandleDrawing.DrawLine(planeYOffset, planeYOffset + planeZOffset, handleSize);
            HandleDrawing.DrawLine(planeZOffset, planeZOffset + planeYOffset, handleSize);

            if (yzPlane.State == HandleSlider.StateType.Active)
                HandleDrawing.Color = Color.Red * planeActive;
            else if (yzPlane.State == HandleSlider.StateType.Hover)
                HandleDrawing.Color = Color.Red * planeHover;
            else
                HandleDrawing.Color = Color.Red * planeNormal;

            Rect3 yzPlaneArea = new Rect3(
                (planeYOffset + planeZOffset) * 0.5f,
                new Vector3[] { Vector3.YAxis, Vector3.ZAxis },
                new float[] { 0.15f, 0.15f });

            HandleDrawing.DrawRect(yzPlaneArea, handleSize);

            //// ZX plane
            HandleDrawing.Color = Color.Green;

            HandleDrawing.DrawLine(planeZOffset, planeZOffset + planeXOffset, handleSize);
            HandleDrawing.DrawLine(planeXOffset, planeXOffset + planeZOffset, handleSize);

            if (zxPlane.State == HandleSlider.StateType.Active)
                HandleDrawing.Color = Color.Green * planeActive;
            else if (zxPlane.State == HandleSlider.StateType.Hover)
                HandleDrawing.Color = Color.Green * planeHover;
            else
                HandleDrawing.Color = Color.Green * planeNormal;

            Rect3 zxPlaneArea = new Rect3(
                (planeZOffset + planeXOffset) * 0.5f,
                new Vector3[] { Vector3.ZAxis, Vector3.XAxis },
                new float[] { 0.15f, 0.15f });
            HandleDrawing.DrawRect(zxPlaneArea, handleSize);
        }

        /// <summary>
        /// Returns the direction of the handle's x axis in world space.
        /// </summary>
        /// <returns>Direction of the handle's x axis in world space</returns>
        private Vector3 GetXDir()
        {
             return rotation.Rotate(Vector3.XAxis);
        }

        /// <summary>
        /// Returns the direction of the handle's y axis in world space.
        /// </summary>
        /// <returns>Direction of the handle's y axis in world space</returns>
        private Vector3 GetYDir()
        {
            return rotation.Rotate(Vector3.YAxis);
        }

        /// <summary>
        /// Returns the direction of the handle's z axis in world space.
        /// </summary>
        /// <returns>Direction of the handle's z axis in world space</returns>
        private Vector3 GetZDir()
        {
            return rotation.Rotate(Vector3.ZAxis);
        }
    }
}
