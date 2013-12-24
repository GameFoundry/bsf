using System.Runtime.InteropServices;

namespace BansheeEngine
{
    public sealed class SpriteTexture : Resource // TODO - Dummy class
    {
        public SpriteTexture()
        {
            Internal_CreateInstance(this);
        }

        [DllImport("__Internal")]
        private static extern void Internal_CreateInstance(SpriteTexture instance);
    }
}
