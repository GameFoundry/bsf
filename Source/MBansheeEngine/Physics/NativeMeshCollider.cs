//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Wrapper around the native MeshCollider class.
    /// <see cref="MeshCollider"/>
    /// </summary>
    internal class NativeMeshCollider : NativeCollider
    {
        public NativeMeshCollider()
        {
            Internal_CreateInstance(this);
        }

        public PhysicsMesh Mesh
        {
            get { return Internal_GetMesh(mCachedPtr); }
            set
            {
                IntPtr meshPtr = IntPtr.Zero;
                if (value != null)
                    meshPtr = value.GetCachedPtr();

                Internal_SetMesh(mCachedPtr, meshPtr);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(NativeMeshCollider instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PhysicsMesh Internal_GetMesh(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMesh(IntPtr thisPtr, IntPtr mesh);
    }
}
