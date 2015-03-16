using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    // Note: Must match C++ enum PlatformType
    public enum PlatformType
    {
        Windows,
        Count // Keep at end
    }

    public abstract class PlatformInfo : ScriptObject
    {
        protected PlatformInfo()
        { }

        public PlatformType Type
        {
            get { return Internal_GetType(mCachedPtr); }
        }

        public string Defines
        {
            get { return Internal_GetDefines(mCachedPtr); }
            set { Internal_SetDefines(mCachedPtr, value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PlatformType Internal_GetType(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetDefines(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetDefines(IntPtr thisPtr, string value);
    }

    public class WinPlatformInfo : PlatformInfo
    {
        public bool Is32Bit
        {
            get { return Internal_GetIs32Bit(mCachedPtr); }
            set { Internal_SetIs32Bit(mCachedPtr, value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetIs32Bit(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetIs32Bit(IntPtr thisPtr, bool value);
    }

    public static class BuildManager
    {
        public static PlatformType[] AvailablePlatforms
        {
            get { return Internal_GetAvailablePlatforms(); }
        }

        public static PlatformType ActivePlatform
        {
            get { return Internal_GetActivePlatform(); }
            set { Internal_SetActivePlatform(value); }
        }

        public static PlatformInfo ActivePlatformInfo
        {
            get { return Internal_GetActivePlatformInfo(); }
        }

        public static void Build()
        {
            // TODO
        }

        internal static string[] GetFrameworkAssemblies(PlatformType type)
        {
            return Internal_GetFrameworkAssemblies(type);
        }

        internal static string GetMainExecutable(PlatformType type)
        {
            return Internal_GetMainExecutable(type);
        }

        internal static string GetDefines(PlatformType type)
        {
            return Internal_GetDefines(type);
        }

        internal static PlatformInfo GetPlatformInfo(PlatformType type)
        {
            return Internal_GetPlatformInfo(type);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PlatformType[] Internal_GetAvailablePlatforms();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PlatformType Internal_GetActivePlatform();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetActivePlatform(PlatformType value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PlatformInfo Internal_GetActivePlatformInfo();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PlatformInfo Internal_GetPlatformInfo(PlatformType type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string[] Internal_GetFrameworkAssemblies(PlatformType type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetMainExecutable(PlatformType type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetDefines(PlatformType type);
    }
}
