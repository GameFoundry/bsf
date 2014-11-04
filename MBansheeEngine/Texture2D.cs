using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class Texture2D : Texture
    {
        // For internal use by the runtime
        private Texture2D()
        { }

        public Texture2D(PixelFormat format, int width, int height, TextureUsage usage = TextureUsage.Default, 
            int numSamples = 1, bool hasMipmaps = false, bool gammaCorrection = false)
        {
            Internal_CreateInstance(this, format, width, height, usage, numSamples, hasMipmaps, gammaCorrection);
        }

        public PixelData GetPixels(int mipLevel = 0)
        {
            return Internal_GetPixels(mCachedPtr, mipLevel);
        }

        public void SetPixels(PixelData data, int mipLevel = 0)
        {
            Internal_SetPixels(mCachedPtr, data, mipLevel);
        }

        public AsyncOp GetGPUPixels(int mipLevel = 0)
        {
            return Internal_GetGPUPixels(mCachedPtr, mipLevel);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(Texture2D instance, PixelFormat format, int width, 
            int height, TextureUsage usage, int numSamples, bool hasMipmaps, bool gammaCorrection);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PixelData Internal_GetPixels(IntPtr thisPtr, int mipLevel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern AsyncOp Internal_GetGPUPixels(IntPtr thisPtr, int mipLevel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPixels(IntPtr thisPtr, PixelData data, int mipLevel);
    }
}
