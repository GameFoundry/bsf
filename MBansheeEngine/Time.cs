using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public static class Time
    {
        public static float elapsed
        {
            get
            {
                return Internal_GetElapsed();
            }
        }

        public static float frameDelta
        {
            get
            {
                return Internal_GetFrameDelta();
            }
        }

        public static int frameNumber
        {
            get
            {
                return Internal_GetFrameNumber();
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetElapsed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetFrameDelta();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetFrameNumber();
    }
}
