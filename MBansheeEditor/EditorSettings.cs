//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Contains various settings that are applied globally to the editor. Settings will persist through editor sessions.
    /// </summary>
    internal static class EditorSettings
    {
        /// <summary>
        /// Determines if snapping for move handle is active. When active the move handle can only be moved in increments
        /// specified by <see cref="MoveHandleSnapAmount"/>.
        /// </summary>
        public static bool MoveHandleSnapActive
        {
            get { return Internal_GetMoveHandleSnapActive(); }
            set { Internal_SetMoveHandleSnapActive(value); }
        }

        /// <summary>
        /// Determines if snapping for rotate handle is active. When active the rotate handle can only be rotated in 
        /// increments specified by <see cref="RotateHandleSnapAmount"/>.
        /// </summary>
        public static bool RotateHandleSnapActive
        {
            get { return Internal_GetRotateHandleSnapActive(); }
            set { Internal_SetRotateHandleSnapActive(value); }
        }

        /// <summary>
        /// Determines size of the increments the move handle can be moved when <see cref="MoveHandleSnapActive"/> is
        /// active.
        /// </summary>
        public static float MoveHandleSnapAmount
        {
            get { return Internal_GetMoveHandleSnapAmount(); }
            set { Internal_SetMoveHandleSnapAmount(value); }
        }

        /// <summary>
        /// Determines size of the increments the rotate handle can be moved when <see cref="RotateHandleSnapActive"/> is
        /// active.
        /// </summary>
        public static Degree RotateHandleSnapAmount
        {
            get { return Internal_GetRotateHandleSnapAmount(); }
            set { Internal_SetRotateHandleSnapAmount(value.Degrees); }
        }

        /// <summary>
        /// Determines the default size for all handles.
        /// </summary>
        public static float DefaultHandleSize
        {
            get { return Internal_GetDefaultHandleSize(); }
            set { Internal_SetDefaultHandleSize(value); }
        }

        /// <summary>
        /// Determines the active tool shown in the scene view.
        /// </summary>
        public static SceneViewTool ActiveSceneTool
        {
            get { return (SceneViewTool)Internal_GetActiveSceneTool(); }
            set { Internal_SetActiveSceneTool((int)value); }
        }

        /// <summary>
        /// Determines the coordinate mode used by the tools in the scene view.
        /// </summary>
        public static HandleCoordinateMode ActiveCoordinateMode
        {
            get { return (HandleCoordinateMode)Internal_GetActiveCoordinateMode(); }
            set { Internal_SetActiveCoordinateMode((int)value); }
        }

        /// <summary>
        /// Determines the pivot mode used by the tools in the scene view.
        /// </summary>
        public static HandlePivotMode ActivePivotMode
        {
            get { return (HandlePivotMode)Internal_GetActivePivotMode(); }
            set { Internal_SetActivePivotMode((int)value); }
        }

        /// <summary>
        /// Contains the absolute path to the last open project, if any.
        /// </summary>
        public static string LastOpenProject
        {
            get { return Internal_GetLastOpenProject(); }
            set { Internal_SetLastOpenProject(value); }
        }

        /// <summary>
        /// Determines should the last open project be automatically loaded on editor startup.
        /// </summary>
        public static bool AutoLoadLastProject
        {
            get { return Internal_GetAutoLoadLastProject(); }
            set { Internal_SetAutoLoadLastProject(value); }
        }

        /// <summary>
        /// Contains a list of most recently loaded projects.
        /// </summary>
        public static RecentProject[] RecentProjects
        {
            get
            {
                string[] paths;
                UInt64[] timeStamps;

                Internal_GetRecentProjects(out paths, out timeStamps);
                RecentProject[] output = new RecentProject[paths.Length];

                for (int i = 0; i < paths.Length; i++)
                    output[i] = new RecentProject(paths[i], timeStamps[i]);

                return output;
            }

            set
            {
                int numEntries = 0;
                if (value != null)
                    numEntries = value.Length;

                string[] paths = new string[numEntries];
                UInt64[] timeStamps = new UInt64[numEntries];

                for (int i = 0; i < numEntries; i++)
                {
                    paths[i] = value[i].path;
                    timeStamps[i] = value[i].accessTimestamp;
                }

                Internal_SetRecentProjects(paths, timeStamps);
            }
        }

        /// <summary>
        /// Contains a hash value that is updated whenever one of the properties in this object is updated. This allows
        /// external systems to track when they might need to reload the settings.
        /// </summary>
        public static int Hash
        {
            get { return Internal_GetHash(); }
        }

        /// <summary>
        /// Sets a generic floating point property.
        /// </summary>
        /// <param name="name">Name to record the property under.</param>
        /// <param name="value">Value of the property.</param>
        public static void SetFloat(string name, float value)
        {
            Internal_SetFloat(name, value);
        }

        /// <summary>
        /// Sets a generic integer property.
        /// </summary>
        /// <param name="name">Name to record the property under.</param>
        /// <param name="value">Value of the property.</param>
        public static void SetInt(string name, int value)
        {
            Internal_SetInt(name, value);
        }

        /// <summary>
        /// Sets a generic boolean property.
        /// </summary>
        /// <param name="name">Name to record the property under.</param>
        /// <param name="value">Value of the property.</param>
        public static void SetBool(string name, bool value)
        {
            Internal_SetBool(name, value);
        }

        /// <summary>
        /// Sets a generic string property.
        /// </summary>
        /// <param name="name">Name to record the property under.</param>
        /// <param name="value">Value of the property.</param>
        public static void SetString(string name, String value)
        {
            Internal_SetString(name, value);
        }

        /// <summary>
        /// Retrieves a generic floating point property.
        /// </summary>
        /// <param name="name">Name of the property to retrieve.</param>
        /// <param name="defaultValue">Default value to return if property cannot be found.</param>
        /// <returns>Value of the property if it exists, otherwise the default value.</returns>
        public static float GetFloat(string name, float defaultValue = 0.0f)
        {
            return Internal_GetFloat(name, defaultValue);
        }

        /// <summary>
        /// Retrieves a generic integer property.
        /// </summary>
        /// <param name="name">Name of the property to retrieve.</param>
        /// <param name="defaultValue">Default value to return if property cannot be found.</param>
        /// <returns>Value of the property if it exists, otherwise the default value.</returns>
        public static int GetInt(string name, int defaultValue = 0)
        {
            return Internal_GetInt(name, defaultValue);
        }

        /// <summary>
        /// Retrieves a generic boolean property.
        /// </summary>
        /// <param name="name">Name of the property to retrieve.</param>
        /// <param name="defaultValue">Default value to return if property cannot be found.</param>
        /// <returns>Value of the property if it exists, otherwise the default value.</returns>
        public static bool GetBool(string name, bool defaultValue = false)
        {
            return Internal_GetBool(name, defaultValue);
        }

        /// <summary>
        /// Retrieves a generic string property.
        /// </summary>
        /// <param name="name">Name of the property to retrieve.</param>
        /// <param name="defaultValue">Default value to return if property cannot be found.</param>
        /// <returns>Value of the property if it exists, otherwise the default value.</returns>
        public static String GetString(string name, string defaultValue = "")
        {
            return Internal_GetString(name, defaultValue);
        }

        /// <summary>
        /// Checks does a generic property with the specified name exists.
        /// </summary>
        /// <param name="name">Name of the property to check.</param>
        /// <returns>True if the property exists, false otherwise.</returns>
        public static bool HasKey(string name)
        {
            return Internal_HasKey(name);
        }

        /// <summary>
        /// Deletes a generic property with the specified name.
        /// </summary>
        /// <param name="name">Name of the property to delete.</param>
        public static void DeleteKey(string name)
        {
            Internal_DeleteKey(name);
        }

        /// <summary>
        /// Deletes all generic properties.
        /// </summary>
        public static void DeleteAllKeys()
        {
            Internal_DeleteAllKeys();
        }

        /// <summary>
        /// Saves editor settings to the disk.
        /// </summary>
        public static void Save()
        {
            Internal_Save();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetMoveHandleSnapActive();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMoveHandleSnapActive(bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetRotateHandleSnapActive();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRotateHandleSnapActive(bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetMoveHandleSnapAmount();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMoveHandleSnapAmount(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetRotateHandleSnapAmount();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRotateHandleSnapAmount(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetDefaultHandleSize();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetDefaultHandleSize(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetActiveSceneTool();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetActiveSceneTool(int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetActiveCoordinateMode();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetActiveCoordinateMode(int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetActivePivotMode();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetActivePivotMode(int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetLastOpenProject();
        [MethodImpl(MethodImplOptions.InternalCall)]
         private static extern void Internal_SetLastOpenProject(string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetAutoLoadLastProject();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetAutoLoadLastProject(bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetRecentProjects(out string[] paths, out UInt64[] timestamps);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRecentProjects(string[] paths, UInt64[] timestamps);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetFloat(string name, float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetInt(string name, int value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetBool(string name, bool value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetString(string name, String value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetFloat(string name, float defaultValue);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetInt(string name, int defaultValue);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetBool(string name, bool defaultValue);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetString(string name, string defaultValue);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_HasKey(string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DeleteKey(string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_DeleteAllKeys();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetHash();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Save();
    }

    /// <summary>
    /// Contains data about a recently opened project.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct RecentProject // Note: Must match C++ struct RecentProject
    {
        /// <summary>
        /// Constructs a new recently opened project object.
        /// </summary>
        /// <param name="path">Absolute path to the project.</param>
        /// <param name="timestamp">Timestamp when the project was last opened.</param>
        public RecentProject(string path, UInt64 timestamp)
        {
            this.path = path;
            this.accessTimestamp = timestamp;
        }

        public string path;
        public UInt64 accessTimestamp;
    }
}
