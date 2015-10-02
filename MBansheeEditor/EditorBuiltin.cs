using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Types of icons that may be displayed on the tool bar.
    /// </summary>
    public enum ToolbarIcon // Note: Must match C++ enum ToolbarIcon
	{
		NewCamera, NewRenderable, NewPointLight, NewDirLight, NewSpotLight, NewCube, NewSphere, NewCone, NewQuad,
		NewMat, NewCSScript, NewShader, NewSpriteTex, Pause, Play, Step, Undo, Redo, OpenProject, SaveProject, SaveScene
	};

    /// <summary>
    /// Types of icons that may be displayed in the scene window.
    /// </summary>
    public enum SceneWindowIcon // Note: Must match C++ enum SceneWindowIcon
	{
		View, Move, Rotate, Scale, Pivot, Center, Local, World, MoveSnap, RotateSnap
	};

    /// <summary>
    /// Types of icons that may be displayed in the library window.
    /// </summary>
    public enum LibraryWindowIcon // Note: Must match C++ enum LibraryWindowIcon
	{
		Home, Up, Clear, Options
	};

    /// <summary>
    /// Types of icons that may be displayed in the inspector window.
    /// </summary>
	public enum InspectorWindowIcon  // Note: Must match C++ enum InspectorWindowIcon
	{
		Create, Clone, Clear, Resize, Delete, MoveUp, MoveDown, Edit, Apply
	};

    /// <summary>
    /// Contains various editor-specific resources that are always available.
    /// </summary>
    public static class EditorBuiltin
    {
        /// <summary>Icon used for displaying folders in the library window.</summary>
        public static SpriteTexture FolderIcon { get { return Internal_GetFolderIcon(); } }

        /// <summary>Icon used for displaying mesh resources in the library window.</summary>
        public static SpriteTexture MeshIcon { get { return Internal_GetMeshIcon(); } }

        /// <summary>Icon used for displaying font resources in the library window.</summary>
        public static SpriteTexture FontIcon { get { return Internal_GetFontIcon(); } }

        /// <summary>Icon used for displaying texture resources in the library window.</summary>
        public static SpriteTexture TextureIcon { get { return Internal_GetTextureIcon(); } }

        /// <summary>Icon used for displaying plain text resources in the library window.</summary>
        public static SpriteTexture PlainTextIcon { get { return Internal_GetPlainTextIcon(); } }

        /// <summary>Icon used for displaying script code resources in the library window.</summary>
        public static SpriteTexture ScriptCodeIcon { get { return Internal_GetScriptCodeIcon(); } }

        /// <summary>Icon used for displaying shader resources in the library window.</summary>
        public static SpriteTexture ShaderIcon { get { return Internal_GetShaderIcon(); } }

        /// <summary>Icon used for displaying shader include resources in the library window.</summary>
        public static SpriteTexture ShaderIncludeIcon { get { return Internal_GetShaderIncludeIcon(); } }

        /// <summary>Icon used for displaying material resources in the library window.</summary>
        public static SpriteTexture MaterialIcon { get { return Internal_GetMaterialIcon(); } }

        /// <summary>Icon used for displaying sprite texture resources in the library window.</summary>
        public static SpriteTexture SpriteTextureIcon { get { return Internal_GetSpriteTextureIcon(); } }

        /// <summary>Icon used for displaying prefab resources in the library window.</summary>
        public static SpriteTexture PrefabIcon { get { return Internal_GetPrefabIcon(); } }

        public static SpriteTexture XBtnIcon { get { return Internal_GetXBtnIcon(); } }

        /// <summary>Returns text contained in the default "empty" shader.</summary>
        public static string EmptyShaderCode { get { return Internal_GetEmptyShaderCode(); } }

        /// <summary>Returns text contained in the default "empty" C# script.</summary>
        public static string EmptyCSScriptCode { get { return Internal_GetEmptyCSScriptCode(); } }

        /// <summary>
        /// Retrieves an icon that may be displayed on the main window's toolbar.
        /// </summary>
        /// <param name="icon">Type of icon to retrieve.</param>
        /// <returns>Sprite texture of the icon.</returns>
        public static SpriteTexture GetToolbarIcon(ToolbarIcon icon)
        {
            return Internal_GetToolbarIcon(icon);
        }

        /// <summary>
        /// Retrieves an icon that may be displayed on the library window.
        /// </summary>
        /// <param name="icon">Type of icon to retrieve.</param>
        /// <returns>Sprite texture of the icon.</returns>
        public static SpriteTexture GetLibraryWindowIcon(LibraryWindowIcon icon)
        {
            return Internal_GetLibraryWindowIcon(icon);
        }

        /// <summary>
        /// Retrieves an icon that may be displayed on the inspector window.
        /// </summary>
        /// <param name="icon">Type of icon to retrieve.</param>
        /// <returns>Sprite texture of the icon.</returns>
        public static SpriteTexture GetInspectorWindowIcon(InspectorWindowIcon icon)
        {
            return Internal_GetInspectorWindowIcon(icon);
        }


        /// <summary>
        /// Retrieves an icon that may be displayed on the scene window.
        /// </summary>
        /// <param name="icon">Type of icon to retrieve.</param>
        /// <returns>Sprite texture of the icon.</returns>
        public static SpriteTexture GetSceneWindowIcon(SceneWindowIcon icon)
        {
            return Internal_GetSceneWindowIcon(icon);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetFolderIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetMeshIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetFontIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetTextureIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetPlainTextIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetScriptCodeIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetShaderIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetShaderIncludeIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetMaterialIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetSpriteTextureIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetPrefabIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetXBtnIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetEmptyShaderCode();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetEmptyCSScriptCode();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetToolbarIcon(ToolbarIcon icon);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetLibraryWindowIcon(LibraryWindowIcon icon);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetInspectorWindowIcon(InspectorWindowIcon icon);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetSceneWindowIcon(SceneWindowIcon icon);
    }
}
