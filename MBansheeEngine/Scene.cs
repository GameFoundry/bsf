using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Handles operations with the active scene (level).
    /// </summary>
    public static class Scene
    {
        /// <summary>
        /// Returns the name of the scene prefab. This is empty if scene hasn't been saved yet.
        /// </summary>
        public static string ActiveSceneName { get { return activeSceneName; } }

        /// <summary>
        /// Returns the UUID of the scene prefab. This is empty if scene hasn't been saved yet.
        /// </summary>
        internal static string ActiveSceneUUID { get { return activeSceneUUID; } }

        private static string activeSceneName = "Unnamed";
        private static string activeSceneUUID = "";

        /// <summary>
        /// Returns the root scene object for the current scene.
        /// </summary>
        public static SceneObject Root
        {
            get { return Internal_GetRoot(); }
        }

        /// <summary>
        /// Clears all scene objects from the current scene.
        /// </summary>
        public static void Clear()
        {
            Internal_ClearScene();
            activeSceneUUID = null;
            activeSceneName = "Unnamed";
        }

        /// <summary>
        /// Loads a new scene.
        /// </summary>
        /// <param name="path">Path to the prefab to load.</param>
        public static void Load(string path)
        {
            Clear();
            Prefab scene = Internal_LoadScene(path);

            if (scene != null)
            {
                activeSceneUUID = scene.UUID;
                activeSceneName = scene.Name;
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Prefab Internal_LoadScene(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SceneObject Internal_GetRoot();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClearScene();
    }
}
