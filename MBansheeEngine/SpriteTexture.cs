using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Texture interface that encapsulates underlying texture which allows us to create a sprite texture atlas 
    /// (e.g. multiple sprite textures referencing different parts of a single texture).
    /// </summary>
    public sealed class SpriteTexture : Resource
    {
        /// <summary>
        /// Constructor for internal use by the runtime.
        /// </summary>
        private SpriteTexture()
        { }

        /// <summary>
        /// Creates a new sprite texture that references the entire area of the provided texture.
        /// </summary>
        /// <param name="texture">Texture to wrap by the sprite texture.</param>
        public SpriteTexture(Texture2D texture)
        {
            Internal_CreateInstance(this, texture, Vector2.Zero, Vector2.One);
        }

        /// <summary>
        /// Creates a new sprite texture that references a sub-area of the provided texture.
        /// </summary>
        /// <param name="texture">Texture to wrap by the sprite texture.</param>
        /// <param name="uvOffset">Top-left position of the area used by the sprite texture, in normalized coordinates.
        ///                        </param>
        /// <param name="uvScale">Size of the area used by the sprite texture, in normalized coordinates.</param>
        public SpriteTexture(Texture2D texture, Vector2 uvOffset, Vector2 uvScale)
        {
            Internal_CreateInstance(this, texture, uvOffset, uvScale);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SpriteTexture instance, 
            Texture2D teture, Vector2 offset, Vector2 scale);
    }
}
