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
    /// Contains various settings that are applied globally per project. Settings will persist through editor sessions.
    /// </summary>
    internal static class ProjectSettings
    {
        /// <summary>
        /// Contains the path to the last open scene. Path is relative to the project's resources folder.
        /// </summary>
        public static string LastOpenScene
        {
            get { return Internal_GetLastOpenScene(); }
            set { Internal_SetLastOpenScene(value); }
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
        /// Saves project settings to the disk.
        /// </summary>
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
