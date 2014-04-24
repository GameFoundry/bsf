using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public sealed class Debug
    {
        public static void Log(string message)
        {
            Internal_Log(message);
        }

        public static void LogWarning(string message)
        {
            Internal_LogWarning(message);
        }

        public static void LogError(string message)
        {
            Internal_LogError(message);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_Log(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_LogWarning(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_LogError(string message);
    }
}
