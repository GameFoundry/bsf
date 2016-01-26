//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    /// <summary>
    /// Base class for all user-defined managed resources. Managed resources are automatically serialized, can be saved
    /// and persistently referenced by other objects.
    /// </summary>
    public class ManagedResource : Resource
    {
        /// <summary>
        /// Constructor for internal use by the runtime.
        /// </summary>
        protected ManagedResource()
        { }

        /// <summary>
        /// Creates a new managed resource.
        /// </summary>
        /// <typeparam name="T">Type of the managed resource to create.</typeparam>
        /// <returns>A new instance of a managed resource with default values.</returns>
        static public T Create<T>() where T : ManagedResource, new()
        {
            T newResource = new T();
            Internal_CreateInstance(newResource);

            return newResource;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(ManagedResource resource);
    }
}
