using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public static class EditorBuiltin
    {
        public static SpriteTexture FolderIcon { get { return Internal_GetFolderIcon(); } }
        public static SpriteTexture MeshIcon { get { return Internal_GetMeshIcon(); } }
        public static SpriteTexture FontIcon { get { return Internal_GetFontIcon(); } }
        public static SpriteTexture TextureIcon { get { return Internal_GetTextureIcon(); } }
        public static SpriteTexture PlainTextIcon { get { return Internal_GetPlainTextIcon(); } }
        public static SpriteTexture ScriptCodeIcon { get { return Internal_GetScriptCodeIcon(); } }
        public static SpriteTexture ShaderIcon { get { return Internal_GetShaderIcon(); } }
        public static SpriteTexture ShaderIncludeIcon { get { return Internal_GetShaderIncludeIcon(); } }
        public static SpriteTexture MaterialIcon { get { return Internal_GetMaterialIcon(); } }
        public static SpriteTexture SpriteTextureIcon { get { return Internal_GetSpriteTextureIcon(); } }
        public static SpriteTexture PrefabIcon { get { return Internal_GetPrefabIcon(); } }

        public static SpriteTexture XBtnIcon { get { return Internal_GetXBtnIcon(); } }

        /// <summary>
        /// Returns text contained in the default "empty" shader.
        /// </summary>
        public static string EmptyShaderCode { get { return Internal_GetEmptyShaderCode(); } }

        /// <summary>
        /// Returns text contained in the default "empty" C# script.
        /// </summary>
        public static string EmptyCSScriptCode { get { return Internal_GetEmptyCSScriptCode(); } }

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
    }
}
