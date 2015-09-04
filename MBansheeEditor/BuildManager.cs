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

    public enum WinIconSizes
    {
        Icon16 = 16,
        Icon32 = 32,
        Icon48 = 48,
        Icon64 = 64,
        Icon96 = 96,
        Icon128 = 128,
        Icon196 = 196,
        Icon256 = 256
    }

    public class WinPlatformInfo : PlatformInfo
    {
        public bool Is32Bit
        {
            get { return Internal_GetIs32Bit(mCachedPtr); }
            set { Internal_SetIs32Bit(mCachedPtr, value); }
        }

        public Texture2D GetIcon(WinIconSizes size)
        {
            return Internal_GetIcon(mCachedPtr, (int)size);
        }

        public void SetIcon(WinIconSizes size, Texture2D texture)
        {
            IntPtr texturePtr = IntPtr.Zero;
            if (texture != null)
                texturePtr = texture.GetCachedPtr();

            Internal_SetIcon(mCachedPtr, (int)size, texturePtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetIs32Bit(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetIs32Bit(IntPtr thisPtr, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Texture2D Internal_GetIcon(IntPtr thisPtr, int size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetIcon(IntPtr thisPtr, int size, IntPtr texturePtr);
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
