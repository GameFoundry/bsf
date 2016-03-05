//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Handles scene object and resource selection. Triggeres events when selection changes and allows the user to query
    /// current selection state.
    /// </summary>
    public sealed class Selection
    {
        /// <summary>
        /// Triggered whenever scene object or resource selection changes. The provided parameters will contain the newly 
        /// selected objects/resource paths.
        /// </summary>
        public static Action<SceneObject[], string[]> OnSelectionChanged;

        /// <summary>
        /// Triggered when a resource ping is requested. Ping usually means the object will be highlighted in its respective
        /// editors.
        /// </summary>
        internal static Action<string> OnResourcePing;

        /// <summary>
        /// Currently selected scene object. If more than one object is selected, this returns the first one.
        /// </summary>
        public static SceneObject SceneObject
        {
            get
            {
                SceneObject[] selection;
                Internal_GetSceneObjectSelection(out selection);

                if(selection.Length > 0)
                    return selection[0];

                return null;
            }
            set
            {
                Internal_SetSceneObjectSelection(new SceneObject[] { value });
            }
        }

        /// <summary>
        /// Currently selected set of scene objects.
        /// </summary>
        public static SceneObject[] SceneObjects
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

        /// <summary>
        /// Currently selected set of resource UUIDs.
        /// </summary>
        public static string[] ResourceUUIDs
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

        /// <summary>
        /// Currently selected set of resource paths.
        /// </summary>
        public static string[] ResourcePaths
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

        /// <summary>
        /// Pings the resource, highlighting it in its respective editors.
        /// </summary>
        /// <param name="resource">Resource to highlight.</param>
        public static void Ping(Resource resource)
        {
            string path = ProjectLibrary.GetPath(resource);

            Internal_PingResource(path);
        }

        /// <summary>
        /// Pings the scene object, highlighting it in its respective editors.
        /// </summary>
        /// <param name="so">Scene object to highlight.</param>
        public static void Ping(SceneObject so)
        {
            Internal_PingSceneObject(so);
        }

        /// <summary>
        /// Triggered internally by the runtime when the selection changes.
        /// </summary>
        /// <param name="objects">Set of newly selected scene objects.</param>
        /// <param name="paths">Set of newly selected resource paths.</param>
        private static void Internal_TriggerSelectionChanged(SceneObject[] objects, string[] paths)
        {
            if (OnSelectionChanged != null)
                OnSelectionChanged(objects, paths);
        }

        /// <summary>
        /// Triggered internally by the runtime when the scene object ping is triggered.
        /// </summary>
        /// <param name="so">Scene object to highlight.</param>
        private static void Internal_TriggerSceneObjectPing(SceneObject so)
        {
            // Ignoring this until something needs this event
        }

        /// <summary>
        /// Triggered internally by the runtime when the resource ping is triggered.
        /// </summary>
        /// <param name="path">Path to the resource to highlight.</param>
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
