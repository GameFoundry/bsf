using System;
using System.IO;
using System.Runtime.CompilerServices;
using System.Threading;
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
        /// Determines should the scripts be output in debug mode (worse performance but better error reporting).
        /// </summary>
        public bool Debug
        {
            get { return Internal_GetDebug(mCachedPtr); }
            set { Internal_SetDebug(mCachedPtr, value); }
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern bool Internal_GetDebug(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static extern void Internal_SetDebug(IntPtr thisPtr, bool fullscreen);
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
        /// Texture that will be displayed on the application's executable.
        /// </summary>
        public Texture2D Icon
        {
            get { return Internal_GetIcon(mCachedPtr); }
            set
            {
                IntPtr texturePtr = IntPtr.Zero;
                if (value != null)
                    texturePtr = value.GetCachedPtr();

                Internal_SetIcon(mCachedPtr, texturePtr);
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
        
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Texture2D Internal_GetIcon(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetIcon(IntPtr thisPtr, IntPtr texturePtr);

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
        /// Returns absolute path to the folder where builds for the currently active platform are output.
        /// </summary>
        public static string OutputFolder
        {
            get { return GetBuildFolder(BuildFolder.DestinationRoot, ActivePlatform); }
        }

        /// <summary>
        /// Returns a path to a specific folder used in the build process. See entries of BuildFolder enum for explanations 
        /// of individual folder types.
        /// </summary>
        /// <param name="folder">Type of folder to retrieve the path for.</param>
        /// <param name="platform">Platform to retrieve the path for.</param>
        /// <returns>Path for the requested folder. This can be absolute or relative, see <see cref="BuildFolder"/> enum
        ///          for details.</returns>
        private static string GetBuildFolder(BuildFolder folder, PlatformType platform)
        {
            return Internal_GetBuildFolder(folder, platform);
        }

        /// <summary>
        /// Returns a list of names of all native binaries required for a specific platform.
        /// </summary>
        /// <param name="platform">Platform type for which to get the binaries for.</param>
        /// <returns>Array of names of native binary files.</returns>
        private static string[] GetNativeBinaries(PlatformType platform)
        {
            return Internal_GetNativeBinaries(platform);
        }

        /// <summary>
        /// Builds the executable and packages the game.
        /// </summary>
        public static void Build()
        {
            PlatformType activePlatform = ActivePlatform;
            PlatformInfo platformInfo = ActivePlatformInfo;

            string srcRoot = GetBuildFolder(BuildFolder.SourceRoot, activePlatform);
            string destRoot = GetBuildFolder(BuildFolder.DestinationRoot, activePlatform);

            // Prepare clean destination folder
            if(Directory.Exists(destRoot))
                Directory.Delete(destRoot, true);

            Directory.CreateDirectory(destRoot);

            // Compile game assembly
            string bansheeAssemblyFolder = GetBuildFolder(BuildFolder.BansheeAssemblies, activePlatform);
            string srcBansheeAssemblyFolder = Path.Combine(srcRoot, bansheeAssemblyFolder);
            string destBansheeAssemblyFolder = Path.Combine(destRoot, bansheeAssemblyFolder);

            Directory.CreateDirectory(destBansheeAssemblyFolder);
            CompilerInstance ci = ScriptCompiler.CompileAsync(ScriptAssemblyType.Game, ActivePlatform, platformInfo.Debug, destBansheeAssemblyFolder);

            // Copy engine assembly
            // TODO - Copy Release version of engine assembly if not in debug mode, and Debug otherwise
            {
                string srcFile = Path.Combine(srcBansheeAssemblyFolder, EditorApplication.EngineAssemblyName);
                string destFile = Path.Combine(destBansheeAssemblyFolder, EditorApplication.EngineAssemblyName);

                File.Copy(srcFile, destFile);
            }

            // Copy builtin data
            string dataFolder = GetBuildFolder(BuildFolder.Data, activePlatform);
            string srcData = Path.Combine(srcRoot, dataFolder);
            string destData = Path.Combine(destRoot, dataFolder);

            DirectoryEx.Copy(srcData, destData);

            // Copy native binaries
            string binaryFolder = GetBuildFolder(BuildFolder.NativeBinaries, activePlatform);
            string srcBin = Path.Combine(srcRoot, binaryFolder);
            string destBin = destRoot;

            string[] nativeBinaries = GetNativeBinaries(activePlatform);
            foreach (var entry in nativeBinaries)
            {
                string srcFile = Path.Combine(srcBin, entry);
                string destFile = Path.Combine(destBin, entry);

                File.Copy(srcFile, destFile);
            }

            // Copy .NET framework assemblies
            string frameworkAssemblyFolder = GetBuildFolder(BuildFolder.FrameworkAssemblies, activePlatform);
            string srcFrameworkAssemblyFolder = Path.Combine(srcRoot, frameworkAssemblyFolder);
            string destFrameworkAssemblyFolder = Path.Combine(destRoot, frameworkAssemblyFolder);

            Directory.CreateDirectory(destFrameworkAssemblyFolder);

            string[] frameworkAssemblies = GetFrameworkAssemblies(activePlatform);
            foreach (var entry in frameworkAssemblies)
            {
                string srcFile = Path.Combine(srcFrameworkAssemblyFolder, entry + ".dll");
                string destFile = Path.Combine(destFrameworkAssemblyFolder, entry + ".dll");

                File.Copy(srcFile, destFile);
            }

            // Copy Mono
            string monoFolder = GetBuildFolder(BuildFolder.Mono, activePlatform);
            string srcMonoFolder = Path.Combine(srcRoot, monoFolder);
            string destMonoFolder = Path.Combine(destRoot, monoFolder);

            DirectoryEx.Copy(srcMonoFolder, destMonoFolder);

            string srcExecFile = GetMainExecutable(activePlatform);
            string destExecFile = Path.Combine(destBin, Path.GetFileName(srcExecFile));

            File.Copy(srcExecFile, destExecFile);

            InjectIcons(destExecFile, platformInfo);
            PackageResources(destRoot, platformInfo);
            CreateStartupSettings(destRoot, platformInfo);

            // Wait until compile finishes
            while (!ci.IsDone)
                Thread.Sleep(200);

            ci.Dispose();
        }

        /// <summary>
        /// Injects icons specified in <see cref="PlatformInfo"/> into an executable at the specified path.
        /// </summary>
        /// <param name="filePath">Absolute path to the executable to inject icons in.</param>
        /// <param name="info">Object containing references to icons to inject.</param>
        private static void InjectIcons(string filePath, PlatformInfo info)
        {
            IntPtr infoPtr = IntPtr.Zero;
            if (info != null)
                infoPtr = info.GetCachedPtr();

            Internal_InjectIcons(filePath, infoPtr);
        }

        /// <summary>
        /// Finds all used resources by the build and packages them into an output folder.
        /// </summary>
        /// <param name="buildFolder">Absolute path to the root folder of the build. This is where the packaged resource
        ///                           folder be placed.</param>
        /// <param name="info">Platform information about the current build.</param>
        private static void PackageResources(string buildFolder, PlatformInfo info)
        {
            IntPtr infoPtr = IntPtr.Zero;
            if (info != null)
                infoPtr = info.GetCachedPtr();

            Internal_PackageResources(buildFolder, infoPtr);
        }

        /// <summary>
        /// Creates a game settings asset that contains necessary data for starting up the game (e.g. initial scene).
        /// </summary>
        /// <param name="buildFolder">Absolute path to the root folder of the build. This is where the settings assets
        ///                           will be output.</param>
        /// <param name="info">Platform information about the current build.</param>
        private static void CreateStartupSettings(string buildFolder, PlatformInfo info)
        {
            IntPtr infoPtr = IntPtr.Zero;
            if (info != null)
                infoPtr = info.GetCachedPtr();

            Internal_CreateStartupSettings(buildFolder, infoPtr);
        }

        /// <summary>
        /// Returns a list of .NET framework managed assemblies (without extension) to be included for the specified platform.
        /// </summary>
        /// <param name="type">Platform type to retrieve the list of assemblies for.</param>
        /// <returns>A list of .NET framework managed assemblies (without extension) that will be included with the build.</returns>
        internal static string[] GetFrameworkAssemblies(PlatformType type)
        {
            return Internal_GetFrameworkAssemblies(type);
        }

        /// <summary>
        /// Returns the absolute path to the executable for the provided platform.
        /// </summary>
        /// <param name="type">Platform type to retrieve the executable location for.</param>
        /// <returns>Absolute path to the executable.</returns>
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

        /// <summary>
        /// Types of various folders used by the build manager.
        /// </summary>
        private enum BuildFolder // Note: Must match C++ enum ScriptBuildFolder
        {
            /// <summary>Absolute path to the root folder where all the prebuilt binaries and data exist.</summary>
            SourceRoot,
            /// <summary>Absolute path to the root folder for a build for a specific platform.</summary>
            DestinationRoot,
            /// <summary>Folder where native binaries are stored. Relative to root.</summary>
            NativeBinaries,
            /// <summary>Folder where Banshee specific assemblies are stored. Relative to root.</summary>
            BansheeAssemblies,
            /// <summary>Folder where .NET framework assemblies are stored. Relative to root.</summary>
            FrameworkAssemblies,
            /// <summary>Folder where miscelaneous Mono files are stored. Relative to root.</summary>
            Mono,
            /// <summary>Folder where builtin data is stored. Relative to root.</summary>
            Data
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string[] Internal_GetNativeBinaries(PlatformType type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetBuildFolder(BuildFolder folder, PlatformType platform);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_InjectIcons(string filePath, IntPtr info);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_PackageResources(string buildFolder, IntPtr info);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateStartupSettings(string buildFolder, IntPtr info);
    }
}
