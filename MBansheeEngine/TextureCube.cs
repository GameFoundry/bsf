using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class TextureCube : Texture
    {
        // For internal use by the runtime
        private TextureCube()
        { }

        public TextureCube(PixelFormat format, int width, int height, TextureUsage usage = TextureUsage.Default,
            int numSamples = 1, bool hasMipmaps = false, bool gammaCorrection = false)
        {
            Internal_CreateInstance(this, format, width, height, usage, numSamples, hasMipmaps, gammaCorrection);
        }

        public PixelData GetPixels(CubeFace face = CubeFace.PositiveX, int mipLevel = 0)
        {
            return Internal_GetPixels(mCachedPtr, face, mipLevel);
        }

        public void SetPixels(PixelData data, CubeFace face = CubeFace.PositiveX, int mipLevel = 0)
        {
            Internal_SetPixels(mCachedPtr, data, face, mipLevel);
        }

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
