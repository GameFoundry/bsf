using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class SpriteTexture : Resource // TODO - Dummy class
    {
        // For internal use by the runtime
        private SpriteTexture()
        {
            
        }

        public SpriteTexture(Texture2D texture)
        {
            Internal_CreateInstance(this, texture, Vector2.Zero, Vector2.One);
        }

        public SpriteTexture(Texture2D texture, Vector2 uvOffset, Vector2 uvScale)
        {
            Internal_CreateInstance(this, texture, uvOffset, uvScale);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SpriteTexture instance, 
            Texture2D teture, Vector2 offset, Vector2 scale);
    }
}
