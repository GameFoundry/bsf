using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    /// <summary>
    /// Possible types of debug messages.
    /// </summary>
    public enum DebugMessageType // Note: Must match C++ enum DebugChannel
    {
        Info, Warning, Error
    }

    /// <summary>
    /// Utility class providing various debug functionality.
    /// </summary>
    public sealed class Debug
    {
        /// <summary>
        /// Triggered when a new message is added to the debug log.
        /// </summary>
        public static Action<DebugMessageType, string> OnAdded;

        /// <summary>
        /// Logs a new informative message to the global debug log.
        /// </summary>
        /// <param name="message">Message to log.</param>
        public static void Log(object message)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine(message.ToString());
            sb.Append(GetStackTrace());

            Internal_Log(sb.ToString());
        }

        /// <summary>
        /// Logs a new warning message to the global debug log.
        /// </summary>
        /// <param name="message">Message to log.</param>
        public static void LogWarning(object message)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine(message.ToString());
            sb.Append(GetStackTrace());

            Internal_LogWarning(sb.ToString());
        }

        /// <summary>
        /// Logs a new error message to the global debug log.
        /// </summary>
        /// <param name="message">Message to log.</param>
        public static void LogError(object message)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine(message.ToString());
            sb.Append(GetStackTrace());

            Internal_LogError(sb.ToString());
        }

        /// <summary>
        /// Returns the stack trace of the current point in code.
        /// </summary>
        /// <returns>String containing the stack trace.</returns>
        public static string GetStackTrace()
        {
            StackTrace stackTrace = new StackTrace(1, true);

            StackFrame[] frames = stackTrace.GetFrames();
            if (frames == null)
                return "";

            StringBuilder sb = new StringBuilder();
            foreach (var frame in frames)
            {
                MethodBase method = frame.GetMethod();
                if (method == null)
                    continue;

                Type parentType = method.DeclaringType;
                if (parentType == null)
                    continue;

                sb.Append("\tat " + parentType.Name + "." + method.Name + "(");

                ParameterInfo[] methodParams = method.GetParameters();
                for(int i = 0; i < methodParams.Length; i++)
                {
                    if (i > 0)
                        sb.Append(", ");

                    sb.Append(methodParams[i].ParameterType.Name);
                }

                sb.Append(")");

                string ns = parentType.Namespace;
                string fileName = frame.GetFileName();
                if (!string.IsNullOrEmpty(fileName))
                {
                    int line = frame.GetFileLineNumber();
                    int column = frame.GetFileColumnNumber();

                    sb.Append(" in " + fileName + ", line " + line + ", column " + column + ", namespace " + ns);
                }
                else
                {
                    if (!string.IsNullOrEmpty(ns))
                        sb.Append(" in namespace " + ns);
                }

                sb.AppendLine();
            }

            return sb.ToString();
        }

        /// <summary>
        /// Triggered by the runtime when a new message is added to the debug log.
        /// </summary>
        /// <param name="type">Type of the message that was added.</param>
        /// <param name="message">Text of the message.</param>
        private static void Internal_OnAdded(DebugMessageType type, string message)
        {
            if (OnAdded != null)
                OnAdded(type, message);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_Log(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_LogWarning(string message);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_LogError(string message);
    }
}
