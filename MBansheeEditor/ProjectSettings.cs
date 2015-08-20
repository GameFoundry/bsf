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
    internal static class ProjectSettings
    {
        public static string LastOpenScene
        {
            get { return Internal_GetLastOpenScene(); }
            set { Internal_SetLastOpenScene(value); }
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
        private static extern string Internal_GetLastOpenScene();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLastOpenScene(string value);

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
}
