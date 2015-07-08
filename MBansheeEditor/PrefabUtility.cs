using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public static class PrefabUtility
    {
        public static void BreakPrefab(SceneObject obj)
        {
            if (obj == null)
                return;

            IntPtr objPtr = obj.GetCachedPtr();
            Internal_BreakPrefab(objPtr);
        }

        public static void ApplyPrefab(SceneObject obj)
        {
            if (obj == null)
                return;

            IntPtr objPtr = obj.GetCachedPtr();
            Internal_ApplyPrefab(objPtr);
        }

        public static void RevertPrefab(SceneObject obj)
        {
            if (obj == null)
                return;

            IntPtr objPtr = obj.GetCachedPtr();
            Internal_RevertPrefab(objPtr);
        }

        public static bool HasPrefabLink(SceneObject obj)
        {
            if (obj == null)
                return false;

            IntPtr objPtr = obj.GetCachedPtr();
            return Internal_HasPrefabLink(objPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_BreakPrefab(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ApplyPrefab(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_RevertPrefab(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_HasPrefabLink(IntPtr nativeInstance);
    }
}
