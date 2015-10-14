using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Base class for all resources. Resources can be persistently referenced by scene objects or other resources.
    /// </summary>
    public class Resource : ScriptObject
    {
        /// <summary>
        /// Name of the resource. Use primarily for easier identification and not important to the engine itself.
        /// </summary>
        public string Name
        {
            get { return Internal_GetName(mCachedPtr); }
        }

        /// <summary>
        /// Returns a universally unique identifier of this resource.
        /// </summary>
        public string UUID
        {
            get { return Internal_GetUUID(mCachedPtr); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetName(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetUUID(IntPtr nativeInstance);
    }
}
