using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Handles operations with the active scene (level).
    /// </summary>
    public static class Scene
    {
        /// <summary>
        /// Returns the UUID of the scene prefab. This is empty if scene hasn't been saved yet.
        /// </summary>
        internal static string ActiveSceneUUID { get; set; }

        /// <summary>
        /// Checks did we make any modifications to the scene since it was last saved.
        /// </summary>
        /// <returns>True if the scene was never saved, or was modified after last save.</returns>
        public static bool IsModified()
        {
            // TODO - Needs implementing
            return true;
        }

        /// <summary>
        /// Clears all scene objects from the current scene.
        /// </summary>
        public static void Clear()
        {
            Internal_ClearScene();
            ActiveSceneUUID = null;
        }

        /// <summary>
        /// Loads a new scene.
        /// </summary>
        /// <param name="path">Path to the prefab to load.</param>
        public static void Load(string path)
        {
            Clear();
            ActiveSceneUUID = Internal_LoadScene(path);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_LoadScene(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClearScene();
    }
}
