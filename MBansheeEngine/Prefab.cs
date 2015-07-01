using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    public class Prefab : Resource
    {
        // For internal use by the runtime
        private Prefab()
        { }

        public Prefab(SceneObject so)
        {
            IntPtr soPtr = so.GetCachedPtr();
            Internal_CreateInstance(this, soPtr);
        }

        public SceneObject Root
        {
            get { return Internal_GetRoot(mCachedPtr); }
        }

        public SceneObject Instantiate()
        {
            return Internal_Instantiate(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(Prefab instance, IntPtr so);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SceneObject Internal_GetRoot(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SceneObject Internal_Instantiate(IntPtr thisPtr);
    }
}
