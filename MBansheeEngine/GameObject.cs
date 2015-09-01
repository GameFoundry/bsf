using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// A base class for objects that can be part of the scene and referenced by other game objects.
    /// </summary>
    public class GameObject : ScriptObject
    {
        /// <summary>
        /// Returns a unique ID for the game object.
        /// </summary>
        public UInt64 InstanceId
        {
            get { return Internal_GetInstanceId(mCachedPtr); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UInt64 Internal_GetInstanceId(IntPtr thisPtr);
    }
}
