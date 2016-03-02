//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Allows you to store a reference to a resource without needing to have that resource loaded.
    /// </summary>
    public class ResourceRef : ScriptObject
    {
        /// <summary>
        /// Constructor for internal use only.
        /// </summary>
        protected ResourceRef()
        { }

        /// <summary>
        /// Checks is the referenced resource loaded.
        /// </summary>
        public bool IsLoaded
        {
            get { return Internal_IsLoaded(mCachedPtr); }
        }

        /// <inheritdoc/>
        public override bool Equals(object other)
        {
            if (!(other is ResourceRef))
                return false;

            ResourceRef otherRef = (ResourceRef)other;
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
}
