using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Contains various builtin resources that are always available.
    /// </summary>
    public static class Builtin
    {
        /// <summary>
        /// Returns a pure white texture.
        /// </summary>
        public static SpriteTexture WhiteTexture
        {
            get { return Internal_GetWhiteTexture(); }
        }

        /// <summary>
        /// Returns the default shader to be used with renderables.
        /// </summary>
        public static Shader DiffuseShader
        {
            get { return Internal_GetDiffuseShader(); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetWhiteTexture();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Shader Internal_GetDiffuseShader();
    }
}
