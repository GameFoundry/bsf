using BansheeEngine;

namespace BansheeEditor
{
    public sealed class Handles
    {
        // TODO - Implement these properly by retrieving them from some user-toggled properly
        public static bool MoveHandleSnapActive { get { return true; } }
        public static bool RotateHandleSnapActive { get { return true; } }
        public static bool ScaleHandleSnapActive { get { return true; } }

        public static float MoveSnapAmount { get { return 1.0f; /* TODO */ } }
        public static float RotateSnapAmount { get { return 1.0f; /* TODO */ } }
        public static float ScaleSnapAmount { get { return 1.0f; /* TODO */ } }

        public static float SnapValue(float value, float snapAmount)
        {
            int intValue = MathEx.FloorToInt(value/snapAmount);

            return value - intValue*snapAmount;
        }

        public static float GetHandleSize(Camera camera, Vector3 position)
        {
            // TODO 

            return 1;
        }
    }
}
