using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class SpriteTexture : Resource // TODO - Dummy class
    {
        // For internal use by the runtime
        private SpriteTexture()
        {
            
        }

        public SpriteTexture(int dummyParam)
        {
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SpriteTexture instance);
    }
}
