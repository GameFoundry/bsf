using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Contains a list of valid platforms that can be built for.
    /// </summary>
    public enum PlatformType // Note: Must match C++ enum PlatformType
    {
        Windows,
        Count // Keep at end
    }

    /// <summary>
    /// Contains build data for a specific platform.
    /// </summary>
    public abstract class PlatformInfo : ScriptObject
    {
        /// <summary>
        /// Creates a new platform info. For internal runtime use only.
        /// </summary>
        protected PlatformInfo()
        { }

        /// <summary>
        /// Returns the platform that this object contains data for.
        /// </summary>
        public PlatformType Type
        {
            get { return Internal_GetType(mCachedPtr); }
        }

        /// <summary>
        /// Initial scene that is loaded when application is first started.
        /// </summary>
        public Prefab MainScene
        {
            get { return Internal_GetMainScene(mCachedPtr); }
            set
            {
                IntPtr scenePtr = IntPtr.Zero;
                if (value != null)
                    scenePtr = value.GetCachedPtr();

                Internal_SetMainScene(mCachedPtr, scenePtr);
            }
        }

        /// <summary>
        /// Determines should the application be started in fullscreen using the user's desktop resolution.
        /// </summary>
        public bool Fullscreen
        {
            get { return Internal_GetFullscreen(mCachedPtr); }
            set { Internal_SetFullscreen(mCachedPtr, value); }
        }

        /// <summary>
        /// Width of a window if the game is started in windowed mode. This is only relevant if <see cref="Fullscreen"/>
        /// is off.
        /// </summary>
        public int WindowedWidth
        {
            get
            {
                int width, height;
                Internal_GetResolution(mCachedPtr, out width, out height);

                return width;
            }

            set { Internal_SetResolution(mCachedPtr, value, WindowedHeight); }
        }

        /// <summary>
        /// Height of a window if the game is started in windowed mode. This is only relevant if <see cref="Fullscreen"/>
        /// is off.
        /// </summary>
        public int WindowedHeight
        {
            get
            {
                int width, height;
                Internal_GetResolution(mCachedPtr, out width, out height);

                return height;
            }

            set { Internal_SetResolution(mCachedPtr, WindowedWidth, value); }
        }

        /// <summary>
        /// A set of semicolon separated defines to use when compiling scripts for this platform.
        /// </summary>
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Prefab Internal_GetMainScene(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void Internal_SetMainScene(IntPtr thisPtr, IntPtr prefabPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern bool Internal_GetFullscreen(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void Internal_SetFullscreen(IntPtr thisPtr, bool fullscreen);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void Internal_GetResolution(IntPtr thisPtr, out int width, out int height);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void Internal_SetResolution(IntPtr thisPtr, int width, int height);
    }

    /// <summary>
    /// Supported icon sizes for Windows platform.
    /// </summary>
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

    /// <summary>
    /// Platform data specific to Windows.
    /// </summary>
    public class WinPlatformInfo : PlatformInfo
    {
        /// <summary>
        /// Texture that will be displayed in the taskbar when the application is running.
        /// </summary>
        public Texture2D TaskbarIcon
        {
            get { return Internal_GetTaskbarIcon(mCachedPtr); }
            set
            {
                IntPtr texturePtr = IntPtr.Zero;
                if (value != null)
                    texturePtr = value.GetCachedPtr();

                Internal_SetTaskbarIcon(mCachedPtr, texturePtr);
            }
        }

        /// <summary>
        /// Text that will be displayed in the application's title bar.
        /// </summary>
        public string TitleText
        {
            get { return Internal_GetTitleText(mCachedPtr); }
            set { Internal_SetTitleText(mCachedPtr, value); }
        }

        /// <summary>
        /// Returns a texture of a specific icon size that will be added to the executable.
        /// </summary>
        /// <param name="size">Type of icon to retrieve the texture for.</param>
        /// <returns>Texture for the specified icon size.</returns>
        public Texture2D GetIcon(WinIconSizes size)
        {
            return Internal_GetIcon(mCachedPtr, (int)size);
        }

        /// <summary>
        /// Sets a texture of a specific icon size that will be added to the executable.
        /// </summary>
        /// <param name="size">Type of icon to set the texture for.</param>
        /// <param name="texture">Texture for the specified icon size.</param>
        public void SetIcon(WinIconSizes size, Texture2D texture)
        {
            IntPtr texturePtr = IntPtr.Zero;
            if (texture != null)
                texturePtr = texture.GetCachedPtr();

            Internal_SetIcon(mCachedPtr, (int)size, texturePtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Texture2D Internal_GetIcon(IntPtr thisPtr, int size);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetIcon(IntPtr thisPtr, int size, IntPtr texturePtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Texture2D Internal_GetTaskbarIcon(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTaskbarIcon(IntPtr thisPtr, IntPtr texturePtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetTitleText(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTitleText(IntPtr thisPtr, string value);
    }

    /// <summary>
    /// Handles building of the game executable and packaging of all necessary resources, making the game be ready to ran
    /// as a standalone product.
    /// </summary>
    public static class BuildManager
    {
        /// <summary>
        /// Returns a list of all available platforms that can be built for.
        /// </summary>
        public static PlatformType[] AvailablePlatforms
        {
            get { return Internal_GetAvailablePlatforms(); }
        }

        /// <summary>
        /// Returns the currently active platform.
        /// </summary>
        public static PlatformType ActivePlatform
        {
            get { return Internal_GetActivePlatform(); }
            set { Internal_SetActivePlatform(value); }
        }

        /// <summary>
        /// Returns the data about the currently active platform.
        /// </summary>
        public static PlatformInfo ActivePlatformInfo
        {
            get { return Internal_GetActivePlatformInfo(); }
        }

        /// <summary>
        /// Builds the executable and packages the game.
        /// </summary>
        public static void Build()
        {
            // TODO
        }

        /// <summary>
        /// Returns a list of .NET framework managed assemblies to be included for the specified platform.
        /// </summary>
        /// <param name="type">Platform type to retrieve the list of assemblies for.</param>
        /// <returns>A list of .NET framework managed assemblies that will be included with the build.</returns>
        internal static string[] GetFrameworkAssemblies(PlatformType type)
        {
            return Internal_GetFrameworkAssemblies(type);
        }

        /// <summary>
        /// Returns the location of the executable for the provided platform.
        /// </summary>
        /// <param name="type">Platform type to retrieve the executable location for.</param>
        /// <returns>Path to the executable in the editor install folder.</returns>
        internal static string GetMainExecutable(PlatformType type)
        {
            return Internal_GetMainExecutable(type);
        }

        /// <summary>
        /// Returns a list of semicolon separated defines that will be used when compiling scripts for the specified 
        /// platform.
        /// </summary>
        /// <param name="type">Platfrom type to retrieve the defines for.</param>
        /// <returns>Semicolor separated defines that will be passed along to the script compiler.</returns>
        internal static string GetDefines(PlatformType type)
        {
            return Internal_GetDefines(type);
        }

        /// <summary>
        /// Returns an object containing all platform specific build data.
        /// </summary>
        /// <param name="type">Platform type to retrieve the data for.</param>
        /// <returns>An object containing all platform specific build data</returns>
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
