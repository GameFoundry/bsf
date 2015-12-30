using System;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Manages various global values relating to handles.
    /// </summary>
    public sealed class Handles
    {
        /// <summary>
        /// Determines if snapping for move handle is active. When active the move handle can only be moved in increments
        /// specified by <see cref="MoveSnapAmount"/>.
        /// </summary>
        public static bool MoveHandleSnapActive
        {
            get { return EditorSettings.MoveHandleSnapActive; }
            set { EditorSettings.MoveHandleSnapActive = value; }
        }

        /// <summary>
        /// Determines if snapping for rotate handle is active. When active the rotate handle can only be rotated in 
        /// increments specified by <see cref="RotateSnapAmount"/>.
        /// </summary>
        public static bool RotateHandleSnapActive
        {
            get { return EditorSettings.RotateHandleSnapActive; }
            set { EditorSettings.RotateHandleSnapActive = value; }
        }

        /// <summary>
        /// Determines size of the increments the move handle can be moved when <see cref="MoveHandleSnapActive"/> is
        /// active.
        /// </summary>
        public static float MoveSnapAmount
        {
            get { return EditorSettings.MoveHandleSnapAmount; }
            set { EditorSettings.MoveHandleSnapAmount = value; }
        }

        /// <summary>
        /// Determines size of the increments the rotate handle can be moved when <see cref="RotateHandleSnapActive"/> is
        /// active.
        /// </summary>
        public static Degree RotateSnapAmount
        {
            get { return EditorSettings.RotateHandleSnapAmount; }
            set { EditorSettings.RotateHandleSnapAmount = value; }
        }

        /// <summary>
        /// Snaps a value to the specified increments.
        /// </summary>
        /// <param name="value">Value to snap.</param>
        /// <param name="snapAmount">Increment to which to snap the value to.</param>
        /// <returns>Value snapped to the provided increments.</returns>
        public static float SnapValue(float value, float snapAmount)
        {
            if (snapAmount > 0)
                return MathEx.RoundToInt(value / snapAmount) * snapAmount;

            return value;
        }

        /// <summary>
        /// Snaps an angle value to the specified increments.
        /// </summary>
        /// <param name="value">Value to snap.</param>
        /// <param name="snapAmount">Increment to which to snap the value to.</param>
        /// <returns>Value snapped to the provided increments.</returns>
        public static Degree SnapValue(Degree value, Degree snapAmount)
        {
            return SnapValue(value.Degrees, snapAmount.Degrees);
        }

        /// <summary>
        /// Returns a scale that can be applied to a handle in order to keep it at constant size regardless of distance
        /// from the provided camera.
        /// </summary>
        /// <param name="camera">Camera through which the handle is being viewed.</param>
        /// <param name="position">Center of the handle.</param>
        /// <returns>Uniform scale to apply to the handle.</returns>
        public static float GetHandleSize(Camera camera, Vector3 position)
        {
            if (camera.ProjectionType == ProjectionType.Perspective)
            {
                Vector3 cameraPos = camera.SceneObject.Position;

                Vector3 diff = position - cameraPos;
                float distAlongViewDir = Math.Abs(Vector3.Dot(diff, camera.SceneObject.Rotation.Forward));

                return distAlongViewDir*EditorSettings.DefaultHandleSize;
            }
            else
            {
                return camera.OrthoHeight*EditorSettings.DefaultHandleSize;
            }
        }
    }
}
