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
    internal static class EditorSettings
    {
        public static bool MoveHandleSnapActive
        {
            get { return Internal_GetMoveHandleSnapActive(); }
            set { Internal_SetMoveHandleSnapActive(value); }
        }

        public static bool RotateHandleSnapActive
        {
            get { return Internal_GetRotateHandleSnapActive(); }
            set { Internal_SetRotateHandleSnapActive(value); }
        }

        public static float MoveHandleSnapAmount
        {
            get { return Internal_GetMoveHandleSnapAmount(); }
            set { Internal_SetMoveHandleSnapAmount(value); }
        }

        public static Degree RotateHandleSnapAmount
        {
            get { return Internal_GetRotateHandleSnapAmount(); }
            set { Internal_SetRotateHandleSnapAmount(value.Degrees); }
        }

        public static float DefaultHandleSize
        {
            get { return Internal_GetDefaultHandleSize(); }
            set { Internal_SetDefaultHandleSize(value); }
        }

        public static SceneViewTool ActiveSceneTool
        {
            get { return (SceneViewTool)Internal_GetActiveSceneTool(); }
            set { Internal_SetActiveSceneTool((int)value); }
        }

        public static HandleCoordinateMode ActiveCoordinateMode
        {
            get { return (HandleCoordinateMode)Internal_GetActiveCoordinateMode(); }
            set { Internal_SetActiveCoordinateMode((int)value); }
        }

        public static HandlePivotMode ActivePivotMode
        {
            get { return (HandlePivotMode)Internal_GetActivePivotMode(); }
            set { Internal_SetActivePivotMode((int)value); }
        }

        public static string LastOpenProject
        {
            get { return Internal_GetLastOpenProject(); }
            set { Internal_SetLastOpenProject(value); }
        }

        public static bool AutoLoadLastProject
        {
            get { return Internal_GetAutoLoadLastProject(); }
            set { Internal_SetAutoLoadLastProject(value); }
        }

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

        public static int Hash
        {
            get { return Internal_GetHash(); }
        }

        public static void SetFloat(string name, float value)
        {
            Internal_SetFloat(name, value);
        }

        public static void SetInt(string name, int value)
        {
            Internal_SetInt(name, value);
        }

        public static void SetBool(string name, bool value)
        {
            Internal_SetBool(name, value);
        }

        public static void SetString(string name, String value)
        {
            Internal_SetString(name, value);
        }

        public static float GetFloat(string name, float defaultValue = 0.0f)
        {
            return Internal_GetFloat(name, defaultValue);
        }

        public static int GetInt(string name, int defaultValue = 0)
        {
            return Internal_GetInt(name, defaultValue);
        }

        public static bool GetBool(string name, bool defaultValue = false)
        {
            return Internal_GetBool(name, defaultValue);
        }

        public static String GetString(string name, string defaultValue = "")
        {
            return Internal_GetString(name, defaultValue);
        }

        public static bool HasKey(string name)
        {
            return Internal_HasKey(name);
        }

        public static void DeleteKey(string name)
        {
            Internal_DeleteKey(name);
        }

        public static void DeleteAllKeys()
        {
            Internal_DeleteAllKeys();
        }

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

    // Note: Must match C++ struct RecentProject
    [StructLayout(LayoutKind.Sequential)]
    public struct RecentProject
    {
        public RecentProject(string path, UInt64 timestamp)
        {
            this.path = path;
            this.accessTimestamp = timestamp;
        }

        public string path;
        public UInt64 accessTimestamp;
    }
}
