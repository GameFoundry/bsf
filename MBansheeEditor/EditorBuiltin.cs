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

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetFolderIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetMeshIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetFontIcon();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetTextureIcon();
    }
}
