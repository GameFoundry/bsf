using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class SpriteTexture : Resource // TODO - Dummy class
    {
        public SpriteTexture()
        {
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SpriteTexture instance);
    }
}
