using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public class HandleSliderPlane : HandleSlider
    {
        public HandleSliderPlane(Vector3 dir1, Vector3 dir2, float length, bool fixedScale = true, float snapValue = 0.0f)
        {
            Internal_CreateInstance(this, dir1, dir2, length, fixedScale, snapValue);
        }

        public float Delta
        {
            get
            {
                float value;
                Internal_GetDelta(mCachedPtr, out value);
                return value;
            }
        }

        public Vector3 DeltaDirection
        {
            get
            {
                Vector3 value;
                Internal_GetDeltaDirection(mCachedPtr, out value);
                return value;
            }
        }

        public Vector3 NewPosition
        {
            get
            {
                Vector3 value;
                Internal_GetNewPosition(mCachedPtr, out value);
                return value;
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(HandleSliderPlane instance, Vector3 dir1, Vector3 dir2, float length, bool fixedScale, float snapValue);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetDelta(IntPtr nativeInstance, out float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetDeltaDirection(IntPtr nativeInstance, out Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetNewPosition(IntPtr nativeInstance, out Vector3 value);
    }
}
