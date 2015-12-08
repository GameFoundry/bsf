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
        /// if loading using the project library.
        /// </summary>
        /// <typeparam name="T">Type of the resource.</typeparam>
        /// <param name="path">Path of the resource, relative to game directory. If running from editor this will be
        ///                    the same location as resource location in the project library.</param>
        /// <returns>Loaded resource, or null if resource cannot be found.</returns>
        public static T Load<T>(string path) where T : Resource
        {
            return (T)Internal_Load(path);
        }

        /// <summary>
        /// Unloads all resources that are no longer referenced. Usually the system keeps resources in memory even if
        /// they are no longer referenced to avoid constant loading/unloading if resource is often passed around.
        /// </summary>
        public static void UnloadUnused()
        {
            Internal_UnloadUnused();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Resource Internal_Load(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UnloadUnused();
    }
}
