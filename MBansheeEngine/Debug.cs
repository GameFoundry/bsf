using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public sealed class Debug
    {
        public static void Log(object message)
        {
            Internal_Log(message.ToString());
        }

        public static void LogWarning(object message)
        {
            Internal_LogWarning(message.ToString());
        }

        public static void LogError(object message)
        {
            Internal_LogError(message.ToString());
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_Log(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_LogWarning(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Component Internal_LogError(string message);
    }
}
