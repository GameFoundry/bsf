using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    // Note: This must match C++ ShaderParameterType enum
    public enum ShaderParameterType
    {
        Float, Vector2, Vector3, Vector4, Color,
        Matrix3, Matrix4, Texture2D, 
        Texture3D, TextureCube, Sampler
    }

    public struct ShaderParameter
    {
        public ShaderParameter(string name, ShaderParameterType type)
        {
            this.name = name;
            this.type = type;
        }

        public string name;
        public ShaderParameterType type;
    }

    public class Shader : Resource
    {
        // For internal use by the runtime
        private Shader()
        { }

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
