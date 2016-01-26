//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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
        /// Returns the name of the parameter variable.
        /// </summary>
        public string Name { get { return name; } }

        /// <summary>
        /// Returns the data type of the parameter.
        /// </summary>
        public ShaderParameterType Type { get { return type; } }

        /// <summary>
        /// Determines is parameter managed internally be the renderer, or is it expected to be set by the user.
        /// </summary>
        public bool Internal { get { return isInternal; } }

        private string name;
        private ShaderParameterType type;
        private bool isInternal;

        /// <summary>
        /// Creates a new shader parameter.
        /// </summary>
        /// <param name="name">Name of the parameter variable.</param>
        /// <param name="type">Data type of the parameter.</param>
        /// <param name="isInternal">Determines is parameter managed internally be the renderer, or is expected to be set 
        ///                          by the user.</param>
        internal ShaderParameter(string name, ShaderParameterType type, bool isInternal)
        {
            this.name = name;
            this.type = type;
            this.isInternal = isInternal;
        }
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
                bool[] visibility;

                Internal_GetShaderParameters(mCachedPtr, out names, out types, out visibility);

                ShaderParameter[] parameters = new ShaderParameter[names.Length];
                for (int i = 0; i < names.Length; i++)
                {
                    parameters[i] = new ShaderParameter(names[i], types[i], visibility[i]);
                }

                return parameters;
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetShaderParameters(IntPtr nativeInstance, out string[] names,
            out ShaderParameterType[] types, out bool[] visibility);
    }
}
