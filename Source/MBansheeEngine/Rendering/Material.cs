//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
namespace BansheeEngine
{
    /** @addtogroup Rendering
     *  @{
     */

    public partial class Material
    {
        /// <summary>
        /// Returns a texture assigned to the material.
        /// </summary>
        /// <param name="name">Name of the texture parameter.</param>
        /// <returns>Texture assigned to the specified material</returns>
        public Texture GetTexture(string name)
        {
            return Internal_getTexture(mCachedPtr, name);
        }

        /// <summary>
        /// Assigns a texture to the specified material parameter. Allows you to specify a surface parameter that allows you
        /// to bind only a sub-set of the texture.
        /// </summary>
        /// <param name="name">Name of the texture parameter.</param>
        /// <param name="texture">Texture resource to assign.</param>
        /// <param name="surface">Subset of the texture to assign.</param>
        public void SetTexture(string name, Texture texture, TextureSurface surface)
        {
            Internal_setTexture(mCachedPtr, name, texture, surface.mipLevel, surface.numMipLevels, surface.face, 
                surface.numFaces);
        }

        /// <summary>
        /// Assigns a texture to the specified material parameter.
        /// </summary>
        /// <param name="name">Name of the texture parameter.</param>
        /// <param name="texture">Texture resource to assign.</param>
        public void SetTexture(string name, Texture texture)
        {
            Internal_setTexture(mCachedPtr, name, texture, 0, 0, 0, 0);
        }
    }

    /** @} */
}
