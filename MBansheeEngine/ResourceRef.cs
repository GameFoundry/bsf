using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Allows you to store a reference to a resource without needing to have that resource loaded.
    /// </summary>
    public class ResourceRef<T> : ScriptObject where T : Resource
    {
        /// <summary>
        /// Constructor for internal use only.
        /// </summary>
        private ResourceRef()
        { }

        /// <summary>
        /// Checks is the referenced resource loaded
        /// </summary>
        public bool IsLoaded
        {
            get { return Internal_IsLoaded(mCachedPtr); }
        }

        /// <summary>
        /// Retrieves the referenced resource. This will load the resources if it is not already loaded.
        /// </summary>
        /// <typeparam name="T">Type of the resource to load.</typeparam>
        /// <returns>Loaded resource object.</returns>
        public T Get()
        {
            return (T)Internal_GetResource(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsLoaded(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Resource Internal_GetResource(IntPtr thisPtr);
    }
}
