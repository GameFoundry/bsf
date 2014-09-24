using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public sealed class Selection
    {
        public SceneObject[] sceneObjects
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

        public string[] resourceUUIDs
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

        public string[] resourcePaths
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
    }
}
