//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Handles dynamic loading of resources during runtime. 
    /// </summary>
    public static class Resources
    {
        /// <summary>
        /// Loads a resource at the specified path. If running outside of the editor you must make sure to mark that 
        /// the resource gets included in the build. If running inside the editor this has similar functionality as
        /// if loading using the project library. If resource is already loaded an existing instance is returned.
        /// 
        /// All resources are automatically unloaded when they go out of scope unless <see cref="keepLoaded"/> parameter
        /// is specified. 
        /// </summary>
        /// <typeparam name="T">Type of the resource.</typeparam>
        /// <param name="path">Path of the resource, relative to game directory. If running from editor this will be
        ///                    the same location as resource location in the project library.</param>
        /// <param name="keepLoaded">If true the system will keep the resource loaded even when it goes out of scope.
        ///                          You must call <see cref="Release(Resource)"/> in order to allow the resource to be
        ///                          unloaded (it must be called once for each corresponding load). </param>
        /// <returns>Loaded resource, or null if resource cannot be found.</returns>
        public static T Load<T>(string path, bool keepLoaded = true) where T : Resource
        {
            return (T)Internal_Load(path, keepLoaded);
        }

        /// <summary>
        /// Loads a resource referenced by the provided reference. If running outside of the editor you must make sure 
        /// to mark that the resource gets included in the build. If running inside the editor this has similar functionality 
        /// as if loading using the project library. If resource is already loaded an existing instance is returned.
        ///
        /// All resources are automatically unloaded when they go out of scope unless <see cref="keepLoaded"/> parameter
        /// is specified.
        /// </summary>
        /// <typeparam name="T">Type of the resource.</typeparam>
        /// <param name="reference">Reference to the resource to load.</param>
        /// <param name="keepLoaded">If true the system will keep the resource loaded even when it goes out of scope.
        ///                          You must call <see cref="Release(ResourceRef)"/> in order to allow the resource to be
        ///                          unloaded (it must be called once for each corresponding load). </param> 
        /// <returns>Loaded resource, or null if resource cannot be found.</returns>
        public static T Load<T>(ResourceRef reference, bool keepLoaded = true) where T : Resource
        {
            return (T)Internal_LoadRef(reference, keepLoaded);
        }

        /// <summary>
        /// Releases an internal reference to the resource held by the resources system. This allows the resource
        ///	to be unloaded when it goes out of scope, if the resource was loaded with "keepLoaded" parameter.
        ///
        /// Alternatively you can also skip manually calling <see cref="Release(ResourceRef)"/> and call <see cref="UnloadUnused"/> 
        /// which will unload all resources that do not have any external references, but you lose the fine grained control 
        /// of what will be unloaded.
        /// </summary>
        /// <param name="resource">Resource to release</param>
        public static void Release(ResourceRef resource)
        {
            if (resource == null)
                return;

            Internal_ReleaseRef(resource.GetCachedPtr());
        }

        /// <summary>
        /// Releases an internal reference to the resource held by the resources system. This allows the resource
        ///	to be unloaded when it goes out of scope, if the resource was loaded with "keepLoaded" parameter.
        ///
        /// Alternatively you can also skip manually calling <see cref="Release(Resource)"/> and call <see cref="UnloadUnused"/> 
        /// which will unload all resources that do not have any external references, but you lose the fine grained control 
        /// of what will be unloaded.
        /// </summary>
        /// <param name="resource">Resource to release</param>
        public static void Release(Resource resource)
        {
            if(resource == null)
                return;

            Internal_Release(resource.GetCachedPtr());
        }

        /// <summary>
        /// Unloads all resources that are no longer referenced. This only applies to resources loaded with "keepLoaded"
        /// parameter, as all other resources will be loaded when they go out of scope.
        /// </summary>
        public static void UnloadUnused()
        {
            Internal_UnloadUnused();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Resource Internal_Load(string path, bool keepLoaded);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Resource Internal_LoadRef(ResourceRef reference, bool keepLoaded);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Release(IntPtr resource);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ReleaseRef(IntPtr resource);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UnloadUnused();
    }
}
