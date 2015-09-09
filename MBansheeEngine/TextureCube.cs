using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// A cube texture. Cube texture consists of six two dimensional texture faces that can be accessed separately.
    /// </summary>
    public sealed class TextureCube : Texture
    {
        /// <summary>
        /// Constructor for internal use by the runtime.
        /// </summary>
        private TextureCube()
        { }

        /// <summary>
        /// Creates a new blank cube texture.
        /// </summary>
        /// <param name="width">Width of a single face of the texture in pixels.</param>
        /// <param name="height">Height of a single face of the texture in pixels.</param>
        /// <param name="format">Format of the pixels.</param>
        /// <param name="usage">Describes planned texture use.</param>
        /// <param name="numSamples">If higher than 1, texture containing multiple samples per pixel is created.</param>
        /// <param name="hasMipmaps">Should the texture allocate memory for the entire mip-map chain or only the top level.
        ///                          </param>
        /// <param name="gammaCorrection">If true the texture data is assumed to have be gamma corrected and will be
        ///                               converted back to linear space when sampled on GPU, and converted to gamma space
        ///                               before being written by the GPU.</param>
        public TextureCube(PixelFormat format, int width, int height, TextureUsage usage = TextureUsage.Default,
            int numSamples = 1, bool hasMipmaps = false, bool gammaCorrection = false)
        {
            Internal_CreateInstance(this, format, width, height, usage, numSamples, hasMipmaps, gammaCorrection);
        }

        /// <summary>
        /// Returns pixels for the specified mip level of the specified face. Pixels will be read from system memory, 
        /// which means the texture has to be created with <see cref="TextureUsage.CPUCached"/>. If the texture was updated 
        /// from the GPU the pixels retrieved from this method will not reflect that, and you should use 
        /// <see cref="GetGPUPixels"/> instead.
        /// </summary>
        /// <param name="face">Face of the cube to access.</param>
        /// <param name="mipLevel">Mip level to retrieve pixels for. Top level (0) is the highest quality.</param>
        /// <returns>A set of pixels for the specified mip level.</returns>
        public PixelData GetPixels(CubeFace face = CubeFace.PositiveX, int mipLevel = 0)
        {
            return Internal_GetPixels(mCachedPtr, face, mipLevel);
        }

        /// <summary>
        /// Sets pixels for the specified mip level of the specified face..
        /// </summary>
        /// <param name="data">Pixels to assign to the specified mip level. Pixel data must match the mip level size
        ///                    and texture pixel format.</param>
        /// <param name="face">Face of the cube to access.</param>
        /// <param name="mipLevel">Mip level to set pixels for. Top level (0) is the highest quality.</param>
        public void SetPixels(PixelData data, CubeFace face = CubeFace.PositiveX, int mipLevel = 0)
        {
            Internal_SetPixels(mCachedPtr, data, face, mipLevel);
        }

        /// <summary>
        /// Reads texture pixels directly from the GPU. This is similar to <see cref="GetPixels"/> but the texture doesn't
        /// need to be created with <see cref="TextureUsage.CPUCached"/>, and the data will contain any updates performed by
        /// the GPU. This method can be potentially slow as it introduces a CPU-GPU synchronization point. Additionally
        /// this method is asynchronous which means the data is not available immediately.
        /// </summary>
        /// <param name="face">Face of the cube to access.</param>
        /// <param name="mipLevel">Mip level to retrieve pixels for. Top level (0) is the highest quality.</param>
        /// <returns><see cref="AsyncOp"/> object that will contain <see cref="PixelData"/> object when the operation 
        ///          completes.</returns>
        public AsyncOp GetGPUPixels(CubeFace face = CubeFace.PositiveX, int mipLevel = 0)
        {
            return Internal_GetGPUPixels(mCachedPtr, face, mipLevel);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(TextureCube instance, PixelFormat format, int width,
            int height, TextureUsage usage, int numSamples, bool hasMipmaps, bool gammaCorrection);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PixelData Internal_GetPixels(IntPtr thisPtr, CubeFace face, int mipLevel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern AsyncOp Internal_GetGPUPixels(IntPtr thisPtr, CubeFace face, int mipLevel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPixels(IntPtr thisPtr, PixelData data, CubeFace face, int mipLevel);
    }

    /// <summary>
    /// Indices for the faces of a cube texture.
    /// </summary>
    public enum CubeFace
    {
        PositiveX = 0,
        NegativeX = 1,
        PositiveY = 2,
        NegativeY = 3,
        PositiveZ = 4,
        NegativeZ = 5,
    }
}
