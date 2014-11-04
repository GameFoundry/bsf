using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace BansheeEngine
{
    public static class PixelUtility
    {
        public static int GetMemorySize(int width, int height, int depth, PixelFormat format)
        {
            int value;
            Internal_GetMemorySize(width, height, depth, format, out value);
            return value;
        }

        public static bool HasAlpha(PixelFormat format)
        {
            bool value;
            Internal_HasAlpha(format, out value);
            return value;
        }

        public static bool IsFloatingPoint(PixelFormat format)
        {
            bool value;
            Internal_IsFloatingPoint(format, out value);
            return value;
        }

        public static bool IsCompressed(PixelFormat format)
        {
            bool value;
            Internal_IsCompressed(format, out value);
            return value;
        }

        public static bool IsDepth(PixelFormat format)
        {
            bool value;
            Internal_IsDepth(format, out value);
            return value;
        }

        public static int GetMaxMipmaps(int width, int height, int depth, PixelFormat format)
        {
            int value;
            Internal_GetMaxMipmaps(width, height, depth, format, out value);
            return value;
        }

        public static PixelData ConvertFormat(PixelData source, PixelFormat newFormat)
        {
            return Internal_ConvertFormat(source, newFormat);
        }

        public static PixelData Compress(PixelData source, CompressionOptions options)
        {
            return Internal_Compress(source, options);
        }

		public static PixelData[] GenerateMipmaps(PixelData source, MipMapGenOptions options)
        {
            return Internal_GenerateMipmaps(source, options);
        }

        public static PixelData Scale(PixelData source, PixelVolume newSize, ScaleFilter filter = ScaleFilter.Linear)
        {
            return Internal_Scale(source, newSize, filter);
        }

        public static void ApplyGamma(PixelData source, float gamma)
        {
            Internal_ApplyGamma(source, gamma);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetMemorySize(int width, int height, int depth, PixelFormat format, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_HasAlpha(PixelFormat format, out bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_IsFloatingPoint(PixelFormat format, out bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_IsCompressed(PixelFormat format, out bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_IsDepth(PixelFormat format, out bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetMaxMipmaps(int width, int height, int depth, PixelFormat format, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PixelData Internal_ConvertFormat(PixelData source, PixelFormat newFormat);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PixelData Internal_Compress(PixelData source, CompressionOptions options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PixelData[] Internal_GenerateMipmaps(PixelData source, MipMapGenOptions options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PixelData Internal_Scale(PixelData source, PixelVolume newSize, ScaleFilter filter);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ApplyGamma(PixelData source, float gamma);
    }

    // Note: IDs must match C++ enum PixelUtil::Filter
    public enum ScaleFilter
    {
        Nearest,
        Linear
    };

    // Note: IDs must match C++ enum CompressionQuality
	public enum CompressionQuality
	{
		Fastest,
		Normal,
		Production,
		Highest
	};

    // Note: IDs must match C++ enum AlphaMode
	public enum AlphaMode
	{
		None,
		Transparency,
		Premultiplied
	};

    // Note: IDs must match C++ enum MipMapWrapMode
	public enum MipMapWrapMode
	{
		Mirror,
		Repeat,
		Clamp
	};

	// Note: IDs must match C++ enum MipMapFilter
	public enum MipMapFilter
	{
		Box,
		Triangle,
		Kaiser
	};

    // Note: Layout must match C++ struct CompressionOptions
    [StructLayout(LayoutKind.Sequential)]
	public struct CompressionOptions
	{
		public PixelFormat format;
	    public AlphaMode alphaMode;
		public bool isNormalMap;
		public bool isSRGB;
		public CompressionQuality quality;
	};

    // Note: Layout must match C++ struct MipMapGenOptions
    [StructLayout(LayoutKind.Sequential)]
	public struct MipMapGenOptions
	{
		public MipMapFilter filter;
		public MipMapWrapMode wrapMode;
		public bool isNormalMap;
		public bool normalizeMipmaps;
	};
}
