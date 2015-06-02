using System;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class Handles
    {
        public static bool MoveHandleSnapActive
        {
            get { return EditorSettings.MoveHandleSnapActive; }
            set { EditorSettings.MoveHandleSnapActive = value; }
        }

        public static bool RotateHandleSnapActive
        {
            get { return EditorSettings.RotateHandleSnapActive; }
            set { EditorSettings.RotateHandleSnapActive = value; }
        }

        public static float MoveSnapAmount
        {
            get { return EditorSettings.MoveHandleSnapAmount; }
            set { EditorSettings.MoveHandleSnapAmount = value; }
        }

        public static Degree RotateSnapAmount
        {
            get { return EditorSettings.RotateHandleSnapAmount; }
            set { EditorSettings.RotateHandleSnapAmount = value; }
        }

        public static float SnapValue(float value, float snapAmount)
        {
            if (snapAmount > 0)
                return MathEx.RoundToInt(value / snapAmount) * snapAmount;

            return value;
        }

        public static Degree SnapValue(Degree value, Degree snapAmount)
        {
            return SnapValue(value.Degrees, snapAmount.Degrees);
        }

        public static float GetHandleSize(Camera camera, Vector3 position)
        {
            Vector3 cameraPos = camera.SceneObject.Position;

		    Vector3 diff = position - cameraPos;
		    float distAlongViewDir = Math.Abs(Vector3.Dot(diff, camera.SceneObject.Rotation.Forward));

            return distAlongViewDir * EditorSettings.DefaultHandleSize;
        }
    }
}
