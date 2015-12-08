using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Common class for all resource references. <see cref="ResourceRef{T}"/>
    /// </summary>
    public class ResourceRefBase : ScriptObject
    {
        /// <summary>
        /// Constructor for internal use only.
        /// </summary>
        protected ResourceRefBase()
        { }

        /// <summary>
        /// Checks is the referenced resource loaded
        /// </summary>
        public bool IsLoaded
        {
            get { return Internal_IsLoaded(mCachedPtr); }
        }

        /// <inheritdoc/>
        public override bool Equals(object other)
        {
            if (!(other is ResourceRefBase))
                return false;

            ResourceRefBase otherRef = (ResourceRefBase)other;
            return Internal_GetUUID(mCachedPtr).Equals(Internal_GetUUID(otherRef.mCachedPtr));
        }

        /// <inheritdoc/>
        public override int GetHashCode()
        {
            return Internal_GetUUID(mCachedPtr).GetHashCode();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsLoaded(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        protected static extern Resource Internal_GetResource(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetUUID(IntPtr thisPtr);
    }

    /// <summary>
    /// Allows you to store a reference to a resource without needing to have that resource loaded.
    /// </summary>
    public class ResourceRef<T> : ResourceRefBase where T : Resource
    {
        /// <summary>
        /// Retrieves the referenced resource. This will load the resources if it is not already loaded.
        /// </summary>
        /// <typeparam name="T">Type of the resource to load.</typeparam>
        /// <returns>Loaded resource object.</returns>
        public T Get()
        {
            return (T)Internal_GetResource(mCachedPtr);
        }
    }
}
