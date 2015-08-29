using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    internal class ProfilerOverlayInternal : ScriptObject
    {
        internal ProfilerOverlayInternal(Camera camera)
        {
            IntPtr ptr = IntPtr.Zero;
            if (camera != null)
                ptr = camera.Native.GetCachedPtr();

            Internal_CreateInstance(this, ptr);
        }

        public void SetType(ProfilerOverlayType type)
        {
            Internal_SetType(mCachedPtr, type);
        }

        internal void Update()
        {
            Internal_Update(mCachedPtr);
        }

        internal void Destroy()
        {
            Internal_DestroyInstance(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(ProfilerOverlayInternal instance, IntPtr camera);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetType(IntPtr instance, ProfilerOverlayType type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Update(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DestroyInstance(IntPtr instance);
    }
}
