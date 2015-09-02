using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    /// <summary>
    /// Material that controls how objects are rendered. It is represented by a shader and parameters used to set up that 
    /// shader. It provides a simple interface for manipulating the parameters.
    /// </summary>
    public class Material : Resource
    {
        /// <summary>
        /// Creates a new empty material that references no shader.
        /// </summary>
        public Material()
        {
            Internal_CreateInstance(this, IntPtr.Zero);
        }

        /// <summary>
        /// Creates a new material with the specified shader.
        /// </summary>
        /// <param name="shader">Shader to initialize the material with.</param>
        public Material(Shader shader)
        {
            IntPtr nativeShader = IntPtr.Zero;
            if (shader != null)
                nativeShader = shader.GetCachedPtr();

            Internal_CreateInstance(this, nativeShader);
        }

        /// <summary>
        /// Shader used by the material. Best technique from the shader will be used for rendering, depending on currently
        /// active renderer and render API.
        /// </summary>
        public Shader Shader
        {
            get { return Internal_GetShader(mCachedPtr); }
            set
            {
                IntPtr nativeShader = IntPtr.Zero;
                if (value != null)
                    nativeShader = value.GetCachedPtr();

                Internal_SetShader(mCachedPtr, nativeShader); 
            }
        }

        /// <summary>
        /// Assigns a float value to the shader parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <param name="value">Value of the parameter.</param>
        public void SetFloat(string name, float value)
        {
            Internal_SetFloat(mCachedPtr, name, value);
        }

        /// <summary>
        /// Assigns a 2D vector to the shader parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <param name="value">Value of the parameter.</param>
        public void SetVector2(string name, Vector2 value)
        {
            Internal_SetVector2(mCachedPtr, name, value);
        }

        /// <summary>
        /// Assigns a 3D vector to the shader parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <param name="value">Value of the parameter.</param>
        public void SetVector3(string name, Vector3 value)
        {
            Internal_SetVector3(mCachedPtr, name, value);
        }

        /// <summary>
        /// Assigns a 4D vector to the shader parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <param name="value">Value of the parameter.</param>
        public void SetVector4(string name, Vector4 value)
        {
            Internal_SetVector4(mCachedPtr, name, value);
        }

        /// <summary>
        /// Assigns a 3x3 matrix to the shader parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <param name="value">Value of the parameter.</param>
        public void SetMatrix3(string name, Matrix3 value)
        {
            Internal_SetMatrix3(mCachedPtr, name, value);
        }

        /// <summary>
        /// Assigns a 4x4 matrix to the shader parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <param name="value">Value of the parameter.</param>
        public void SetMatrix4(string name, Matrix4 value)
        {
            Internal_SetMatrix4(mCachedPtr, name, value);
        }

        /// <summary>
        /// Assigns a color to the shader parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <param name="value">Value of the parameter.</param>
        public void SetColor(string name, Color value)
        {
            Internal_SetColor(mCachedPtr, name, value);
        }

        /// <summary>
        /// Assigns a 2D texture to the shader parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <param name="value">Value of the parameter.</param>
        public void SetTexture2D(string name, Texture2D value)
        {
            IntPtr texturePtr = IntPtr.Zero;
            if (value != null)
                texturePtr = value.GetCachedPtr();

            Internal_SetTexture2D(mCachedPtr, name, texturePtr);
        }

        /// <summary>
        /// Assigns a 3D texture to the shader parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <param name="value">Value of the parameter.</param>
        public void SetTexture3D(string name, Texture3D value)
        {
            IntPtr texturePtr = IntPtr.Zero;
            if (value != null)
                texturePtr = value.GetCachedPtr();

            Internal_SetTexture3D(mCachedPtr, name, texturePtr);
        }

        /// <summary>
        /// Assigns a cube texture to the shader parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <param name="value">Value of the parameter.</param>
        public void SetTextureCube(string name, TextureCube value)
        {
            IntPtr texturePtr = IntPtr.Zero;
            if (value != null)
                texturePtr = value.GetCachedPtr();

            Internal_SetTextureCube(mCachedPtr, name, texturePtr);
        }

        /// <summary>
        /// Returns a float value assigned with the parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <returns>Value of the parameter.</returns>
        public float GetFloat(string name)
        {
            return Internal_GetFloat(mCachedPtr, name);
        }

        /// <summary>
        /// Returns a 2D vector assigned with the parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <returns>Value of the parameter.</returns>
        public Vector2 GetVector2(string name)
        {
            return Internal_GetVector2(mCachedPtr, name);
        }

        /// <summary>
        /// Returns a 3D vector assigned with the parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <returns>Value of the parameter.</returns>
        public Vector3 GetVector3(string name)
        {
            return Internal_GetVector3(mCachedPtr, name);
        }

        /// <summary>
        /// Returns a 4D vector assigned with the parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <returns>Value of the parameter.</returns>
        public Vector4 GetVector4(string name)
        {
            return Internal_GetVector4(mCachedPtr, name);
        }

        /// <summary>
        /// Returns a 3x3 matrix assigned with the parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <returns>Value of the parameter.</returns>
        public Matrix3 GetMatrix3(string name)
        {
            return Internal_GetMatrix3(mCachedPtr, name);
        }

        /// <summary>
        /// Returns a 4x4 matrix assigned with the parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <returns>Value of the parameter.</returns>
        public Matrix4 GetMatrix4(string name)
        {
            return Internal_GetMatrix4(mCachedPtr, name);
        }

        /// <summary>
        /// Returns a color assigned with the parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <returns>Value of the parameter.</returns>
        public Color GetColor(string name)
        {
            return Internal_GetColor(mCachedPtr, name);
        }

        /// <summary>
        /// Returns a 2D texture assigned with the parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <returns>Value of the parameter.</returns>
        public Texture2D GetTexture2D(string name)
        {
            return Internal_GetTexture2D(mCachedPtr, name);
        }

        /// <summary>
        /// Returns a 3D texture assigned with the parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <returns>Value of the parameter.</returns>
        public Texture3D GetTexture3D(string name)
        {
            return Internal_GetTexture3D(mCachedPtr, name);
        }

        /// <summary>
        /// Returns a cube texture assigned with the parameter with the specified name.
        /// </summary>
        /// <param name="name">Name of the shader parameter.</param>
        /// <returns>Value of the parameter.</returns>
        public TextureCube GetTextureCube(string name)
        {
            return Internal_GetTextureCube(mCachedPtr, name);
        }

        /// <summary>
        /// Creates a deep copy of the material.
        /// </summary>
        /// <returns>A new object with exact data as this object.</returns>
        public Material Clone()
        {
            return Internal_Clone(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(Material instance, IntPtr shader);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Shader Internal_GetShader(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetShader(IntPtr nativeInstance, IntPtr shader);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetFloat(IntPtr nativeInstance, string name, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetVector2(IntPtr nativeInstance, string name, Vector2 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetVector3(IntPtr nativeInstance, string name, Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetVector4(IntPtr nativeInstance, string name, Vector4 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMatrix3(IntPtr nativeInstance, string name, Matrix3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMatrix4(IntPtr nativeInstance, string name, Matrix4 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetColor(IntPtr nativeInstance, string name, Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTexture2D(IntPtr nativeInstance, string name, IntPtr value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTexture3D(IntPtr nativeInstance, string name, IntPtr value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTextureCube(IntPtr nativeInstance, string name, IntPtr value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetFloat(IntPtr nativeInstance, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector2 Internal_GetVector2(IntPtr nativeInstance, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 Internal_GetVector3(IntPtr nativeInstance, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector4 Internal_GetVector4(IntPtr nativeInstance, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Matrix3 Internal_GetMatrix3(IntPtr nativeInstance, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Matrix4 Internal_GetMatrix4(IntPtr nativeInstance, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Color Internal_GetColor(IntPtr nativeInstance, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Texture2D Internal_GetTexture2D(IntPtr nativeInstance, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Texture3D Internal_GetTexture3D(IntPtr nativeInstance, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern TextureCube Internal_GetTextureCube(IntPtr nativeInstance, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Material Internal_Clone(IntPtr nativeInstance);
    }
}
