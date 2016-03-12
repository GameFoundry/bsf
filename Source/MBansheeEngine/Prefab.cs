//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /// <summary>
    /// Prefab is a saveable hierarchy of scene objects. In general it can serve as any grouping of scene objects 
    /// (for example a level) or be used as a form of a template instantiated and reused throughout the scene.
    /// </summary>
    public class Prefab : Resource
    {
        /// <summary>
        /// Constructor for internal use by the runtime.
        /// </summary>
        private Prefab()
        { }

        /// <summary>
        /// Creates a new prefab from the provided scene object. If the scene object has an existing prefab link it will 
        /// be broken. After the prefab is created the scene object will be automatically linked to it.
        /// </summary>
        /// <param name="so">Scene object to generate the prefab for.</param>
        public Prefab(SceneObject so)
        {
            IntPtr soPtr = so.GetCachedPtr();
            Internal_CreateInstance(this, soPtr);
        }

        /// <summary>
        /// Instantiates a prefab by creating an instance of the prefab's scene object hierarchy. The returned hierarchy 
        /// will be parented to world root by default.
        /// </summary>
        /// <returns>New scene object instance containing a copy of the prefab's hierarchy.</returns>
        public SceneObject Instantiate()
        {
            return Internal_Instantiate(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(Prefab instance, IntPtr so);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SceneObject Internal_Instantiate(IntPtr thisPtr);
    }
}
