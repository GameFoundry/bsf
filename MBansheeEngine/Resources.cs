using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public static class Resources
    {
        public static T Load<T>(string path) where T : Resource
        {
            return (T)Internal_Load(path);
        }

        public static void Unload(Resource resource)
        {
            if (resource != null)
                Internal_Unload(resource.GetCachedPtr());
        }

        public static void UnloadUnused()
        {
            Internal_UnloadUnused();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Resource Internal_Load(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Unload(IntPtr resourcePtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UnloadUnused();
    }
}
