using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class Selection
    {
        public static Action<SceneObject[], string[]> OnSelectionChanged;
        public static Action<SceneObject> OnSceneObjectPing;
        public static Action<string> OnResourcePing;

        public static SceneObject[] sceneObjects
        {
            get
            {
                SceneObject[] selection;
                Internal_GetSceneObjectSelection(out selection);
                return selection;
            }
            set
            {
                Internal_SetSceneObjectSelection(value);
            }
        }

        public static string[] resourceUUIDs
        {
            get
            {
                string[] selection;
                Internal_GetResourceUUIDSelection(out selection);
                return selection;
            }
            set
            {
                Internal_SetResourceUUIDSelection(value);
            }
        }

        public static string[] resourcePaths
        {
            get
            {
                string[] selection;
                Internal_GetResourcePathSelection(out selection);
                return selection;
            }
            set
            {
                Internal_SetResourcePathSelection(value);
            }
        }

        public static void Ping(Resource resource)
        {
            string path = ProjectLibrary.GetPath(resource);

            Internal_PingResource(path);
        }

        public static void Ping(SceneObject so)
        {
            Internal_PingSceneObject(so);
        }

        private static void Internal_TriggerSelectionChanged(SceneObject[] objects, string[] paths)
        {
            if (OnSelectionChanged != null)
                OnSelectionChanged(objects, paths);
        }

        private static void Internal_TriggerSceneObjectPing(SceneObject so)
        {
            if (OnSceneObjectPing != null)
                OnSceneObjectPing(so);
        }

        private static void Internal_TriggerResourcePing(string path)
        {
            if (OnResourcePing != null)
                OnResourcePing(path);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_GetSceneObjectSelection(out SceneObject[] selection);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_SetSceneObjectSelection(SceneObject[] selection);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_GetResourceUUIDSelection(out string[] selection);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_SetResourceUUIDSelection(string[] selection);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_GetResourcePathSelection(out string[] selection);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_SetResourcePathSelection(string[] selection);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_PingResource(string resourcePath);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_PingSceneObject(SceneObject so);
    }
}
