using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class HandleSliderPlane : HandleSlider
    {
        public HandleSliderPlane(Handle parentHandle, Vector3 dir1, Vector3 dir2, float length, bool fixedScale = true)
            :base(parentHandle)
        {
            Internal_CreateInstance(this, dir1, dir2, length, fixedScale);
        }

        public Vector2 Delta
        {
            get
            {
                Vector2 value;
                Internal_GetDelta(mCachedPtr, out value);
                return value;
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(HandleSliderPlane instance, Vector3 dir1, Vector3 dir2, float length, bool fixedScale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetDelta(IntPtr nativeInstance, out Vector2 value);
    }
}
