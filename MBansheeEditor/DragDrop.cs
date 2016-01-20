//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Manages all drag and drop events within the engine. This doesn't include external OS drag and drop events.
    /// </summary>
    public static class DragDrop
    {
        /// <summary>
        /// Checks is a drag and drop operation in progress this frame.
        /// </summary>
        public static bool DragInProgress { get { return Internal_IsDragInProgress(); } }

        /// <summary>
        /// Checks has a drag and drop operation finished this frame.
        /// </summary>
        public static bool DropInProgress { get { return Internal_IsDropInProgress(); } }

        /// <summary>
        /// Data being currently dragged or dropped. Drag and drop must be currently in progress or have finished this 
        /// frame for this data to be valid.
        /// </summary>
        public static DragDropData Data { get { return Internal_GetData(); } }

        /// <summary>
        /// Type of data being currently dragged or dropped. 
        /// </summary>
        public static DragDropType Type { get { return Internal_GetDragType();} }

        /// <summary>
        /// Starts a new drag and drop operations. Drag and drop operation ends automatically when the mouse button is
        /// released.
        /// </summary>
        /// <param name="data">Data to drag around. This will be passed to the element that accepts the drop operation.</param>
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

    /// <summary>
    /// Contains data about a drag and drop operation.
    /// </summary>
    public class DragDropData : ScriptObject
    {
        internal DragDropType type;

        /// <summary>
        /// Determines what kind of drag and drop operation this data belongs to.
        /// </summary>
        public DragDropType Type { get { return type; } }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(DragDropData instance);
    }

    /// <summary>
    /// Drag and drop data containing one or multiple scene objects.
    /// </summary>
    public class SceneObjectDragDropData : DragDropData
    {
        /// <summary>
        /// Scene objects that are being dragged.
        /// </summary>
        public SceneObject[] Objects { get { return Internal_GetObjects(mCachedPtr); } }

        /// <summary>
        /// Creates a new scene drag and drop data.
        /// </summary>
        /// <param name="objects">A set of scene objects to drag.</param>
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

    /// <summary>
    /// Drag and drop data containing one or multiple resource paths.
    /// </summary>
    public class ResourceDragDropData : DragDropData
    {
        /// <summary>
        /// Absolute resource paths that are being dragged.
        /// </summary>
        public string[] Paths { get { return Internal_GetPaths(mCachedPtr); } }

        /// <summary>
        /// Creates a new resource drag and drop data.
        /// </summary>
        /// <param name="paths">A set of absolute resource paths to drag.</param>
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

    /// <summary>
    /// Types of available drag and drop operations.
    /// </summary>
    public enum DragDropType // Note: Must match C++ enum ScriptDragDropType
    {
        Resource,
        SceneObject,
        None
    }
}
