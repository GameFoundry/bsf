using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Type of parameters that can be defined in a shader.
    /// </summary>
    public enum ShaderParameterType // Note: Must match C++ ShaderParameterType enum
    {
        Float, Vector2, Vector3, Vector4, Color,
        Matrix3, Matrix4, Texture2D, 
        Texture3D, TextureCube, Sampler
    }

    /// <summary>
    /// Contains information about a shader parameter.
    /// </summary>
    public struct ShaderParameter
    {
        /// <summary>
        /// Creates a new shader parameter.
        /// </summary>
        /// <param name="name">Name of the parameter.</param>
        /// <param name="type">Type of the parameter.</param>
        internal ShaderParameter(string name, ShaderParameterType type)
        {
            this.name = name;
            this.type = type;
        }

        public string name;
        public ShaderParameterType type;
    }

    /// <summary>
    /// Contains definitions of GPU programs used for rendering, as well as a set of global parameters to control those
    /// programs.
    /// </summary>
    public class Shader : Resource
    {
        /// <summary>
        /// Constuctor for internal runtime use only.
        /// </summary>
        private Shader()
        { }

        /// <summary>
        /// Returns data about all parameters available in the shader.
        /// </summary>
        public ShaderParameter[] Parameters
        {
            get
            {
                string[] names;
                ShaderParameterType[] types;

                Internal_GetShaderParameters(mCachedPtr, out names, out types);

                ShaderParameter[] parameters = new ShaderParameter[names.Length];
                for (int i = 0; i < names.Length; i++)
                {
                    parameters[i] = new ShaderParameter(names[i], types[i]);
                }

                return parameters;
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetShaderParameters(IntPtr nativeInstance, out string[] names, out ShaderParameterType[] types);
    }
}
