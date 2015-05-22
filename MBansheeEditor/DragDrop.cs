using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public static class DragDrop
    {
        public static bool DragInProgress { get { return Internal_IsDragInProgress(); } }
        public static bool DropInProgress { get { return Internal_IsDropInProgress(); } }
        public static DragDropData Data { get { return Internal_GetData(); } }
        public static DragDropType Type { get { return Internal_GetDragType();} }

        public static void StartDrag(DragDropData data)
        {
            if (data is SceneObjectDragDropData)
                Internal_StartSceneObjectDrag(data.GetCachedPtr());
            else if (data is ResourceDragDropData)
                Internal_StartResourceDrag(data.GetCachedPtr());
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsDragInProgress();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsDropInProgress();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern DragDropData Internal_GetData();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern DragDropType Internal_GetDragType();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_StartSceneObjectDrag(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_StartResourceDrag(IntPtr instance);
    }

    public class DragDropData : ScriptObject
    {
        internal DragDropType type;

        public DragDropType Type { get { return type; } }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(DragDropData instance);
    }

    public class SceneObjectDragDropData : DragDropData
    {
        public SceneObject[] Objects { get { return Internal_GetObjects(mCachedPtr); } }

        public SceneObjectDragDropData(SceneObject[] objects)
        {
            this.type = DragDropType.SceneObject;

            Internal_CreateInstance(this, objects);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SceneObjectDragDropData instance, SceneObject[] objects);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SceneObject[] Internal_GetObjects(IntPtr thisPtr);
    }

    public class ResourceDragDropData : DragDropData
    {
        public string[] Paths { get { return Internal_GetPaths(mCachedPtr); } }

        public ResourceDragDropData(string[] paths)
        {
            this.type = DragDropType.Resource;

            Internal_CreateInstance(this, paths);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(ResourceDragDropData instance, string[] paths);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string[] Internal_GetPaths(IntPtr thisPtr);
    }

    // Note: Must match C++ enum ScriptDragDropType
    public enum DragDropType
    {
        Resource,
        SceneObject,
        None
    }
}
