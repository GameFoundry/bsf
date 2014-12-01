using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class RenderTexture2D : RenderTexture
    {
        public RenderTexture2D(PixelFormat format, int width, int height, int numSamples = 1, 
            bool gammaCorrection = false, bool createDepth = true, PixelFormat depthStencilFormat = PixelFormat.D24S8)
        {
            Internal_CreateDetailed(this, format, width, height, numSamples, gammaCorrection, createDepth, depthStencilFormat);
        }

        public RenderTexture2D(Texture2D colorSurface, Texture2D depthStencilSurface = null)
        {
            IntPtr[] colorSurfaceInstances = new IntPtr[1];
            colorSurfaceInstances[0] = colorSurface.GetCachedPtr();

            IntPtr depthStencilInstance = IntPtr.Zero;
            if (depthStencilSurface != null)
                depthStencilInstance = depthStencilSurface.GetCachedPtr();

            Internal_Create(this, colorSurfaceInstances, depthStencilInstance);
        }

        public RenderTexture2D(Texture2D[] colorSurfaces, Texture2D depthStencilSurface = null)
        {
            IntPtr[] colorSurfaceInstances = new IntPtr[colorSurfaces.Length];

            for(int i = 0; i < colorSurfaces.Length; i++)
                colorSurfaceInstances[i] = colorSurfaces[i] != null ? colorSurfaces[i].GetCachedPtr() : IntPtr.Zero;

            IntPtr depthStencilInstance = IntPtr.Zero;
            if (depthStencilSurface != null)
                depthStencilInstance = depthStencilSurface.GetCachedPtr();

            Internal_Create(this, colorSurfaceInstances, depthStencilInstance);
        }

        public Texture2D colorSurface
        {
            get
            {
                Texture2D[] surfaces;
                Internal_GetColorSurfaces(mCachedPtr, out surfaces);
                return surfaces[0];
            }
        }

        public Texture2D[] colorSurfaces
        {
            get
            {
                Texture2D[] surfaces;
                Internal_GetColorSurfaces(mCachedPtr, out surfaces);
                return surfaces;
            }
        }

        public Texture2D depthStencilSurface
        {
            get
            {
                Texture2D surface;
                Internal_GetDepthStencilSurface(mCachedPtr, out surface);
                return surface;
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateDetailed(RenderTexture2D instance, PixelFormat format, 
            int width, int height, int numSamples, bool gammaCorrection, bool createDepth, PixelFormat depthStencilFormat);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Create(RenderTexture2D instance, IntPtr[] colorSurfaces, IntPtr depthStencilSurface);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetColorSurfaces(IntPtr thisPtr, out Texture2D[] surfaces);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetDepthStencilSurface(IntPtr thisPtr, out Texture2D surface);
    }
}
