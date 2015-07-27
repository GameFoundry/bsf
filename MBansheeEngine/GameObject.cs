using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public class GameObject : ScriptObject
    {
        public UInt64 InstanceId
        {
            get { return Internal_GetInstanceId(mCachedPtr); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UInt64 Internal_GetInstanceId(IntPtr thisPtr);
    }
}
