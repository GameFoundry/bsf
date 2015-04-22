using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public static class Builtin
    {
        public static SpriteTexture WhiteTexture
        {
            get { return Internal_GetWhiteTexture(); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SpriteTexture Internal_GetWhiteTexture();
    }
}
