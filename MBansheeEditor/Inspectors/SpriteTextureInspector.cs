using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Renders an inspector for the <see cref="SpriteTexture"/> resource.
    /// </summary>
    [CustomInspector(typeof(SpriteTexture))]
    internal class SpriteTextureInspector : Inspector
    {
        private GUITextureField textureField = new GUITextureField(new LocEdString("Texture"));
        private GUIVector2Field offsetField = new GUIVector2Field(new LocEdString("Offset"));
        private GUIVector2Field scaleField = new GUIVector2Field(new LocEdString("Scale"));
        private bool isInitialized;

        /// <inheritdoc/>
        internal override bool Refresh()
        {
            SpriteTexture spriteTexture = referencedObject as SpriteTexture;
            if (spriteTexture == null)
                return false;

            if (!isInitialized)
            {
                textureField.OnChanged += (x) =>
                {
                    spriteTexture.Texture = x as Texture2D;
                    EditorApplication.SetDirty(spriteTexture);
                };

                offsetField.OnChanged += (x) =>
                {
                    spriteTexture.Offset = x;
                    EditorApplication.SetDirty(spriteTexture);
                };

                scaleField.OnChanged += (x) =>
                {
                    spriteTexture.Scale = x;
                    EditorApplication.SetDirty(spriteTexture);
                };

                layout.AddElement(textureField);
                layout.AddElement(offsetField);
                layout.AddElement(scaleField);

                isInitialized = true;
            }

            bool anythingModified = false;

            if (textureField.Value != spriteTexture.Texture)
            {
                textureField.Value = spriteTexture.Texture;
                anythingModified = true;
            }

            if (offsetField.Value != spriteTexture.Offset)
            {
                offsetField.Value = spriteTexture.Offset;
                anythingModified = true;
            }

            if (scaleField.Value != spriteTexture.Scale)
            {
                scaleField.Value = spriteTexture.Scale;
                anythingModified = true;
            }

            return anythingModified;
        }
    }
}