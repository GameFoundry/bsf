using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    /// <summary>
    /// Render target is a frame buffer or a texture that the render system renders the scene to.
    /// </summary>
    public class RenderTarget : ScriptObject
    {
        /// <summary>
        /// Returns the width of the render target, in pixels.
        /// </summary>
        public int Width
        {
            get
            {
                int value;
                Internal_GetWidth(mCachedPtr, out value);
                return value;
            }
        }

        /// <summary>
        /// Returns the height of the render target, in pixels.
        /// </summary>
        public int Height
        {
            get
            {
                int value;
                Internal_GetHeight(mCachedPtr, out value);
                return value;
            }
        }

        /// <summary>
        /// Returns true if pixels written to the render target will be gamma corrected.
        /// </summary>
        public bool GammaCorrection
        {
            get
            {
                bool value;
                Internal_GetGammaCorrection(mCachedPtr, out value);
                return value;
            }
        }

        /// <summary>
        /// Gets the number of samples used for multisampling (0 or 1 if multisampling is not used).
        /// </summary>
        public int SampleCount
        {
            get
            {
                int value;
                Internal_GetSampleCount(mCachedPtr, out value);
                return value;
            }
        }

        /// <summary>
        /// Controls in what order is the render target rendered to compared to other render targets. Targets with higher 
        /// priority will be rendered before ones with lower priority.
        /// </summary>
        public int Priority
        {
            get
            {
                int value;
                Internal_GetPriority(mCachedPtr, out value);
                return value;
            }

            set
            {
                Internal_SetPriority(mCachedPtr, value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetWidth(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetHeight(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetGammaCorrection(IntPtr thisPtr, out bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetSampleCount(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetPriority(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPriority(IntPtr thisPtr, int value);
    }
}